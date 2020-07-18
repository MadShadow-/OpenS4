#pragma once

#include <map>
#include <memory>
#include <vector>

#include "../../Logger/Logger.hpp"
#include "IndexList.hpp"
#include "Palette.hpp"

#include <iostream>

namespace OpenS4::Import {

class ImageData {
    uint32_t m_width;
    uint32_t m_height;
    std::vector<uint32_t> m_data;

    __inline uint32_t getIndex(uint32_t x, uint32_t y) const {
        return y * m_width + x;
    }

    uint32_t m_palette;

   public:
    ImageData(uint32_t width = 0, uint32_t height = 0, uint32_t palette = 0) {
        m_width = width;
        m_height = height;
        m_data.resize(m_width * m_height);

        m_palette = palette;
    }

    void setPixel(uint32_t x, uint32_t y, uint32_t color) {
        m_data[getIndex(x, y)] = color;
    }

    void setPixel(uint32_t idx, uint32_t color) { m_data[idx] = color; }

    auto getHeight() const { return m_height; }
    auto getWidth() const { return m_width; }

    auto getColor(uint32_t x, uint32_t y) const {
        return m_data[getIndex(x, y)];
    }

    auto getPalette() const { return m_palette; }

    auto getData() const { return m_data.data(); }
};

class IGraphics {
   protected:
    ImageData readImage(std::shared_ptr<Reader> reader, uint32_t offset,
                        std::shared_ptr<PaletteCollection>& pal,
                        uint32_t paletteOffset) {
        uint16_t imgHeaderType = reader->read_word(offset);
        // do not increase offset here!

        auto type = 0;

        auto data_offset = offset;

        auto width = 0;
        auto height = 0;
        auto top = 0;
        auto left = 0;
        auto flag1 = 0;
        auto flag2 = 0;

        if (imgHeaderType > 860) {
            width = reader->read_byte(offset++);
            height = reader->read_byte(offset++);
            left = reader->read_byte(offset++);
            top = reader->read_byte(offset++);

            type = 0;

            flag1 = reader->read_word(offset);
            offset += 2;
            flag2 = reader->read_word(offset);
            offset += 2;

            data_offset += 8;
        } else {
            width = reader->read_word(offset);
            offset += 2;
            height = reader->read_word(offset);
            offset += 2;
            left = reader->read_word(offset);
            offset += 2;
            top = reader->read_word(offset);
            offset += 2;

            type = reader->read_byte(offset++);

            flag1 = reader->read_byte(offset++);
            flag2 = reader->read_int32(offset);
            offset += 4;

            data_offset += 12;
        }

        ImageData imgData(width, height, paletteOffset);

        bool print_error = true;

        if (type != 32) {
            // run length encoding

            auto first_black = 0;
            auto first_player_color = 0;

            for (int i = 2; i < 255; i++) {
                if (pal->get_color(paletteOffset + i) == 0xff000000) {
                    if (first_black) {
                        first_player_color = i;
                        break;
                    } else {
                        first_black = i;
                    }
                }
            }

            auto position = data_offset;

            auto j = 0;

            while (j < width * height) {
                uint8_t value = 0;

                if (position < reader->size()) {
                    value = reader->read_byte(position++);
                } else {
                    if (position == reader->size() && print_error) {
                        OpenS4::getLogger().warn(
                            "Position exceeded size of image at offset %d",
                            offset);
                        print_error = false;
                    }

                    value = 1;
                }

                auto color = 0;
                auto count = 1;

                bool is_shadow = false;

                if (value <= 1) {
                    if (position < reader->size()) {
                        count = reader->read_byte(position++);
                    } else {
                        if (position == reader->size() && print_error) {
                            OpenS4::getLogger().warn(
                                "Position exceeded size of image at offset %d",
                                offset);
                            print_error = false;
                        }
                        count = 1;
                    }

                    if (value == 0) {
                        color = 0x00000000;
                    } else {
                        is_shadow = true;
                        // color = 0xFF000000; // Shadow
                    }
                } else {
                    color = pal->get_color(paletteOffset + value);
                }

                if (value >= first_player_color) {
                    if (color == 0xff000000 && !is_shadow) {
                        auto f = [](float a, float b, float c, float d, int v) {
                            return c + ((d - c) / (b - a)) * (v - a);
                        };

                        // Scale player color to a range of "transparency"
                        // exclude 0 and 255 because 0 means transparent which
                        // is needed for transparent pixels and 255 is needed
                        // for black itself
                        color = 0x00000000;
                        color = ((uint8_t)f(196, 223, 1, 254, value)) << 24;
                    }
                }

                for (auto i = 0; (i < count) && (j < width * height); i++) {
                    if (!is_shadow) {
                        imgData.setPixel(j++, color);
                    } else {
                        auto x = j - (j / width) * width;
                        auto y = (j / width);

                        if (y % 2 == 0) {
                            if (x % 2 == 1)
                                color = 0xFF000000;
                            else
                                color = 0;
                        } else if (x % 2 == 0)
                            color = 0xFF000000;
                        else
                            color = 0;

                        color = 0x7f010101;

                        imgData.setPixel(j++, color);
                    }
                }
            }
        } else {
            // no run length encoding
            auto length = width * height;

            auto j = 0;
            ImageData imgData(width, height, paletteOffset);

            auto pos = data_offset;

            while (j < length) {
                auto value = reader->read_byte(pos++);
                auto color = pal->get_color(paletteOffset + value);
                imgData.setPixel(j++, color);
            }
        }

        return imgData;
    }

   public:
    virtual uint32_t getNumberOfImages() = 0;
    virtual ImageData getImage(int offset) = 0;
    virtual u32 getPalette(u32 offset) = 0;
};

class GraphicsLazy : public IGraphics {
    std::vector<uint32_t> m_header;

    std::vector<ImageData> m_images;

    std::shared_ptr<GraphicsIndexList> m_gil;
    std::shared_ptr<PaletteCollection> m_pal;
    std::shared_ptr<JobIndexList> m_jil;
    std::shared_ptr<DirectionIndexList> m_dil;

    std::shared_ptr<Reader> m_reader;

   public:
    GraphicsLazy(std::shared_ptr<Reader> reader,
                 std::shared_ptr<GraphicsIndexList> gil,
                 std::shared_ptr<PaletteCollection> pal) {
        m_reader = reader;
        m_gil = gil;
        m_pal = pal;
    }

    GraphicsLazy(std::shared_ptr<Reader> reader,
                 std::shared_ptr<GraphicsIndexList> gil,
                 std::shared_ptr<PaletteCollection> pal,
                 std::shared_ptr<JobIndexList> jil,
                 std::shared_ptr<DirectionIndexList> dil) {
        m_reader = reader;
        m_gil = gil;
        m_pal = pal;
        m_jil = jil;
        m_dil = dil;
    }

    uint32_t getNumberOfImages() override { return m_gil->getImageCount(); }

    ImageData getImage(int offset) override {
        auto gfx_offset = m_gil->get_offset(offset);

        auto palette = 0;

        if (m_jil) {
            palette = 0;
            for (auto i = offset; i > 0; i--) {
                auto pal = m_jil->lookup(m_dil->lookup(i));
                if (pal) {
                    palette = pal;
                    break;
                }
            }
        } else {
            palette = offset;
        }

        return readImage(m_reader, gfx_offset, m_pal,
                         m_pal->get_offset(palette));
    }

    u32 getPalette(u32 offset) override {
        auto gfx_offset = m_gil->get_offset(offset);

        auto palette = 0;

        if (m_jil) {
            palette = 0;
            for (auto i = offset; i > 0; i--) {
                auto pal = m_jil->lookup(m_dil->lookup(i));
                if (pal) {
                    palette = pal;
                    break;
                }
            }
        } else {
            palette = offset;
        }

        return palette;
    }
};

class BackgroundGraphics {
    std::vector<uint8_t> m_header;

    std::vector<ImageData> readImages(std::shared_ptr<Reader> reader,
                                      uint32_t type, uint32_t row_count,
                                      uint32_t offset) {
        auto width =
            type == 0 ? 128 : (type == 1 ? 256 : (type == 2 ? 128 : 256));
        auto height = width;  // *row_count;

        auto pos = offset;

        std::vector<ImageData> result;

        if (type < 2) {
            for (int i = 0; i < row_count; i++) {
                auto j = 0;

                ImageData img(width, height);

                while (j < width * height) {
                    auto value1 = reader->read_byte(pos++);
                    auto value2 = reader->read_byte(pos++);

                    uint8_t buf[4];
                    buf[0] = value2 & 0xF8;                           // r
                    buf[1] = (value1 >> 3) | ((value2 << 5) & 0xFC);  // g
                    buf[2] = (value1 << 3) & 0xF8;                    // b
                    buf[3] = 255;                                     // alpha

                    int32_t v = *(int32_t*)buf;

                    img.setPixel(j++, v);
                }

                result.push_back(img);
            }
        } else {
            ImageData img(width, height);

            auto chunk_length = width * width;
            int pos = offset;

            auto j = 0;
            auto c = 0;

            pos -= 256 * 3;

            struct Color {
                uint8_t r;
                uint8_t g;
                uint8_t b;
            };

            std::vector<Color> palette;
            auto get_color = [&palette](int idx) {
                Color col = palette[idx];
                return col.r | (col.g << 8) | (col.b << 16) | (255 << 24);
            };

            while (j < width * height) {
                if (c <= 0) {
                    pos += 256 * 3;
                    palette.clear();
                    int p = pos + chunk_length;
                    for (auto i = 0; i < 256; i++) {
                        Color col;
                        col.r = reader->read_byte(p++);
                        col.g = reader->read_byte(p++);
                        col.b = reader->read_byte(p++);
                        palette.push_back(col);
                    }
                    c = chunk_length;
                }
                c--;

                int index = reader->read_byte(pos++);

                img.setPixel(j++, get_color(index));
            }
            result.push_back(img);
        }
        return result;
    }

    std::vector<ImageData> m_images;

   public:
    BackgroundGraphics(std::shared_ptr<Reader> reader) {
        {
            // 5 * 4 byte header
            m_header.push_back(reader->read_int32(0));
            m_header.push_back(reader->read_int32(4));
            m_header.push_back(reader->read_int32(8));
            m_header.push_back(reader->read_int32(12));
            m_header.push_back(reader->read_int32(16));
            uint32_t offset = 20;

            auto size = -1;

            while (size != 0) {
                auto image_type = reader->read_byte(offset++);
                auto flag1 = reader->read_byte(offset++);
                auto flag2 = reader->read_byte(offset++);
                auto row_count = reader->read_byte(offset++);

                size = reader->read_int32(offset);
                offset += 4;

                auto images = readImages(reader, image_type, row_count, offset);

                m_images.insert(m_images.end(), images.begin(), images.end());

                offset += size;
            }
        }
    }

    uint32_t getNumberOfImages() { return m_images.size(); }

    ImageData getImage(int offset) { return m_images[offset]; }
};

}  // namespace OpenS4::Graphics
