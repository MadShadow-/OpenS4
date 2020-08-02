#pragma once

#include "../../Import/Graphics/Image.hpp"
#include "../OpenGL.hpp"

namespace OpenS4::Renderer::ObjectRenderer {

class Atlas;

struct UV {
    u32 width;
    u32 height;

    u32 atlasID;

    u32 left;
    u32 top;

    float uBegin;
    float uEnd;
    float vBegin;
    float vEnd;
};

class MultiAtlas {
    OpenS4::Import::ImageData inflateImage(OpenS4::Import::ImageData image) {
        auto img = &image;
        OpenS4::Import::ImageData biggerImage(
            img->getWidth() + 2, img->getHeight() + 2, img->getPalette(),
            img->getLeft(), img->getTop(), img->getFlag1(), img->getFlag2());
        for (int y = 0; y < biggerImage.getHeight(); y++) {
            for (int x = 0; x < biggerImage.getWidth(); x++) {
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
    OpenS4::Import::ImageData swapImage(OpenS4::Import::ImageData image) {
        OpenS4::Import::ImageData img(image.getWidth(), image.getHeight(),
                                      image.getPalette(), image.getLeft(),
                                      image.getTop(), image.getFlag1(), image.getFlag2());

        for (int i = 0; i < image.getWidth() * image.getHeight(); i++) {
            uint32_t num = image.getData()[i];
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
    /*
            Adds an image and returns a unique image-id.
    */
    u32 addImage(OpenS4::Import::ImageData image) {
        auto inflatedImage = inflateImage(image);
        auto swappedInflatedImage = swapImage(inflatedImage);
        m_images.push_back(swappedInflatedImage);

        return m_images.size() - 1;
    }

    void buildAtlases(u32 atlasSize);

    void buildAtlasesContinuous(u32 atlasSize);
    void buildAtlasesContinuousFinish(u32 atlasSize);

    u32 getNumberOfAtlases() { return m_atlases.size(); }

    u32 getAtlasIndexByImageID(u32 imageID);

    const UV* getUV(u32 imageID) { return &m_uv[imageID]; }

    GLuint getAtlasTextureID(u32 atlasID);

    ~MultiAtlas();
};
}  // namespace OpenS4::Renderer::ObjectRenderer
