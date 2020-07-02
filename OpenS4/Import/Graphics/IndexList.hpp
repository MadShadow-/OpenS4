#pragma once

#include <memory>
#include <stdexcept>

#include <Import/Reader.hpp>

namespace OpenS4::Import {
class IndexList {
   protected:
    std::vector<uint32_t> m_header;
    uint32_t m_image_count = 0;
    std::vector<uint32_t> m_offsets;

   public:
    IndexList(std::shared_ptr<OpenS4::Import::Reader> reader) {
        {
            // 5 * 4 byte header
            m_header.push_back(reader->read_int32(0));
            m_header.push_back(reader->read_int32(4));
            m_header.push_back(reader->read_int32(8));
            m_header.push_back(reader->read_int32(12));
            m_header.push_back(reader->read_int32(16));

            m_image_count = (reader->size() - 20) / 4;
            uint32_t offset = 20;
            for (auto i = 0; i < m_image_count; i++) {
                m_offsets.push_back(reader->read_int32(offset));
                offset += 4;
            }
        }
    }

    uint32_t getImageCount() { return m_image_count; }

    uint32_t get_offset(uint32_t idx) { return m_offsets[idx]; }
};

class GraphicsIndexList : public IndexList {
   public:
    GraphicsIndexList(std::shared_ptr<Reader> reader) : IndexList(reader) {}
};

class DirectionIndexList : public IndexList {
   public:
    DirectionIndexList(std::shared_ptr<Reader> reader) : IndexList(reader) {}

    uint32_t lookup(uint32_t graphics_index_list_index) {
        auto offset = graphics_index_list_index * 4 + 20;
        auto last_good_index = 0;
        for (auto i = 0; i < m_offsets.size(); i++) {
            if (m_offsets[i] > offset) {
                return last_good_index;
            } else if (m_offsets[i] != 0) {
                last_good_index = i;
            }
        }
        return last_good_index;
    }
};

class JobIndexList : public IndexList {
   public:
    JobIndexList(std::shared_ptr<Reader> reader) : IndexList(reader) {}

    uint32_t lookup(uint32_t graphics_index_list_index) {
        auto offset = graphics_index_list_index * 4 + 20;
        auto last_good_index = 0;
        for (auto i = 0; i < m_offsets.size(); i++) {
            if (m_offsets[i] > offset) {
                return last_good_index;
            } else if (m_offsets[i] != 0) {
                last_good_index = i;
            }
        }
        return last_good_index;
    }
};

class PaletteIndexList : public IndexList {
   public:
    PaletteIndexList(std::shared_ptr<Reader> reader) : IndexList(reader) {}
};

}  // namespace OpenS4::Graphics
