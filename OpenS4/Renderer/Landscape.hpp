#pragma once

#include <vector>

#include "LandscapeTextures.hpp"
#include "TextureMapper.hpp"

namespace OpenS4::Renderer {

struct LandscapeTexturePosition {
    TexturePosition p;
    TexturePosition p2;
    TexturePosition p3;
};

class Landscape {
    size_t m_width;
    size_t m_height;

    __inline int idx(int x, int y) { return y * m_width + x; }

    LandscapeTextures* m_landscapeTextures;
    TextureMapper* m_textureMapper;

    OpenS4::Logic::Map::Landscape* m_landscape;

   public:
    Landscape(OpenS4::Logic::Map::Landscape* landscape,
              LandscapeTextures* landscapeTextures,
              TextureMapper* textureMapper) {
        m_landscape = landscape;
        m_width = m_landscape->getWidth();
        m_height = m_landscape->getHeight();

        m_landscapeTextures = landscapeTextures;
        m_textureMapper = textureMapper;
    }

    int getHeight() { return m_height; }
    int getWidth() { return m_width; }

    bool isValid(int x, int y) {
        if (y >= m_height || x >= m_width || x < 0 || y < 0) return false;
        return true;
    }

    LandscapeTextureID getTerrain(int x, int y) {
        return m_textureMapper->getRendererTextureID(
            m_landscape->getTerrain(x, y));
    }

    int getTerrainHeight(int x, int y) {
        return m_landscape->getTerrainHeight(x, y);
    }

    LandscapeTexturePosition calcGetTexturePositionHigh(int x, int y) {
        auto tex1 = getTerrain(x, y);
        auto tex2 = isValid(x + 1, y) ? getTerrain(x + 1, y) : tex1;
        auto tex3 = isValid(x, y - 1) ? getTerrain(x, y - 1) : tex1;
        auto tex4 = isValid(x + 1, y - 1) ? getTerrain(x + 1, y - 1) : tex1;

        float repeat = 8.0f;

        LandscapeTexturePosition low;
        LandscapeTexturePosition high;

        // First Tri
        if (tex1 != tex2 || tex2 != tex3) {
            auto mappings =
                m_textureMapper->getTextureMappingsLow(tex1, tex2, tex3);

            if (mappings.size() > 0) {
                int idx = 0;  // (x + y) % mappings.size();

                auto info = mappings[idx];

                auto texture_info = m_landscapeTextures->getTextureInformation(
                    mappings[0].hexagonTextureID);

                low.p = info.a;
                low.p2 = info.b;
                low.p3 = info.c;
            }
        } else {
            int id = m_textureMapper->getTextureID(tex1);
            auto info = m_landscapeTextures->getTextureInformation(id);

            float texX1 =
                (info->x + ((x) % (int)repeat) / repeat * info->width) /
                (float)m_landscapeTextures->get_width();
            float texX2 =
                (info->x + ((x) % (int)repeat + 1.0f) / repeat * info->width) /
                (float)m_landscapeTextures->get_width();
            float texX3 = texX1;
            float texX4 = texX2;

            float texY3 =
                (info->y + ((y) % (int)repeat) / repeat * info->height) /
                (float)m_landscapeTextures->get_height();
            float texY4 = texY3;
            float texY1 =
                (info->y + ((y) % (int)repeat + 1.0f) / repeat * info->height) /
                (float)m_landscapeTextures->get_height();
            float texY2 = texY1;

            if (info->tex_type == LandscapeTextures::TextureType::_TEX) {
                texX1 = (info->x) / (float)m_landscapeTextures->get_width();
                texX2 = (info->x + info->width) /
                        (float)m_landscapeTextures->get_width();
                texX3 = texX1;
                texX4 = texX2;

                texY3 = (info->y) / (float)m_landscapeTextures->get_height();
                texY4 = texY3;
                texY1 = (info->y + info->height) /
                        (float)m_landscapeTextures->get_height();
                texY2 = texY1;
            }

            low.p = TexturePosition(texX1, texY1);
            low.p2 = TexturePosition(texX2, texY2);
            low.p3 = TexturePosition(texX3, texY3);
        }
        // Second Tri
        if (tex2 != tex4 || tex3 != tex4) {
            auto mappings =
                m_textureMapper->getTextureMappingsHigh(tex2, tex4, tex3);

            if (mappings.size() > 0) {
                int idx = (x + y) % mappings.size();

                auto info = mappings[idx];

                high.p = info.a;
                high.p2 = info.b;
                high.p3 = info.c;
            }
        } else {
            int id = m_textureMapper->getTextureID(tex4);
            auto info = m_landscapeTextures->getTextureInformation(id);

            float texX1 =
                (info->x + ((x) % (int)repeat) / repeat * info->width) /
                (float)m_landscapeTextures->get_width();
            float texX2 =
                (info->x + ((x) % (int)repeat + 1.0f) / repeat * info->width) /
                (float)m_landscapeTextures->get_width();
            float texX3 = texX1;
            float texX4 = texX2;

            float texY3 =
                (info->y + ((y) % (int)repeat) / repeat * info->height) /
                (float)m_landscapeTextures->get_height();
            float texY4 = texY3;
            float texY1 =
                (info->y + ((y) % (int)repeat + 1.0f) / repeat * info->height) /
                (float)m_landscapeTextures->get_height();
            float texY2 = texY1;

            if (info->tex_type == LandscapeTextures::TextureType::_TEX) {
                texX1 = (info->x) / (float)m_landscapeTextures->get_width();
                texX2 = (info->x + info->width) /
                        (float)m_landscapeTextures->get_width();
                texX3 = texX1;
                texX4 = texX2;

                texY3 = (info->y) / (float)m_landscapeTextures->get_height();
                texY4 = texY3;
                texY1 = (info->y + info->height) /
                        (float)m_landscapeTextures->get_height();
                texY2 = texY1;
            }

            high.p = TexturePosition(texX2, texY2);
            high.p2 = TexturePosition(texX4, texY4);
            high.p3 = TexturePosition(texX3, texY3);
        }

        return high;
    }

    LandscapeTexturePosition calcGetTexturePositionLow(int x, int y) {
        auto tex1 = getTerrain(x, y);
        auto tex2 = isValid(x + 1, y) ? getTerrain(x + 1, y) : tex1;
        auto tex3 = isValid(x, y - 1) ? getTerrain(x, y - 1) : tex1;
        auto tex4 = isValid(x + 1, y - 1) ? getTerrain(x + 1, y - 1) : tex1;

        float repeat = 8.0f;

        LandscapeTexturePosition low;
        LandscapeTexturePosition high;

        // First Tri
        if (tex1 != tex2 || tex2 != tex3) {
            auto mappings =
                m_textureMapper->getTextureMappingsLow(tex1, tex2, tex3);

            if (mappings.size() > 0) {
                int idx = (x + y) % mappings.size();

                auto info = mappings[idx];

                low.p = info.a;
                low.p2 = info.b;
                low.p3 = info.c;
            }
        } else {
            int id = m_textureMapper->getTextureID(tex1);
            auto info = m_landscapeTextures->getTextureInformation(id);

            float texX1 =
                (info->x + ((x) % (int)repeat) / repeat * info->width) /
                (float)m_landscapeTextures->get_width();
            float texX2 =
                (info->x + ((x) % (int)repeat + 1.0f) / repeat * info->width) /
                (float)m_landscapeTextures->get_width();
            float texX3 = texX1;
            float texX4 = texX2;

            float texY3 =
                (info->y + ((y) % (int)repeat) / repeat * info->height) /
                (float)m_landscapeTextures->get_height();
            float texY4 = texY3;
            float texY1 =
                (info->y + ((y) % (int)repeat + 1.0f) / repeat * info->height) /
                (float)m_landscapeTextures->get_height();
            float texY2 = texY1;

            if (info->tex_type == LandscapeTextures::TextureType::_TEX) {
                texX1 = (info->x) / (float)m_landscapeTextures->get_width();
                texX2 = (info->x + info->width) /
                        (float)m_landscapeTextures->get_width();
                texX3 = texX1;
                texX4 = texX2;

                texY3 = (info->y) / (float)m_landscapeTextures->get_height();
                texY4 = texY3;
                texY1 = (info->y + info->height) /
                        (float)m_landscapeTextures->get_height();
                texY2 = texY1;
            }

            low.p = TexturePosition(texX1, texY1);
            low.p2 = TexturePosition(texX2, texY2);
            low.p3 = TexturePosition(texX3, texY3);
        }
        // Second Tri
        if (tex2 != tex4 || tex3 != tex4) {
            auto mappings =
                m_textureMapper->getTextureMappingsHigh(tex2, tex4, tex3);

            if (mappings.size() > 0) {
                int idx = 0;  // (x + y) % mappings.size();

                auto info = mappings[0];

                auto texture_info = m_landscapeTextures->getTextureInformation(
                    mappings[0].hexagonTextureID);

                high.p = info.a;
                high.p2 = info.b;
                high.p3 = info.c;
            }
        } else {
            int id = m_textureMapper->getTextureID(tex4);
            auto info = m_landscapeTextures->getTextureInformation(id);

            float texX1 =
                (info->x + ((x) % (int)repeat) / repeat * info->width) /
                (float)m_landscapeTextures->get_width();
            float texX2 =
                (info->x + ((x) % (int)repeat + 1.0f) / repeat * info->width) /
                (float)m_landscapeTextures->get_width();
            float texX3 = texX1;
            float texX4 = texX2;

            float texY3 =
                (info->y + ((y) % (int)repeat) / repeat * info->height) /
                (float)m_landscapeTextures->get_height();
            float texY4 = texY3;
            float texY1 =
                (info->y + ((y) % (int)repeat + 1.0f) / repeat * info->height) /
                (float)m_landscapeTextures->get_height();
            float texY2 = texY1;

            if (info->tex_type == LandscapeTextures::TextureType::_TEX) {
                texX1 = (info->x) / (float)m_landscapeTextures->get_width();
                texX2 = (info->x + info->width) /
                        (float)m_landscapeTextures->get_width();
                texX3 = texX1;
                texX4 = texX2;

                texY3 = (info->y) / (float)m_landscapeTextures->get_height();
                texY4 = texY3;
                texY1 = (info->y + info->height) /
                        (float)m_landscapeTextures->get_height();
                texY2 = texY1;
            }

            high.p = TexturePosition(texX2, texY2);
            high.p2 = TexturePosition(texX4, texY4);
            high.p3 = TexturePosition(texX3, texY3);
        }

        return low;
    }
};

}  // namespace OpenS4::Renderer
