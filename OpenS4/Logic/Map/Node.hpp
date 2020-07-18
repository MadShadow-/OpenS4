#pragma once

#include "../../Logger/Types.hpp"

namespace OpenS4::Logic::Map {

struct Node {
    bool blocked;
    bool walkable;
    u8 cost;
};

}  // namespace OpenS4::Logic::Map
