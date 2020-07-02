#pragma once

#include <stdint.h>

#include <string>
#include <vector>

namespace OpenS4::Import {
class Reader {
    std::vector<uint8_t> m_data;

   public:
    Reader() {}

    Reader(std::string file);
    Reader(std::vector<uint8_t> data);

    uint32_t read_int32(uint32_t offset);
    uint8_t read_byte(uint32_t offset);
    uint16_t read_word(uint32_t offset);

    size_t size() { return m_data.size(); }
};
}  // namespace OpenS4::Graphics
