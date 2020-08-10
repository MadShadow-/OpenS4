

#include "MultiAtlas.hpp"

namespace OpenS4::Renderer::ObjectRenderer
{
    static u32 makeAtlas(std::vector<rectpack2D::rect_xywh>& rectangles,
                         u32 atlasSize,
                         u32 startIndex)
    {
        using space_type =
            rectpack2D::empty_spaces<false, rectpack2D::default_empty_spaces>;

        auto packing_root = space_type({(int)atlasSize, (int)atlasSize});
        packing_root.flipping_mode = rectpack2D::flipping_option::DISABLED;

        int pixels = 0;

        for (int i = startIndex; i < rectangles.size(); i++)
        {
            auto& r = rectangles[i];
            if (const auto inserted_rectangle =
                    packing_root.insert(std::as_const(r).get_wh()))
            {
                r = *inserted_rectangle;
                pixels += (r.area());
            }
            else
            {
                return i;
            }
        }
        return rectangles.size();
    }

    void MultiAtlas::buildAtlases(u32 atlasSize)
    {
        std::vector<rectpack2D::rect_xywh> rectangles;

        for (auto& image : m_images)
            rectangles.push_back(rectpack2D::rect_xywh(
                0, 0, image.getWidth(), image.getHeight()));

        using spaces_type =
            rectpack2D::empty_spaces<false, rectpack2D::default_empty_spaces>;

        u32 startIndex = 0;

        float atlasSizeFloat = atlasSize;

        while (startIndex < rectangles.size())
        {
            auto idx = startIndex;
            startIndex = makeAtlas(rectangles, atlasSize, startIndex);

            Atlas* atlas = new Atlas(atlasSize);
            atlas->copyImageData(rectangles, m_images, idx, startIndex);

            for (int i = idx; i < startIndex; i++)
            {
                UV uv;

                uv.atlasID = m_atlases.size();

                uv.width = m_images[i].getWidth() - 2;
                uv.height = m_images[i].getHeight() - 2;

                uv.left = m_images[i].getLeft();
                uv.top = m_images[i].getTop();

                uv.uBegin = (rectangles[i].x + 1) / atlasSizeFloat;
                uv.uEnd =
                    (rectangles[i].x + rectangles[i].w - 1) / atlasSizeFloat;

                uv.vBegin = (rectangles[i].y + 1) / atlasSizeFloat;
                uv.vEnd =
                    (rectangles[i].y + rectangles[i].h - 1) / atlasSizeFloat;

                m_uv.push_back(uv);
            }

            m_startIndices.push_back(idx);
            m_endIndices.push_back(startIndex);
            m_atlases.push_back(atlas);
        }
    }

    u32 MultiAtlas::getAtlasIndexByImageID(u32 imageID)
    {
        for (auto i = 0; i < m_endIndices.size(); i++)
            if (i < m_endIndices[i]) return i;
        // TODO error
    }

    GLuint MultiAtlas::getAtlasTextureID(u32 atlasID)
    {
        return m_atlases[atlasID]->getGLTextureID();
    }

    void MultiAtlas::buildAtlasesContinuous(u32 atlasSize)
    {
        std::vector<rectpack2D::rect_xywh> rectangles;

        for (auto& image : m_images)
            rectangles.push_back(rectpack2D::rect_xywh(
                0, 0, image.getWidth(), image.getHeight()));

        using spaces_type =
            rectpack2D::empty_spaces<false, rectpack2D::default_empty_spaces>;

        u32 startIndex = m_index;

        float atlasSizeFloat = atlasSize;

        while (startIndex < rectangles.size())
        {
            auto idx = startIndex;
            startIndex = makeAtlas(rectangles, atlasSize, startIndex);

            if (startIndex == rectangles.size()) return;

            m_index = startIndex;

            Atlas* atlas = new Atlas(atlasSize);
            atlas->copyImageData(rectangles, m_images, idx, startIndex);

            for (int i = idx; i < startIndex; i++)
            {
                UV uv;

                uv.atlasID = m_atlases.size();

                uv.width = m_images[i].getWidth() - 2;
                uv.height = m_images[i].getHeight() - 2;

                uv.left = m_images[i].getLeft();
                uv.top = m_images[i].getTop();

                uv.uBegin = (rectangles[i].x + 1) / atlasSizeFloat;
                uv.uEnd =
                    (rectangles[i].x + rectangles[i].w - 1) / atlasSizeFloat;

                uv.vBegin = (rectangles[i].y + 1) / atlasSizeFloat;
                uv.vEnd =
                    (rectangles[i].y + rectangles[i].h - 1) / atlasSizeFloat;

                m_uv.push_back(uv);

                m_images[i] = OpenS4::Import::ImageData(0, 0, 0, 0, 0, 0, 0);
            }

            m_startIndices.push_back(idx);
            m_endIndices.push_back(startIndex);
            m_atlases.push_back(atlas);
        }
    }

    void MultiAtlas::buildAtlasesContinuousFinish(u32 atlasSize)
    {
        std::vector<rectpack2D::rect_xywh> rectangles;

        for (auto& image : m_images)
            rectangles.push_back(rectpack2D::rect_xywh(
                0, 0, image.getWidth(), image.getHeight()));

        using spaces_type =
            rectpack2D::empty_spaces<false, rectpack2D::default_empty_spaces>;

        u32 startIndex = m_index;

        float atlasSizeFloat = atlasSize;

        while (startIndex < rectangles.size())
        {
            auto idx = startIndex;
            startIndex = makeAtlas(rectangles, atlasSize, startIndex);

            m_index = startIndex;

            Atlas* atlas = new Atlas(atlasSize);
            atlas->copyImageData(rectangles, m_images, idx, startIndex);

            for (int i = idx; i < startIndex; i++)
            {
                UV uv;

                uv.atlasID = m_atlases.size();

                uv.width = m_images[i].getWidth() - 2;
                uv.height = m_images[i].getHeight() - 2;

                uv.left = m_images[i].getLeft();
                uv.top = m_images[i].getTop();

                uv.uBegin = (rectangles[i].x + 1) / atlasSizeFloat;
                uv.uEnd =
                    (rectangles[i].x + rectangles[i].w - 1) / atlasSizeFloat;

                uv.vBegin = (rectangles[i].y + 1) / atlasSizeFloat;
                uv.vEnd =
                    (rectangles[i].y + rectangles[i].h - 1) / atlasSizeFloat;

                m_uv.push_back(uv);

                m_images[i] = OpenS4::Import::ImageData(0, 0, 0, 0, 0, 0, 0);
            }

            m_startIndices.push_back(idx);
            m_endIndices.push_back(startIndex);
            m_atlases.push_back(atlas);
        }
    }

    u32 MultiAtlas::getNumberOfFilledPixels()
    {
        u32 pixels = 0;
        for (auto& atlas : m_atlases) pixels += atlas->getFilledPixels();
        return pixels;
    }
    u32 MultiAtlas::getNumberOfPixels()
    {
        u32 pixels = 0;
        for (auto& atlas : m_atlases) pixels += atlas->getAtlasSize();
        return pixels;
    }
    u32 MultiAtlas::getSizeOfAtlas() { return m_atlases[0]->getAtlasSize(); }

    MultiAtlas::~MultiAtlas()
    {
        for (auto& atlas : m_atlases) delete atlas;
    }

    std::vector<u32> MultiAtlas::getAtlasTexture(u32 atlasID)
    {
        return m_atlases[atlasID]->getAtlasImage();
    }

    void MultiAtlas::dump(OpenS4::Core::Util::FileWriter* writer)
    {
        OpenS4::getLogger().info("Dumping MultiAtlas...");
        writer->write_u32(m_atlases.size());

        for (u32 i = 0; i < m_atlases.size(); i++)
        {
            m_atlases[i]->dump(writer);
        }

        writer->write_u32vector(m_startIndices);
        writer->write_u32vector(m_endIndices);

        writer->write_u32(m_uv.size());
        for (auto& uv : m_uv) uv.dump(writer);

        OpenS4::getLogger().info("Dumping MultiAtlas done.");
    }

    std::shared_ptr<MultiAtlas> MultiAtlas::fromDump(
        OpenS4::Core::Util::FileReader* reader)
    {
        std::shared_ptr<MultiAtlas> ma = std::make_shared<MultiAtlas>();

        MultiAtlas* m = ma.get();

        u32 numberOfAtlases = reader->read_u32();
        for (u32 i = 0; i < numberOfAtlases; i++)
        {
            m->m_atlases.push_back(Atlas::fromDump(reader));
        }

        m->m_startIndices = reader->read_u32vector();
        m->m_endIndices = reader->read_u32vector();

        auto uv_size = reader->read_u32();
        for (u32 i = 0; i < uv_size; i++)
            m->m_uv.push_back(UV::fromDump(reader));

        return ma;
    }

}  // namespace OpenS4::Renderer::ObjectRenderer
