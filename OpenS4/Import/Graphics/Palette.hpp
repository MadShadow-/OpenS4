#pragma once

#include "IndexList.hpp"

namespace OpenS4::Import {

class PaletteCollection {
    struct Color {
        uint8_t r;
        uint8_t g;
        uint8_t b;
    };
    std::shared_ptr<PaletteIndexList> m_pil;

    std::vector<uint32_t> m_header;
    uint32_t m_number_of_palettes = 0;
    std::vector<Color> m_colors;

   public:
    PaletteCollection(std::shared_ptr<Reader> reader,
                      std::shared_ptr<PaletteIndexList> pil) {
        m_pil = pil;

        {
            // 5 * 4 byte header
            m_header.push_back(reader->read_int32(0));
            m_header.push_back(reader->read_int32(4));
            m_header.push_back(reader->read_int32(8));
            m_header.push_back(reader->read_int32(12));
            m_header.push_back(reader->read_int32(16));

            m_number_of_palettes = (reader->size() - 20) / 2;
            uint32_t offset = 20;
            for (auto i = 0; i < m_number_of_palettes; i++) {
                // Read 16 bit palette
                const uint8_t value1 = reader->read_byte(offset++);
                const uint8_t value2 = reader->read_byte(offset++);
                Color col;
                col.r = value2 & 0xF8;
                col.g = (value1 >> 3) | ((value2 << 5) & 0xFC);
                col.b = (value1 << 3) & 0xF8;
                m_colors.push_back(col);
            }
        }
    }

    uint32_t get_color(uint32_t index) {
        Color col = m_colors[index];
        return col.r | (col.g << 8) | (col.b << 16) | (255 << 24);
    }

    uint32_t get_palette_count() { return m_number_of_palettes; }

    uint32_t get_offset(int gfx_image_index) {
        return (m_pil->get_offset(gfx_image_index) - 20) / 2;
    }
};

}  // namespace OpenS4::Graphics
