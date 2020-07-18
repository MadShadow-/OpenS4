#pragma once

#include <Logger/Types.hpp>
#include <string>
#include <vector>

#include "../Import/Graphics/Gfx.hpp"

namespace OpenS4::Logic {

enum class Direction {
    EAST = 0,
    SOUTH_EAST = 1,
    SOUTH_WEST = 2,
    WEST = 3,
    NORTH_WEST = 4,
    NORT_EAST = 5,

    _DIRECTION_COUNT = 6
};

enum class Race { ROMAN, MAYA, VIKING, TROJAN };

enum class RomanAnimations {
    DUMMY,
    DUMMY2,
    DUMMY3,
    DUMMY4,
    DUMMY5,
    DUMMY6,
    CARRIER_WALK,
    CARRIER_WALK_AGAVE,
    CARRIER_WALK_AMMO,
    CARRIER_WALK_ARMOR,
    CARRIER_WALK_AXE,
    CARRIER_WALK_BATTLEAXE,
    CARRIER_WALK_BLOWGUN,
    CARRIER_WALK_BOARD,
    CARRIER_WALK_BOW,
    CARRIER_WALK_BREAD,
    CARRIER_WALK_COAL,
    CARRIER_WALK_FISH,
    CARRIER_WALK_FLOUR,
    CARRIER_WALK_GOAT,
    CARRIER_WALK_GOLDBAR,
    CARRIER_WALK_GOLDORE,
    CARRIER_WALK_GRAIN,
    CARRIER_WALK_GUNPOWDER,
    CARRIER_WALK_HAMMER,
    CARRIER_WALK_HONEY,
    CARRIER_WALK_IRONBAR,
    CARRIER_WALK_IRONORE,
    CARRIER_WALK_LOG,
    CARRIER_WALK_MEAD,  // Viking Mead
    CARRIER_WALK_MEAT,
    CARRIER_WALK_PICKAXE,
    CARRIER_WALK_PIG,
    CARRIER_WALK_ROD,
    CARRIER_WALK_SAW,
    CARRIER_WALK_SCYTHE,
    CARRIER_WALK_SHEEP,
    CARRIER_WALK_SHOVEL,
    CARRIER_WALK_STONE,
    CARRIER_WALK_SULFUR,
    CARRIER_WALK_SWORD,
    CARRIER_WALK_TEQUILA,
    CARRIER_WALK_WATER,
    CARRIER_WALK_WINE,
    // CARRIER_WALK_BACKPACKCATAPULT,
    // CARRIER_WALK_GOOSE,
    // CARRIER_WALK_EXPLOSIVEARROW,
    // CARRIER_WALK_SUNFLOWEROIL,
    // CARRIER_WALK_SUNFLOWER,
    CARRIER_PICKUP,
    CARRIER_IDLE1,
    CARRIER_IDLE2,
    CARRIER_IDLE3,

    CARRIER_GO_INTO_STRIKE,
    CARRIER_STRIKE_WALK,

    DIGGER_WALK,
    DIGGER_WORK,

    _COUNT
};

RomanAnimations operator++(RomanAnimations& x);

RomanAnimations operator*(RomanAnimations c);

RomanAnimations begin(RomanAnimations r);
RomanAnimations end(RomanAnimations r);

struct AnimationFrames {
    u32 frameStart;
    u32 frameEnd;
};

struct Animation {
    u32 frameStart = 0;
    u32 frameEnd = 0;
    u32 duration = 0;
    std::string name;
};

struct TaskList {};

struct EntityProperties {
    std::string name;
    std::vector<Animation> animations;
    std::vector<TaskList> tasklists;
};

class Entity {
   public:
    EntityProperties* m_properties;
    int start_tick = 0;
    int end_tick = 0;
    int current_animation = 0;
    Direction direction;
};

EntityProperties* test_makeProperties(
    OpenS4::Import::GraphicsRegistry* registry);

}  // namespace OpenS4::Logic
