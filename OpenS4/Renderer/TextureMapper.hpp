#pragma once

#include <algorithm>
#include <map>
#include <string>
#include <vector>

#include "Core/Types.hpp"
#include "../Logic/Map/Landscape.hpp"

namespace OpenS4::Renderer {

using LandscapeTextureID = u32;

struct TexturePosition {
    double x;
    double y;
    TexturePosition(double x = 0, double y = 0) : x(x), y(y) {}
};

struct TextureSize {
    double width;
    double height;
    TextureSize(double width = 0, double height = 0)
        : width(width), height(height) {}
};

enum class HexagonPosition { _0, _1, _2, _3, _4, _5 };

struct HexagonInformation {
    int hexagon_texture_id = -1;

    struct Position {
        int x;
        int y;
    };

    Position _0;
    Position _1;
    Position _2;
    Position _3;
    Position _4;
    Position _5;
    Position _6;

    HexagonInformation(int sizeX, int sizeY) {
        _0.x = sizeX / 2;
        _0.y = sizeY / 2;

        _1.x = 0;
        _1.y = sizeY / 2;

        _2.x = sizeX / 4;
        _2.y = sizeY;

        _3.x = 3 * sizeX / 4;
        _3.y = sizeY;

        _4.x = sizeX;
        _4.y = sizeY / 2;

        _5.x = 3 * sizeX / 4;
        _5.y = 0;

        _6.x = sizeX / 4;
        _6.y = 0;
    }

    HexagonInformation() : HexagonInformation(64, 64) {}
};

struct HexInfo {
    int hexagon_texture_id = -1;
    TexturePosition offset;  // start of hexagon
    TextureSize size;        // size relative to texture atlas!
};

struct HexagonPart {
    int hexagonTextureID;

    TexturePosition a;
    TexturePosition b;
    TexturePosition c;

    HexagonPart() {}
    HexagonPart(HexInfo info, HexagonPosition triID) {
        TexturePosition _0;
        TexturePosition _1;
        TexturePosition _2;
        TexturePosition _3;
        TexturePosition _4;
        TexturePosition _5;
        TexturePosition _6;

        _0.x = info.offset.x + info.size.width / 2;
        _0.y = info.offset.y + info.size.height / 2;

        _1.x = info.offset.x;
        _1.y = info.offset.y + info.size.height / 2;

        _2.x = info.offset.x + info.size.width / 4;
        _2.y = info.offset.y + info.size.height;

        _3.x = info.offset.x + info.size.width * 3 / 4;
        _3.y = info.offset.y + info.size.height;

        _4.x = info.offset.x + info.size.width;
        _4.y = info.offset.y + info.size.height / 2;

        _5.x = info.offset.x + info.size.width * 3 / 4;
        _5.y = info.offset.y;

        _6.x = info.offset.x + info.size.width / 4;
        _6.y = info.offset.y;

        hexagonTextureID = info.hexagon_texture_id;
        a = _0;

        switch (triID) {
            case HexagonPosition::_0:
                b = _1;
                c = _2;
                break;
            case HexagonPosition::_1:
                b = _2;
                c = _3;
                break;
            case HexagonPosition::_2:
                b = _3;
                c = _4;
                break;
            case HexagonPosition::_3:
                b = _4;
                c = _5;
                break;
            case HexagonPosition::_4:
                b = _5;
                c = _6;
                break;
            case HexagonPosition::_5:
                b = _6;
                c = _1;
                break;
            default:
                // TODO throw something?
                break;
        }
    }
};

struct HexPart : public HexagonPart {
    HexPart(HexInfo info, HexagonPosition triID) {
        TexturePosition _0;
        TexturePosition _1;
        TexturePosition _2;
        TexturePosition _3;
        TexturePosition _4;
        TexturePosition _5;
        TexturePosition _6;

        _0.x = info.offset.x + info.size.width / 2;
        _0.y = info.offset.y + info.size.height / 2;

        _1.x = info.offset.x;
        _1.y = info.offset.y + info.size.height / 2;

        _2.x = info.offset.x + info.size.width / 4;
        _2.y = info.offset.y + info.size.height;

        _3.x = info.offset.x + info.size.width * 3 / 4;
        _3.y = info.offset.y + info.size.height;

        _4.x = info.offset.x + info.size.width;
        _4.y = info.offset.y + info.size.height / 2;

        _5.x = info.offset.x + info.size.width * 3 / 4;
        _5.y = info.offset.y;

        _6.x = info.offset.x + info.size.width / 4;
        _6.y = info.offset.y;


        hexagonTextureID = info.hexagon_texture_id;

        switch (triID) {
            case HexagonPosition::_0:
                a = _2;
                b = _0;
                c = _1;
                break;
            case HexagonPosition::_1:
                a = _2;
                b = _3;
                c = _0;
                break;
            case HexagonPosition::_2:
                a = _3;
                b = _4;
                c = _0;
                break;
            case HexagonPosition::_3:
                a = _0;
                b = _3;
                c = _4;
                break;
            case HexagonPosition::_4:
                a = _0;
                b = _5;
                c = _6;
                break;
            case HexagonPosition::_5:
                a = _1;
                b = _0;
                c = _6;
                break;
            default:
                // TODO throw something?
                break;
        }
    }
};

class TextureMapper {
    std::map<LandscapeTextureID, int> m_landscapeTextureID;
    std::map<LandscapeTextureID, LandscapeTextureID>
        m_landscapeTextureNameToLandscapeTextureID;

    std::map<LandscapeTextureID,
             std::map<LandscapeTextureID,
                      std::map<LandscapeTextureID, std::vector<HexagonPart>>>>
        m_hexagonTexturesLow;
    std::map<LandscapeTextureID,
             std::map<LandscapeTextureID,
                      std::map<LandscapeTextureID, std::vector<HexagonPart>>>>
        m_hexagonTexturesHigh;

    LandscapeTextureID textureIDCounter = 0;

    std::map<OpenS4::Logic::Map::LandscapeTextureID, LandscapeTextureID>
        m_logicToRenderer;

   public:
    LandscapeTextureID createRendererLandscapeTextureID() {
        return ++textureIDCounter;
    }

    void setTextureMapping(OpenS4::Logic::Map::LandscapeTextureID logicID,
                           LandscapeTextureID textureID) {
        m_logicToRenderer[logicID] = textureID;
    }
    LandscapeTextureID getRendererTextureID(
        OpenS4::Logic::Map::LandscapeTextureID logicID) {
        return m_logicToRenderer[logicID];
    }

    void addTextureHexagon(LandscapeTextureID name0, LandscapeTextureID name1,
                           LandscapeTextureID name2, LandscapeTextureID name3,
                           LandscapeTextureID name4, LandscapeTextureID name5,
                           LandscapeTextureID name6, HexInfo tex) {
        HexagonPart hexPart;
        hexPart.hexagonTextureID = tex.hexagon_texture_id;

        m_hexagonTexturesHigh[name2][name0][name1].push_back(
            HexPart(tex, HexagonPosition::_0));
        m_hexagonTexturesHigh[name3][name4][name0].push_back(
            HexPart(tex, HexagonPosition::_2));
        m_hexagonTexturesHigh[name0][name5][name6].push_back(
            HexPart(tex, HexagonPosition::_4));

        m_hexagonTexturesLow[name2][name3][name0].push_back(
            HexPart(tex, HexagonPosition::_1));
        m_hexagonTexturesLow[name0][name4][name5].push_back(
            HexPart(tex, HexagonPosition::_3));
        m_hexagonTexturesLow[name1][name0][name6].push_back(
            HexPart(tex, HexagonPosition::_5));
    }

    void addTextureHexagon(LandscapeTextureID name0, LandscapeTextureID _1,
                           LandscapeTextureID _2, LandscapeTextureID _2_2,
                           LandscapeTextureID _3, LandscapeTextureID _3_2,
                           LandscapeTextureID _4,
                           LandscapeTextureID _4_2,
                           LandscapeTextureID _5,
                           LandscapeTextureID _5_2,
                           LandscapeTextureID _6,
                           LandscapeTextureID _6_2,
                           LandscapeTextureID _1_2,
        
        HexInfo tex) {
        HexagonPart hexPart;
        hexPart.hexagonTextureID = tex.hexagon_texture_id;

        m_hexagonTexturesHigh[_2][name0][_1].push_back(
            HexPart(tex, HexagonPosition::_0));

        m_hexagonTexturesHigh[_3_2][_4][name0].push_back(
            HexPart(tex, HexagonPosition::_2));
        
        m_hexagonTexturesHigh[name0][_5_2][_6].push_back(
            HexPart(tex, HexagonPosition::_4));

        m_hexagonTexturesLow[_2_2][_3][name0].push_back(
            HexPart(tex, HexagonPosition::_1));
        m_hexagonTexturesLow[name0][_4_2][_5].push_back(
            HexPart(tex, HexagonPosition::_3));
        m_hexagonTexturesLow[_1_2][name0][_6_2].push_back(
            HexPart(tex, HexagonPosition::_5));
    }

    void addTextureHexagon(LandscapeTextureID name0, LandscapeTextureID nameAll,
                           HexInfo tex) {
        addTextureHexagon(name0, nameAll, nameAll, nameAll, nameAll, nameAll,
                          nameAll, tex);
    }

    const std::vector<HexagonPart>& getTextureMappingsLow(
        LandscapeTextureID t1, LandscapeTextureID t2, LandscapeTextureID t3) {
        return m_hexagonTexturesLow[t1][t2][t3];
    }
    const std::vector<HexagonPart>& getTextureMappingsHigh(
        LandscapeTextureID t1, LandscapeTextureID t2, LandscapeTextureID t3) {
        return m_hexagonTexturesHigh[t1][t2][t3];
    }

    int getTextureID(LandscapeTextureID name) {
        return m_landscapeTextureID[name];
    }

    void setLandscapeTextureID(LandscapeTextureID name, int id) {
        m_landscapeTextureID[name] = id;
    }
};

}  // namespace OpenS4::Renderer
