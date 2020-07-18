
#include "Map.hpp"

#include <string.h>  // memset

namespace OpenS4::Logic::Map {

Map::Map(u64 sizeX, u64 sizeY) {
    m_sizeX = sizeX;
    m_sizeY = sizeY;
    m_nodes = new Node[m_sizeX * m_sizeY];
}

Map::~Map() {
    if (m_nodes) {
        delete[] m_nodes;
        m_nodes = nullptr;
    }
}

}  // namespace OpenS4::Logic::Map
