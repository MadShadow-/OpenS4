#pragma once

#include <Core/Types.hpp>

#include <string>

#include <vector>
#include <bitset>

namespace OpenS4::Import::Config
{
    struct ObjectDefinition
    {
        std::string name;
        u32 blocking;
        bool repellant;
    };

    enum class Goods
    {
        NO_GOOD,

        AGAVE,
        AMMO,
        ARMOR,
        AXE,
        BATTLEAXE,
        BLOWGUN,
        BOARD,
        BOW,
        BREAD,
        COAL,
        FISH,
        FLOUR,
        GOAT,
        GOLDBAR,
        GOLDORE,
        GRAIN,
        GUNPOWDER,
        HAMMER,
        HONEY,
        IRONBAR,
        IRONORE,
        LOG,
        MEAD,
        MEAT,
        PICKAXE,
        PIG,
        ROD,
        SAW,
        SCYTHE,
        SHEEP,
        SHOVEL,
        STONE,
        SULFUR,
        SWORD,
        TEQUILA,
        WATER,
        WINE,
        BACKPACKCATAPULT,
        GOOSE,
        EXPLOSIVEARROW,
        SUNFLOWEROIL,
        SUNFLOWER
    };

    enum class Race
    {
        ROMAN,
        VIKING,
        MAYA,
        DARK,
        TROJAN
    };

    struct BuildingDefinition
    {
        struct Builder
        {
            u8 xOffset;
            u8 yOffset;
            u8 direction;
        };
        struct Pile
        {
            s8 xOffset;
            s8 yOffset;

            s32 xPixelOffset;
            s32 yPixelOffset;

            Goods good;

            // true - produced, false - consumed
            bool produced;
        };
        struct BoundingRect
        {
            u8 xMin;
            u8 yMin;
            u8 xMax;
            u8 yMax;
        };


        std::string name;

        Race race;

        s8 hotspotX;
        s8 hotspotY;

        u8 costBoard;
        u8 costStone;
        u8 costGold;

        std::vector<std::bitset<32>> buildingPosLines;
        std::vector<std::bitset<32>> digPosLines;
        std::vector<std::bitset<32>> repealingPosLines;
        std::vector<std::bitset<32>> blockPosLines;
        std::vector<std::bitset<32>> waterPosLines;

        std::vector<std::bitset<32>> waterFreePosLines;
        std::vector<std::bitset<32>> waterBlockPosLines;


        s32 miniflagX;
        s32 miniflagY;

        std::vector<Builder> builderPositions;

        std::vector<Pile> piles;

        BoundingRect boundingRect;


        // patches missing here


    };

}  // namespace OpenS4::Import::Config