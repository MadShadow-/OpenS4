#pragma once

#include <stdint.h>

#include <string>
#include <vector>

class Reader {
    std::vector<uint8_t> m_data;

   public:
    Reader() {}

    Reader(std::string file);
    Reader(std::vector<uint8_t> data);

    uint32_t readInt32(uint32_t offset);
    uint8_t readByte(uint32_t offset);
    uint16_t readWord(uint32_t offset);

    size_t size() { return m_data.size(); }
};
