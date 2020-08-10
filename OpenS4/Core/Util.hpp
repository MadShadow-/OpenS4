#pragma once

#include <fstream>
#include <iostream>
#include <vector>

#include "Types.hpp"

namespace OpenS4::Core::Util
{
    class FileReader
    {
        std::vector<u8> m_buffer;
        u32 m_bufferOffset = 0;

        std::ifstream m_fileStream;

        __inline void load(u32 bytes)
        {
            while (m_buffer.size() - m_bufferOffset < bytes)
            {
                m_buffer.erase(m_buffer.begin(),
                               m_buffer.begin() + m_bufferOffset);
                m_bufferOffset = 0;

                u8 buffer[4096];
                m_fileStream.read((char*)buffer, 4096);
                auto count = m_fileStream.gcount();
                m_buffer.insert(m_buffer.end(), buffer, buffer + count);
            }
        }

       public:
        FileReader(std::string file)
        {
            m_fileStream = std::ifstream(file, std::ios::in | std::ios::binary);
        }
        ~FileReader() { m_fileStream.close(); }

        __inline u32 read_u32()
        {
            load(4);
            u8 b0 = m_buffer[m_bufferOffset++];
            u8 b1 = m_buffer[m_bufferOffset++];
            u8 b2 = m_buffer[m_bufferOffset++];
            u8 b3 = m_buffer[m_bufferOffset++];

            u32 value = b0 | (b1 << 8) | (b2 << 16) | (b3 << 24);
            return value;
        }
        __inline u8 read_u8()
        {
            load(1);
            return m_buffer[m_bufferOffset++];
        }

        std::vector<u32> read_u32vector()
        {
            std::vector<u8> u = read_u8data();

            std::vector<u32> u32Data;
            u32Data.resize(u.size() / sizeof(u32));

            memcpy(u32Data.data(), u.data(), u.size());
            return u32Data;
        }

        float read_float()
        {
            u32 u = read_u32();
            float f = *(float*)&u;
            return f;
        }

        std::vector<u8> read_u8data()
        {
            u32 length = read_u32();

            load(length);

            std::vector<u8> buffer;

            buffer.insert(buffer.end(),
                          m_buffer.begin() + m_bufferOffset,
                          m_buffer.begin() + m_bufferOffset + length);
            m_bufferOffset += length;

            return buffer;
        }
    };

    class FileWriter
    {
        std::ofstream m_fileStream;

        std::vector<u8> m_buffer;

        void writeBuffer()
        {
            m_fileStream.write((char*)m_buffer.data(), m_buffer.size());
            m_buffer.clear();
        }

       public:
        FileWriter(std::string file)
        {
            m_fileStream =
                std::ofstream(file, std::ios::out | std::ios::binary);
        }
        ~FileWriter()
        {
            writeBuffer();
            m_fileStream.close();
        }

        __inline void write_u8(u8 value)
        {
            m_buffer.push_back(value);
            if (m_buffer.size() > 4 * 1024 * 1024)  // 4 MB ?
            {
                writeBuffer();
            }
        }

        __inline void write_u32(u32 value)
        {
            u8 b0 = value & 0xff;
            u8 b1 = (value >> 8) & 0xff;
            u8 b2 = (value >> 16) & 0xff;
            u8 b3 = (value >> 24) & 0xff;

            write_u8(b0);
            write_u8(b1);
            write_u8(b2);
            write_u8(b3);
        }

        void write_u32vector(const std::vector<u32>& vector)
        {
            write_u8data((u8*)vector.data(), vector.size() * sizeof(u32));
        }

        void write_u8data(const u8* data, u32 length)
        {
            write_u32(length);
            m_buffer.insert(m_buffer.end(), data, data + length);
        }

        __inline void write_float(float value)
        {
            u32 u = *(u32*)&value;
            write_u32(u);
        }
    };

}  // namespace OpenS4::Core::Util
