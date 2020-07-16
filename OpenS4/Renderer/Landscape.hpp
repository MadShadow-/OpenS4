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

    std::vector<LandscapeTextureID> m_textureType;

    std::vector<int> m_heightMap;

    std::vector<LandscapeTexturePosition> m_texturePositionLow;
    std::vector<LandscapeTexturePosition> m_texturePositionHigh;

    __inline int idx(int x, int y) { return y * m_width + x; }

    LandscapeTextures* m_landscapeTextures;
    TextureMapper* m_textureMapper;

   public:
    Landscape(LandscapeTextures* landscapeTextures, TextureMapper* textureMapper,
              size_t width = 0, size_t height = 0) {
        m_width = width;
        m_height = height;

        m_textureType.resize(m_width * m_height);
        m_heightMap.resize(m_width * m_height);
      //  m_texturePositionLow.resize(m_width * m_height);
      //  m_texturePositionHigh.resize(m_width * m_height);

        m_landscapeTextures = landscapeTextures;
        m_textureMapper = textureMapper;
    }

    void setTerrain(int x, int y, LandscapeTextureID name) {
        m_textureType[idx(x, y)] = name;
    }

    void setTerrainHeight(int x, int y, int height) {
        m_heightMap[idx(x, y)] = height;
    }

    int getHeight() { return m_height; }
    int getWidth() { return m_width; }

    bool isValid(int x, int y) {
        if (y >= m_height || x >= m_width || x < 0 || y < 0) return false;
        return true;
    }

    LandscapeTextureID getTerrain(int x, int y) { return m_textureType[idx(x, y)]; }

    int getTerrainHeight(int x, int y) { return m_heightMap[idx(x, y)]; }

    void update() {
        for (int y = 0; y < m_height; y++) {
            for (int x = 0; x < m_width; x++) {
                int i = idx(x, y);

                auto tex1 = m_textureType[i];
                auto tex2 = isValid(x + 1, y) ? getTerrain(x + 1, y) : tex1;
                auto tex3 = isValid(x, y - 1) ? getTerrain(x, y - 1) : tex1;
                auto tex4 =
                    isValid(x + 1, y - 1) ? getTerrain(x + 1, y - 1) : tex1;

                float repeat = 8.0f;

                // First Tri
                if (tex1 != tex2 || tex2 != tex3) {
                    auto mappings =
                        m_textureMapper->getTextureMappingsLow(tex1, tex2, tex3);

                    if (mappings.size() > 0) {
                        int idx = 0;  // (x + y) % mappings.size();

                        auto info = mappings[idx];

                        auto texture_info =
                            m_landscapeTextures->getTextureInformation(
                                mappings[0].hexagonTextureID);

                        m_texturePositionLow[i].p = info.a;
                        m_texturePositionLow[i].p2 = info.b;
                        m_texturePositionLow[i].p3 = info.c;
                    }
                } else {
                    int id = m_textureMapper->getTextureID(tex1);
                    auto info = m_landscapeTextures->getTextureInformation(id);

                    float texX1 =
                        (info->x + ((x) % (int)repeat) / repeat * info->width) /
                        (float)m_landscapeTextures->get_width();
                    float texX2 = (info->x + ((x) % (int)repeat + 1.0f) /
                                                 repeat * info->width) /
                                  (float)m_landscapeTextures->get_width();
                    float texX3 = texX1;
                    float texX4 = texX2;

                    float texY3 = (info->y + ((y) % (int)repeat) / repeat *
                                                 info->height) /
                                  (float)m_landscapeTextures->get_height();
                    float texY4 = texY3;
                    float texY1 = (info->y + ((y) % (int)repeat + 1.0f) /
                                                 repeat * info->height) /
                                  (float)m_landscapeTextures->get_height();
                    float texY2 = texY1;

                    if (info->tex_type ==
                        LandscapeTextures::TextureType::_TEX) {
                        texX1 =
                            (info->x) / (float)m_landscapeTextures->get_width();
                        texX2 = (info->x + info->width) /
                                (float)m_landscapeTextures->get_width();
                        texX3 = texX1;
                        texX4 = texX2;

                        texY3 =
                            (info->y) / (float)m_landscapeTextures->get_height();
                        texY4 = texY3;
                        texY1 = (info->y + info->height) /
                                (float)m_landscapeTextures->get_height();
                        texY2 = texY1;
                    }

                    m_texturePositionLow[i].p = TexturePosition(texX1, texY1);
                    m_texturePositionLow[i].p2 = TexturePosition(texX2, texY2);
                    m_texturePositionLow[i].p3 = TexturePosition(texX3, texY3);
                }
                // Second Tri
                if (tex2 != tex4 || tex3 != tex4) {
                    auto mappings = m_textureMapper->getTextureMappingsHigh(
                        tex2, tex4, tex3);

                    if (mappings.size() > 0) {
                        int idx = 0;  // (x + y) % mappings.size();

                        auto info = mappings[0];

                        auto texture_info =
                            m_landscapeTextures->getTextureInformation(
                                mappings[0].hexagonTextureID);

                        m_texturePositionHigh[i].p = info.a;
                        m_texturePositionHigh[i].p2 = info.b;
                        m_texturePositionHigh[i].p3 = info.c;
                    }
                } else {
                    int id = m_textureMapper->getTextureID(tex4);
                    auto info = m_landscapeTextures->getTextureInformation(id);

                    float texX1 =
                        (info->x + ((x) % (int)repeat) / repeat * info->width) /
                        (float)m_landscapeTextures->get_width();
                    float texX2 = (info->x + ((x) % (int)repeat + 1.0f) /
                                                 repeat * info->width) /
                                  (float)m_landscapeTextures->get_width();
                    float texX3 = texX1;
                    float texX4 = texX2;

                    float texY3 = (info->y + ((y) % (int)repeat) / repeat *
                                                 info->height) /
                                  (float)m_landscapeTextures->get_height();
                    float texY4 = texY3;
                    float texY1 = (info->y + ((y) % (int)repeat + 1.0f) /
                                                 repeat * info->height) /
                                  (float)m_landscapeTextures->get_height();
                    float texY2 = texY1;

                    if (info->tex_type ==
                        LandscapeTextures::TextureType::_TEX) {
                        texX1 =
                            (info->x) / (float)m_landscapeTextures->get_width();
                        texX2 = (info->x + info->width) /
                                (float)m_landscapeTextures->get_width();
                        texX3 = texX1;
                        texX4 = texX2;

                        texY3 =
                            (info->y) / (float)m_landscapeTextures->get_height();
                        texY4 = texY3;
                        texY1 = (info->y + info->height) /
                                (float)m_landscapeTextures->get_height();
                        texY2 = texY1;
                    }

                    m_texturePositionHigh[i].p = TexturePosition(texX2, texY2);
                    m_texturePositionHigh[i].p2 = TexturePosition(texX4, texY4);
                    m_texturePositionHigh[i].p3 = TexturePosition(texX3, texY3);
                }
            }
        }
    }
    /*
    const LandscapeTexturePosition& getTexturePositionLow(int x, int y) {
        return m_texturePositionLow[idx(x, y)];
    }

    const LandscapeTexturePosition& getTexturePositionHigh(int x, int y) {
        return m_texturePositionHigh[idx(x, y)];
    }
    */

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