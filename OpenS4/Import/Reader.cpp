#include "Reader.hpp"

#include <fstream>

namespace OpenS4::Import
{
    Reader::Reader(std::string file)
    {
        auto is = std::ifstream(file, std::ios::binary);

        if (is)
        {
            is.seekg(0, is.end);
            int length = is.tellg();
            is.seekg(0, is.beg);

            unsigned char* buffer = new unsigned char[length];

            is.read((char*)buffer, length);

            m_data = std::vector<unsigned char>(buffer, buffer + length);

            delete[] buffer;
        }

        is.close();
    }

    Reader::Reader(std::vector<uint8_t> data) { m_data = data; }

    uint32_t Reader::read_int32(uint32_t offset)
    {
        uint8_t b[4] = {0};
        for (int i = 0; i < 4; i++)
        {
            b[i] = m_data.at(offset++);
        }
        return *(uint32_t*)&b[0];
    }

    uint8_t Reader::read_byte(uint32_t offset) { return m_data.at(offset); }

    uint16_t Reader::read_word(uint32_t offset)
    {
        uint8_t b[2] = {0};
        for (int i = 0; i < 2; i++)
        {
            b[i] = m_data.at(offset++);
        }
        return *(uint16_t*)&b[0];
    }

    void Reader::read_range(uint32_t offset, uint8_t* dest, size_t size)
    {
        if (m_data.size() - offset >= size)
        {
            memcpy_s(dest, size, m_data.data() + offset, size);
        }
        else
        {
            throw std::exception("bad read");
        }
    }
}  // namespace OpenS4::Import
