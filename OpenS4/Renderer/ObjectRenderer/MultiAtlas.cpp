

#include "MultiAtlas.hpp"

#include <rectpack2D/finders_interface.h>

namespace OpenS4::Renderer::ObjectRenderer {

static u32 makeAtlas(std::vector<rectpack2D::rect_xywh>& rectangles,
                     u32 atlasSize, u32 startIndex) {
    using space_type =
        rectpack2D::empty_spaces<false, rectpack2D::default_empty_spaces>;

    auto packing_root = space_type({(int)atlasSize, (int)atlasSize});
    packing_root.flipping_mode = rectpack2D::flipping_option::DISABLED;

    int pixels = 0;

    for (int i = startIndex; i < rectangles.size(); i++) {
        auto& r = rectangles[i];
        if (const auto inserted_rectangle =
                packing_root.insert(std::as_const(r).get_wh())) {
            r = *inserted_rectangle;
            pixels += (r.area());
        } else {
            return i;
        }
    }
    return rectangles.size();
}

class Atlas {
    u32 m_size;

    u32* m_imageBuffer;

    void copy_image(unsigned int* dest, unsigned int* src, int posX, int posY,
                    int width, int height, int destWidth, int srcWidth) {
        if (srcWidth < 0) srcWidth = width;

        unsigned int* destP = dest + destWidth * posY;

        for (int y = 0; y < height; y++) {
            unsigned int* curDestP = destP + destWidth * y + posX;
            unsigned int* srcP = src + (height - y - 1) * srcWidth;

            for (int x = width; x > 0; x--) {
                *curDestP = *srcP;
                curDestP++;
                srcP++;
            }
        }
    }

    void copyImage(OpenS4::Import::ImageData& image,
                   rectpack2D::rect_xywh& pos) {
        for (auto y = 0; y < image.getHeight(); y++) {
            for (auto x = 0; x < image.getWidth(); x++) {
                auto color = image.getColor(x, y);

                if (m_imageBuffer[(y + pos.y) * m_size + (x + pos.x)] != 0) {
                    int z = 0;
                    z++;
                }

                if (color != 0) {
                    m_imageBuffer[(y + pos.y) * m_size + (x + pos.x)] = color;
                }
            }
        }
        int z = 0;
        z++;
    }

    GLuint m_glTextureID = 0;

   public:
    Atlas(u32 size) {
        m_size = size;
        m_imageBuffer = new u32[size * size];
        memset(m_imageBuffer, 0, sizeof(u32) * size * size);
    }
    ~Atlas() {
        if (m_imageBuffer != nullptr) delete[] m_imageBuffer;
    }

    void copyImageData(std::vector<rectpack2D::rect_xywh>& positions,
                       std::vector<OpenS4::Import::ImageData>& images,
                       u32 startIndex, u32 endIndexExcluded) {
        for (auto i = startIndex; i < endIndexExcluded; i++) {
            auto& image = images[i];
            auto& rect = positions[i];

            copyImage(image, rect);
        }

        glGenTextures(1, &m_glTextureID);
        glBindTexture(GL_TEXTURE_2D, m_glTextureID);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, m_size, m_size, 0, GL_RGBA,
                     GL_UNSIGNED_INT_8_8_8_8, m_imageBuffer);
        // TODO error checking.
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

        delete[] m_imageBuffer;
        m_imageBuffer = nullptr;
    }

    GLuint getGLTextureID() { return m_glTextureID; }
};

void MultiAtlas::buildAtlases(u32 atlasSize) {
    std::vector<rectpack2D::rect_xywh> rectangles;

    for (auto& image : m_images)
        rectangles.push_back(
            rectpack2D::rect_xywh(0, 0, image.getWidth(), image.getHeight()));

    using spaces_type =
        rectpack2D::empty_spaces<false, rectpack2D::default_empty_spaces>;

    u32 startIndex = 0;

    float atlasSizeFloat = atlasSize;

    while (startIndex < rectangles.size()) {
        auto idx = startIndex;
        startIndex = makeAtlas(rectangles, atlasSize, startIndex);

        Atlas* atlas = new Atlas(atlasSize);
        atlas->copyImageData(rectangles, m_images, idx, startIndex);

        for (int i = idx; i < startIndex; i++) {
            UV uv;

            uv.atlasID = m_atlases.size();

            uv.width = m_images[i].getWidth() - 2;
            uv.height = m_images[i].getHeight() - 2;

            uv.left = m_images[i].getLeft();
            uv.top = m_images[i].getTop();

            uv.uBegin = (rectangles[i].x + 1) / atlasSizeFloat;
            uv.uEnd = (rectangles[i].x + rectangles[i].w - 1) / atlasSizeFloat;

            uv.vBegin = (rectangles[i].y + 1) / atlasSizeFloat;
            uv.vEnd = (rectangles[i].y + rectangles[i].h - 1) / atlasSizeFloat;

            m_uv.push_back(uv);
        }

        m_startIndices.push_back(idx);
        m_endIndices.push_back(startIndex);
        m_atlases.push_back(atlas);
    }
}

u32 MultiAtlas::getAtlasIndexByImageID(u32 imageID) {
    for (auto i = 0; i < m_endIndices.size(); i++)
        if (i < m_endIndices[i]) return i;
    // TODO error
}

GLuint MultiAtlas::getAtlasTextureID(u32 atlasID) {
    return m_atlases[atlasID]->getGLTextureID();
}

void MultiAtlas::buildAtlasesContinuous(u32 atlasSize) {
    std::vector<rectpack2D::rect_xywh> rectangles;

    for (auto& image : m_images)
        rectangles.push_back(
            rectpack2D::rect_xywh(0, 0, image.getWidth(), image.getHeight()));

    using spaces_type =
        rectpack2D::empty_spaces<false, rectpack2D::default_empty_spaces>;

    u32 startIndex = m_index;

    float atlasSizeFloat = atlasSize;

    while (startIndex < rectangles.size()) {
        auto idx = startIndex;
        startIndex = makeAtlas(rectangles, atlasSize, startIndex);

        if (startIndex == rectangles.size()) return;

        m_index = startIndex;

        Atlas* atlas = new Atlas(atlasSize);
        atlas->copyImageData(rectangles, m_images, idx, startIndex);

        for (int i = idx; i < startIndex; i++) {
            UV uv;

            uv.atlasID = m_atlases.size();

            uv.width = m_images[i].getWidth() - 2;
            uv.height = m_images[i].getHeight() - 2;

            uv.left = m_images[i].getLeft();
            uv.top = m_images[i].getTop();

            uv.uBegin = (rectangles[i].x + 1) / atlasSizeFloat;
            uv.uEnd = (rectangles[i].x + rectangles[i].w - 1) / atlasSizeFloat;

            uv.vBegin = (rectangles[i].y + 1) / atlasSizeFloat;
            uv.vEnd = (rectangles[i].y + rectangles[i].h - 1) / atlasSizeFloat;

            m_uv.push_back(uv);

            m_images[i] = OpenS4::Import::ImageData(0,0,0,0,0,0,0);
        }

        m_startIndices.push_back(idx);
        m_endIndices.push_back(startIndex);
        m_atlases.push_back(atlas);
    }
}

void MultiAtlas::buildAtlasesContinuousFinish(u32 atlasSize) {
    std::vector<rectpack2D::rect_xywh> rectangles;

    for (auto& image : m_images)
        rectangles.push_back(
            rectpack2D::rect_xywh(0, 0, image.getWidth(), image.getHeight()));

    using spaces_type =
        rectpack2D::empty_spaces<false, rectpack2D::default_empty_spaces>;

    u32 startIndex = m_index;

    float atlasSizeFloat = atlasSize;

    while (startIndex < rectangles.size()) {
        auto idx = startIndex;
        startIndex = makeAtlas(rectangles, atlasSize, startIndex);

        m_index = startIndex;

        Atlas* atlas = new Atlas(atlasSize);
        atlas->copyImageData(rectangles, m_images, idx, startIndex);

        for (int i = idx; i < startIndex; i++) {
            UV uv;

            uv.atlasID = m_atlases.size();

            uv.width = m_images[i].getWidth() - 2;
            uv.height = m_images[i].getHeight() - 2;

            uv.left = m_images[i].getLeft();
            uv.top = m_images[i].getTop();

            uv.uBegin = (rectangles[i].x + 1) / atlasSizeFloat;
            uv.uEnd = (rectangles[i].x + rectangles[i].w - 1) / atlasSizeFloat;

            uv.vBegin = (rectangles[i].y + 1) / atlasSizeFloat;
            uv.vEnd = (rectangles[i].y + rectangles[i].h - 1) / atlasSizeFloat;

            m_uv.push_back(uv);

            m_images[i] = OpenS4::Import::ImageData(0, 0, 0, 0, 0, 0, 0);
        }

        m_startIndices.push_back(idx);
        m_endIndices.push_back(startIndex);
        m_atlases.push_back(atlas);
    }
}

}  // namespace OpenS4::Renderer::ObjectRenderer
