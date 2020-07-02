#pragma once

#include "Image.hpp"

namespace OpenS4::Import {
class GraphicsRegistry {
    std::map<uint32_t, std::vector<ImageData>> m_images;

    std::map<uint32_t, std::shared_ptr<IGraphics>> m_gfxs;
    std::map<uint32_t, std::shared_ptr<BackgroundGraphics>> m_ghs;

   public:
    void addGFX(uint32_t id, std::shared_ptr<IGraphics> gfx) {
        m_gfxs.emplace(id, gfx);
    }

    void addGH(uint32_t id, std::shared_ptr<BackgroundGraphics> gh) {
        m_ghs.emplace(id, gh);
    }

    ImageData getImage(uint32_t id, uint32_t pic_id) {
        if (m_gfxs.count(id)) {
            return m_gfxs[id]->getImage(pic_id);
        } else {
            return m_ghs[id]->getImage(pic_id);
        }
    }

    bool hasGfx(uint32_t id) { return m_gfxs.count(id) != 0; }
    bool hasGh(uint32_t id) { return m_ghs.count(id) != 0; }
    bool hasGraphics(uint32_t id) { return hasGfx(id) || hasGh(id); }
};

/* Read gfx by registry entry. Windows only. */
std::string getGraphicsPathByRegistry();
/* Read gfx by path. */
GraphicsRegistry getGraphicsRegistry(std::string path);
}  // namespace OpenS4::Graphics
