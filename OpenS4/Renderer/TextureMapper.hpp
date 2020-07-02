#pragma once

#include <algorithm>
#include <map>
#include <string>
#include <vector>

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
    std::map<int, std::map<int, std::map<int, std::vector<HexagonInformation>>>>
        m_textures;

    void sort(int& a, int& b, int& c) {
        if (a > b) {
            std::swap(a, b);
        }
        if (a > c) {
            std::swap(a, c);
        }
        if (b > c) {
            std::swap(b, c);
        }
    }

    std::map<int, int> m_game_texture_id_to_texture;
    std::map<int, int> m_texture_to_texture_id;

    std::map<int, LandscapeTextureID> m_gameTextureMapping;

    std::map<LandscapeTextureID, int> m_landscapeTextureID;
    std::map<std::string, LandscapeTextureID>
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

    LandscapeTextureID getOrMake(std::string name) {

        if (m_landscapeTextureNameToLandscapeTextureID.count(name) == 0) {
            m_landscapeTextureNameToLandscapeTextureID[name] =
                ++textureIDCounter;
        }

        return m_landscapeTextureNameToLandscapeTextureID[name];
    }

   public:
    void setGameTextureMapping(int id, int id2, LandscapeTextureID texID) {
        m_gameTextureMapping[(id2 << 8) + id] = texID;
    }

    void setGameTextureMapping(int id, int id2, std::string name) {
        m_gameTextureMapping[(id2 << 8) + id] = getOrMake(name);
    }

    LandscapeTextureID getGameTextureMapping(int id2, int id) {
        if (m_gameTextureMapping.count((id2 << 8) + id) == 0) {
            std::cout << "Unknown Texture: " << id << " " << id2 << std::endl;
        }

        return m_gameTextureMapping[(id2 << 8) + id];
    }

    /*
            @param name Name of the texture.
            @param id ID of the texture in LandscapeTextures.
    */
    void setLandscapeTextureID(LandscapeTextureID name, int id) {
        m_landscapeTextureID[name] = id;
    }

    void setLandscapeTextureID(std::string name, int id) {
        setLandscapeTextureID(getOrMake(name), id);
    }

    LandscapeTextureID getLandscapeTextureID(std::string name) {
        return getOrMake(name);
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

    void addTextureHexagon(LandscapeTextureID name0, LandscapeTextureID nameAll,
                           HexInfo tex) {
        addTextureHexagon(name0, nameAll, nameAll, nameAll, nameAll, nameAll,
                          nameAll, tex);
    }

    void addTextureHexagon(std::string name0, std::string nameAll,
                           HexInfo tex) {
        LandscapeTextureID n0 =
            m_landscapeTextureNameToLandscapeTextureID[name0];
        LandscapeTextureID nAll =
            m_landscapeTextureNameToLandscapeTextureID[nameAll];

        addTextureHexagon(n0, nAll, nAll, nAll, nAll, nAll, nAll, tex);
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

    void addTextureMapping(int t1, int t2, int t3, HexagonInformation tex) {
        sort(t1, t2, t3);
        m_textures[t1][t2][t3].push_back(tex);
    }

    const std::vector<HexagonInformation>& getTextureMappings(int t1, int t2,
                                                              int t3) {
        sort(t1, t2, t3);
        return m_textures[t1][t2][t3];
    }

    int get_texture_by_game_texture_id(int id) {
        if (!m_game_texture_id_to_texture.count(id)) {
            int a = 0x0000ff00 & id;
            int b = 0x000000ff & id;

            std::cout << "Unknown Texture ID: " << std::dec << id << " "
                      << std::hex << id << std::dec << " " << (a >> 8) << " "
                      << b << std::endl;
            m_game_texture_id_to_texture[id] = 0;  // Textures::UNKNOWN;
        }
        return m_game_texture_id_to_texture[id];
    }

    void set_texture_by_game_texture_id(int id, int texture) {
        m_game_texture_id_to_texture[id] = texture;
    }

    void set_texture_by_game_texture_id(int id, int id2, int texture) {
        m_game_texture_id_to_texture[(id2 << 8) + id] = texture;
    }

    int get_landscape_id_by_texture(int texture) {
        return m_texture_to_texture_id[texture];
    }

    void set_texture_id_by_texture(int texture, int id) {
        m_texture_to_texture_id[texture] = id;
    }
};

}  // namespace OpenS4::Renderer
