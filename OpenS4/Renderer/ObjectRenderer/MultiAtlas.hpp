#pragma once

#include <rectpack2D/finders_interface.h>

#include <Core/Util.hpp>

#include "../../Import/Graphics/Image.hpp"
#include "../OpenGL.hpp"

namespace OpenS4::Renderer::ObjectRenderer
{
    class Atlas
    {
        u32 m_size = 0;

        std::vector<u32> m_imageBuffer;

        u32 m_filledPixels = 0;

        void copy_image(unsigned int* dest,
                        unsigned int* src,
                        int posX,
                        int posY,
                        int width,
                        int height,
                        int destWidth,
                        int srcWidth)
        {
            if (srcWidth < 0) srcWidth = width;

            unsigned int* destP = dest + destWidth * posY;

            for (int y = 0; y < height; y++)
            {
                unsigned int* curDestP = destP + destWidth * y + posX;
                unsigned int* srcP = src + (height - y - 1) * srcWidth;

                for (int x = width; x > 0; x--)
                {
                    *curDestP = *srcP;
                    curDestP++;
                    srcP++;
                }
            }
        }

        void copyImage(OpenS4::Import::ImageData& image,
                       rectpack2D::rect_xywh& pos)
        {
            for (auto y = 0; y < image.getHeight(); y++)
            {
                for (auto x = 0; x < image.getWidth(); x++)
                {
                    auto color = image.getColor(x, y);
                    if (color != 0)
                    {
                        m_imageBuffer[(y + pos.y) * m_size + (x + pos.x)] =
                            color;
                    }
                }
            }
        }

        GLuint m_glTextureID = 0;

       public:
        Atlas(u32 size)
        {
            m_size = size;
            m_imageBuffer.resize(size * size);
        }
        Atlas(u32 size, u32 filledPixels, std::vector<u32> imageBuffer)
        {
            m_size = size;
            m_filledPixels = filledPixels;

            glGenTextures(1, &m_glTextureID);
            glBindTexture(GL_TEXTURE_2D, m_glTextureID);
            glTexImage2D(GL_TEXTURE_2D,
                         0,
                         GL_RGBA,
                         m_size,
                         m_size,
                         0,
                         GL_RGBA,
                         GL_UNSIGNED_INT_8_8_8_8,
                         imageBuffer.data());
            // TODO error checking.
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        }
        Atlas(const Atlas&) = delete;

        ~Atlas()
        {
            if (m_glTextureID)
            {
                glDeleteTextures(1, &m_glTextureID);
            }
        }

        void copyImageData(std::vector<rectpack2D::rect_xywh>& positions,
                           std::vector<OpenS4::Import::ImageData>& images,
                           u32 startIndex,
                           u32 endIndexExcluded)
        {
            for (auto i = startIndex; i < endIndexExcluded; i++)
            {
                auto& image = images[i];
                auto& rect = positions[i];

                copyImage(image, rect);

                m_filledPixels += image.getWidth() * image.getHeight();
            }

            glGenTextures(1, &m_glTextureID);
            glBindTexture(GL_TEXTURE_2D, m_glTextureID);
            glTexImage2D(GL_TEXTURE_2D,
                         0,
                         GL_RGBA,
                         m_size,
                         m_size,
                         0,
                         GL_RGBA,
                         GL_UNSIGNED_INT_8_8_8_8,
                         m_imageBuffer.data());
            // TODO error checking.
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

            m_imageBuffer.clear();
        }

        GLuint getGLTextureID() { return m_glTextureID; }

        u32 getFilledPixels() { return m_filledPixels; }
        u32 getAtlasSize() { return m_size * m_size; }
        u32 getAtlasWidth() { return m_size; }
        u32 getAtlasHeight() { return m_size; }

        std::vector<u32> getAtlasImage()
        {
            glBindTexture(GL_TEXTURE_2D, getGLTextureID());

            std::vector<u32> pixels;
            pixels.resize(getAtlasSize());

            glGetTexImage(GL_TEXTURE_2D,
                          0,
                          GL_RGBA,
                          GL_UNSIGNED_INT_8_8_8_8,
                          pixels.data());

            return pixels;
        }

        void dump(OpenS4::Core::Util::FileWriter* writer)
        {
            writer->write_u32(m_size);
            writer->write_u32(m_filledPixels);
            writer->write_u32vector(getAtlasImage());
        }

        static Atlas* fromDump(OpenS4::Core::Util::FileReader* reader)
        {
            u32 size = reader->read_u32();
            u32 filledPixels = reader->read_u32();
            return new Atlas(size, filledPixels, reader->read_u32vector());
        }
    };

    struct UV
    {
        u32 width;
        u32 height;

        u32 atlasID;

        u32 left;
        u32 top;

        float uBegin;
        float uEnd;
        float vBegin;
        float vEnd;

        void dump(OpenS4::Core::Util::FileWriter* writer)
        {
            writer->write_u32(width);
            writer->write_u32(height);
            writer->write_u32(atlasID);
            writer->write_u32(left);
            writer->write_u32(top);

            writer->write_float(uBegin);
            writer->write_float(uEnd);
            writer->write_float(vBegin);
            writer->write_float(vEnd);
        }

        static UV fromDump(OpenS4::Core::Util::FileReader* reader)
        {
            UV uv;
            uv.width = reader->read_u32();
            uv.height = reader->read_u32();
            uv.atlasID = reader->read_u32();
            uv.left = reader->read_u32();
            uv.top = reader->read_u32();

            uv.uBegin = reader->read_float();
            uv.uEnd = reader->read_float();
            uv.vBegin = reader->read_float();
            uv.vEnd = reader->read_float();
            return uv;
        }
    };

    class MultiAtlas
    {
        OpenS4::Import::ImageData inflateImage(OpenS4::Import::ImageData& image)
        {
            auto img = &image;
            OpenS4::Import::ImageData biggerImage(img->getWidth() + 2,
                                                  img->getHeight() + 2,
                                                  img->getPalette(),
                                                  img->getLeft(),
                                                  img->getTop(),
                                                  img->getFlag1(),
                                                  img->getFlag2());
            for (int y = 0; y < biggerImage.getHeight(); y++)
            {
                for (int x = 0; x < biggerImage.getWidth(); x++)
                {
                    int yidx = y - 1;
                    int xidx = x - 1;

                    uint32_t color;

                    if (yidx < 0) yidx = 0;
                    if (xidx < 0) xidx = 0;

                    if (yidx >= img->getHeight()) yidx = img->getHeight() - 1;
                    if (xidx >= img->getWidth()) xidx = img->getWidth() - 1;

                    biggerImage.setPixel(x, y, img->getColor(xidx, yidx));
                }
            }

            return biggerImage;
        }
        OpenS4::Import::ImageData swapImage(OpenS4::Import::ImageData& image)
        {
            OpenS4::Import::ImageData img(image.getWidth(),
                                          image.getHeight(),
                                          image.getPalette(),
                                          image.getLeft(),
                                          image.getTop(),
                                          image.getFlag1(),
                                          image.getFlag2());

            auto data = image.getData();
            for (int i = 0; i < image.getWidth() * image.getHeight(); i++)
            {
                uint32_t num = *(data + i);
                uint32_t swapped =
                    ((num >> 24) & 0xff) |       // move byte 3 to byte 0
                    ((num << 8) & 0xff0000) |    // move byte 1 to byte 2
                    ((num >> 8) & 0xff00) |      // move byte 2 to byte 1
                    ((num << 24) & 0xff000000);  // byte 0 to byte 3
                image.setPixel(i, swapped);
            }

            return image;
        }

        std::vector<OpenS4::Import::ImageData> m_images;

        std::vector<Atlas*> m_atlases;
        std::vector<u32> m_startIndices;
        std::vector<u32> m_endIndices;

        std::vector<UV> m_uv;

        u32 m_index = 0;

       public:
        MultiAtlas(const MultiAtlas&) = delete;
        MultiAtlas() {}

        /*
                Adds an image and returns a unique image-id.
        */
        u32 addImage(OpenS4::Import::ImageData& image)
        {
            // auto inflatedImage = inflateImage(image);
            // auto swappedInflatedImage = swapImage(inflatedImage);
            // m_images.push_back(swappedInflatedImage);

            m_images.push_back(swapImage(inflateImage(image)));

            return m_images.size() - 1;
        }

        void buildAtlases(u32 atlasSize);

        void buildAtlasesContinuous(u32 atlasSize);
        void buildAtlasesContinuousFinish(u32 atlasSize);

        u32 getNumberOfAtlases() { return m_atlases.size(); }

        u32 getAtlasIndexByImageID(u32 imageID);

        const UV* getUV(u32 imageID) { return &m_uv[imageID]; }

        GLuint getAtlasTextureID(u32 atlasID);

        u32 getNumberOfPixels();
        u32 getNumberOfFilledPixels();

        u32 getSizeOfAtlas();

        Atlas* getAtlas(u32 atlasID) { return m_atlases[atlasID]; }

        ~MultiAtlas();

        std::vector<u32> getAtlasTexture(u32 atlasID);

        void dump(OpenS4::Core::Util::FileWriter* writer);

        static std::shared_ptr<MultiAtlas> MultiAtlas::fromDump(
            OpenS4::Core::Util::FileReader* reader);
    };
}  // namespace OpenS4::Renderer::ObjectRenderer
