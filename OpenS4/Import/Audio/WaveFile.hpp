#include <Core/Types.hpp>

namespace OpenS4
{
    namespace Import
    {
        class WaveFile
        {
           public:
            WaveFile(u8* data, size_t size) : m_data(data), m_size(size){};
            ~WaveFile() { delete[] m_data; }
            WaveFile(const WaveFile& obj)
            {
                m_size = obj.m_size;
                m_data = new u8[m_size];
                memcpy_s(m_data, m_size, obj.m_data, m_size);
            }
            size_t getSize() { return m_size; }
            u8* getData() { return m_data; }

           private:
            u8* m_data;
            size_t m_size;
        };
    }  // namespace Import
}  // namespace OpenS4
