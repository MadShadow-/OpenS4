#pragma once

#include "Node.hpp"


namespace OpenS4::Logic::Map {

class Map {
    Node* m_nodes = nullptr;

    u64 m_sizeX;
    u64 m_sizeY;

   public:
    Map(u64 sizeX, u64 sizeY);
    Map(const Map&) = delete;
    ~Map();

    __inline Node* getNode(u64 x, u64 y) { return &m_nodes[y * m_sizeX + x]; }
};

}  // namespace OpenS4::Logic::Map
