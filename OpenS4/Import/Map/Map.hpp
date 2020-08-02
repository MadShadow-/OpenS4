#pragma once

#include <memory>
#include <string>

#include "../Reader.hpp"

namespace OpenS4::Import::Map {
enum class MapChunkType {
    /// Save Game
    CommentText = 250,
    SaveGameGeneralInformation = 130,
    SaveGameMapLandscape = 200,

    /// Game Map
    MapGeneralInformation = 1,
    MapPlayerInformation = 2,
    MapTeamInformation = 3,
    MapPreview = 4,
    MapUnknown5 = 5,
    MapObjects = 6,
    MapSettlers = 7,
    MapBuildings = 8,
    MapStacks = 9,
    MapUnknown10 = 10,
    MapQuestText = 11,
    MapQuestTip = 12,
    MapLandscape = 13,

    EndOfFile = 0
};

class MapChunk {
    std::vector<uint8_t> decode_data(std::shared_ptr<Reader> reader, int offset,
                                     int size);

    bool m_valid = true;

    MapChunkType m_chunk_type;
    uint32_t m_length = 0;

    uint32_t m_unpacked_length = 0;
    uint32_t m_checksum = 0;
    uint32_t m_unknown1 = 0;
    uint32_t m_unknown2 = 0;

    uint32_t m_offset = 0;

    std::shared_ptr<Reader> m_reader;

   public:
    MapChunk() {
        m_chunk_type = MapChunkType::EndOfFile;
        m_valid = false;
    }
    MapChunk(std::shared_ptr<Reader> reader, int offset);

    MapChunkType get_chunk_type() { return m_chunk_type; }

    Reader get_data_reader();
    MapChunkType get_type() { return m_chunk_type; }

    uint32_t calc_next_chunk_offset() { return m_offset + m_length; }

    bool is_valid() { return m_valid; }
};

class Map {
    std::string getAsString(std::shared_ptr<Reader> reader, int offset,
                            int length);
    bool isSavegame(std::shared_ptr<Reader> reader);
    int readFileHeader(std::shared_ptr<Reader> reader, int offset);
    void readFileChunks(std::shared_ptr<Reader> reader, int offset);

    uint32_t m_checksum = 0;
    uint32_t m_mapFileVersion = 0;
    std::vector<MapChunk> m_chunks;
    bool m_isValid = true;

    Map(std::shared_ptr<Reader> reader);

   public:
    Map() = delete;

    MapChunk* getChunkByType(MapChunkType type);

    bool isValid() { return m_isValid; };

    friend Map* readMap(std::string file);
};

Map* readMap(std::string file);

struct LandscapePosition {
    uint8_t height;
    uint8_t terrainType;
    uint8_t terrainSubtype;
};

class Landscape {
    std::vector<LandscapePosition> m_landscape;

    uint64_t m_width;
    uint64_t m_height;

   public:
    uint64_t getWidth() { return m_width; }
    uint64_t getHeight() { return m_height; }

    LandscapePosition& getLandscapePosition(uint64_t x, uint64_t y) {
        return m_landscape[m_width * y + x];
    }

    friend Landscape* fromMap(Map* map);
};

Landscape* fromMap(Map* map);

}  // namespace MapReader
