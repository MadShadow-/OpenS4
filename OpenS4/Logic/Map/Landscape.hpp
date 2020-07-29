#pragma once

#include <map>
#include <set>
#include <vector>

#include "../../Logger/Logger.hpp"
#include "Core/Types.hpp"

namespace OpenS4::Logic::Map {

using LandscapeTextureID = u32;

class LandscapeTextureMapper {
    std::map<std::string, LandscapeTextureID> m_textureToID;
    std::map<LandscapeTextureID, std::string> m_IDtoTexture;

    LandscapeTextureID m_textureIDCounter = 0;

   public:
    void registerTexture(std::string name) {
        if (m_textureToID.count(name) != 0) {
            OpenS4::getLogger().err("%s: texture '%s' already defined!",
                                    __FUNCTION__, name.c_str());
            return;
        }

        m_textureIDCounter++;

        m_textureToID[name] = m_textureIDCounter;
        m_IDtoTexture[m_textureIDCounter] = m_textureIDCounter;
    }
    LandscapeTextureID getTextureID(std::string name) {
        if (m_textureToID.count(name) != 0)
            return m_textureToID[name];
        else {
            OpenS4::getLogger().err("%s: texture '%s' not defined!",
                                    __FUNCTION__, name.c_str());
            return 0;
        }
    }
};

class LandscapeTextureConverter {
    std::map<u32, LandscapeTextureID> m_textureConversion;

    std::set<u32> m_alreadyPrinted;

    struct BeginEnd {
        u8 begin;
        u8 end;
        LandscapeTextureID value;
    };

    std::multimap<u32, BeginEnd> m_range;

   public:
    void setTextureConversion(u8 type, u8 subType, LandscapeTextureID id) {
        u32 idx = type;
        idx <<= 8;
        idx += subType;

        if (m_textureConversion.count(idx) != 0) {
            OpenS4::getLogger().err(
                "%s: texture conversion already set for '%d' '%d'!",
                __FUNCTION__, type, subType);
            return;
        }

        m_textureConversion[idx] = id;
    }

    void setRangedTextureConversion(u8 type, u8 subTypeBegin, u8 subTypeEnd,
                                    LandscapeTextureID id) {
        BeginEnd be;
        be.begin = subTypeBegin;
        be.end = subTypeEnd;
        be.value = id;
        m_range.emplace(type, be);
    }

    LandscapeTextureID convert(u8 type, u8 subType) {
        u32 idx = type;
        idx <<= 8;
        idx += subType;

        if (m_textureConversion.count(idx) == 0) {
            auto b = m_range.find(type);

            while (b != m_range.end()) {
                auto& be = (*b).second;

                if (subType >= be.begin && subType <= be.end) return be.value;

                ++b;
            }

            if (m_alreadyPrinted.count(idx) == 0) {
                OpenS4::getLogger().err(
                    "%s: texture conversion not found for '%d' '%d'!",
                    __FUNCTION__, type, subType);
                m_alreadyPrinted.emplace(idx);
            }
            return 0;
        }
        return m_textureConversion[idx];
    }
};

class Landscape {
    struct LandscapePosition {
        LandscapeTextureID texture;
        u32 height;
    };

    std::vector<LandscapePosition> m_landscape;

    u32 m_width;
    u32 m_height;

    __inline u32 idx(u32 x, u32 y) { return y * m_width + x; }

    struct Debug {
        u32 type;
        u32 subType;
    };

    std::vector<Debug> m_landscapeDebug;

   public:
    Landscape(u32 width, u32 height) {
        u32 size = width * height;
        m_width = width;
        m_height = height;
        m_landscape.resize(size);

        m_landscapeDebug.resize(size);
    }

    void setTerrain(u32 x, u32 y, LandscapeTextureID id) {
        m_landscape[idx(x, y)].texture = id;
    }
    LandscapeTextureID getTerrain(u32 x, u32 y) {
        return m_landscape[idx(x, y)].texture;
    }

    void setTerrainHeight(u32 x, u32 y, u32 height) {
        m_landscape[idx(x, y)].height = height;
    }

    u32 getTerrainHeight(u32 x, u32 y) { return m_landscape[idx(x, y)].height; }

    u32 getWidth() { return m_width; }
    u32 getHeight() { return m_height; }

    void setTerrainDebug(u32 x, u32 y, u32 type, u32 subType) {
        m_landscapeDebug[idx(x, y)].type = type;
        m_landscapeDebug[idx(x, y)].subType = subType;
    }

    Debug getTerrainDebug(u32 x, u32 y) { return m_landscapeDebug[idx(x, y)]; }
};

}  // namespace OpenS4::Logic::Map
