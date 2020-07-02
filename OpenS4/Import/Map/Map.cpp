
#include "Map.hpp"

#include <Logger\Logger.hpp>

int max(int a, int b) { return a > b ? a : b; }
int min(int a, int b) { return a < b ? a : b; }

static class ChecksumCalculator {
    static std::vector<uint16_t> key_table;

    static void init_keytable() {
        for (int i = 0; i < 256; i++) {
            /// 1. Round
            int value = i;
            int pos = i;

            for (int j = 7; j >= 0; j -= 1) {
                if (pos & 1) {
                    value = value | (1 << j);
                } else {
                    value = value & (~(1 << j));
                }
                pos = pos >> 1;
            }

            value = value << 8;

            /// 2. Round
            for (int j = 7; j >= 0; j--) {
                if (value & 0x8000) {
                    value = (value + value) ^ 0x8005;
                } else {
                    value = value << 1;
                }
            }

            /// 3. round
            pos = value;

            for (int j = 15; j > 0; j--) {
                if (pos & 1) {
                    value = value | ((1 << j));
                } else {
                    value = value & (~(1 << j));
                }

                pos = pos >> 1;
            }

            ///- save key to table
            key_table.push_back(value & 0xFFFF);
        }
    }

   public:
    static uint32_t calculate_checksum(std::shared_ptr<Reader> reader,
                                       int offset, int length = -1) {
        if (key_table.size() == 0) init_keytable();

        if (length == -1) {
            length = reader->size() - offset;
        }

        uint32_t key = 0;

        for (int i = 0; i < length; i++) {
            key = (key >> 8) ^
                  (key_table[(reader->readByte(offset++) ^ key) & 0xff]);
        }

        return ((key << 8) | (key >> 8)) & 0xffff;
    }
};
std::vector<uint16_t> ChecksumCalculator::key_table;

static class AraCrypt {
    int key0;
    int key1;
    int key2;

   public:
    AraCrypt() {
        key0 = 0x30313233;
        key1 = 0x34353637;
        key2 = 0x38393031;
    }

    uint32_t get_next_key() {
        uint32_t resultKey = 0;
        uint32_t bit1 = key1 & 1;
        uint32_t bit2 = key2 & 1;

        const uint32_t KEY_MASK_A = 0x80000062;
        const uint32_t KEY_MASK_B = 0x40000020;
        const uint32_t KEY_MASK_C = 0x10000002;

        const uint32_t KEY_ROT0_A = 0x7FFFFFFF;
        const uint32_t KEY_ROT0_B = 0x3FFFFFFF;
        const uint32_t KEY_ROT0_C = 0x0FFFFFFF;

        const uint32_t KEY_ROT1_A = 0x80000000;
        const uint32_t KEY_ROT1_B = 0xC0000000;
        const uint32_t KEY_ROT1_C = 0xF0000000;

        for (int c = 0; c < 8; c++) {
            if ((key0 & 1) != 0) {
                key0 = ((KEY_MASK_A ^ key0) >> 1) | KEY_ROT1_A;

                if ((key1 & 1) != 0) {
                    key1 = ((KEY_MASK_B ^ key1) >> 1) | KEY_ROT1_B;
                    bit1 = 1;
                } else {
                    key1 = (key1 >> 1) & KEY_ROT0_B;
                    bit1 = 0;
                }
            } else {
                key0 = (key0 >> 1) & KEY_ROT0_A;

                if ((key2 & 1) != 0) {
                    key2 = ((KEY_MASK_C ^ key2) >> 1) | KEY_ROT1_C;
                    bit2 = 1;
                } else {
                    key2 = (key2 >> 1) & KEY_ROT0_C;
                    bit2 = 0;
                }
            }

            resultKey = (bit2 ^ bit1) | (resultKey << 1);
        }

        return resultKey;
    }
};

static class BitReader {
    std::vector<uint8_t> m_data;
    int m_pos = 0;

    uint32_t m_buffer_len = 0;
    uint32_t m_buffer = 0;

   public:
    BitReader(std::shared_ptr<Reader> reader, int offset, int length) {
        for (int i = 0; i < length; i++) {
            m_data.push_back(reader->readByte(offset++));
        }
    }

    uint32_t read(int length) {
        if (m_buffer_len < length) {
            uint8_t byte = m_data[m_pos++];

            m_buffer |= (byte << (24 - m_buffer_len));
            m_buffer_len += 8;
        }

        uint32_t bitValue = m_buffer >> (32 - length);

        m_buffer = m_buffer << length;
        m_buffer_len -= length;
        return bitValue;
    }

    bool eof() { return m_buffer_len == 0 && m_pos == m_data.size(); }

    int sourceLeftLength() { return max(0, m_data.size() - m_pos); }

    void resetBitBuffer() {
        m_pos =
            (m_pos - m_buffer_len /
                         8);  //- move back the inbuffer for all not used byte
        m_buffer_len = 0;     //- clear bit-buffer
        m_buffer = 0;
    }
};

static class ValueBase {
   public:
    std::vector<uint32_t> m_length;
    std::vector<uint32_t> m_value;

    ValueBase(int length) {
        for (int i = 0; i < length; i++) {
            m_length.push_back(0);
            m_value.push_back(0);
        }
    }
};

static class Uncompress {
    ValueBase m_tab0 = ValueBase(8);
    ValueBase m_tab1 = ValueBase(8);

    void initTables() {
        //-- tab0
        m_tab0.m_length[0] = 0;
        m_tab0.m_value[0] = 0;

        for (int i = 1; i < 8; i++) {
            m_tab0.m_length[i] = i - 1;
            m_tab0.m_value[i] = 1 << (i - 1);
        }

        //-- tab1
        for (int i = 0; i < 8; i++) {
            m_tab1.m_length[i] = i + 1;
            m_tab1.m_value[i] = (1 << (i + 1)) + 6;
        }
    }

    ValueBase initHuffmanTable() {
        ValueBase table(16);
        table.m_length = {0x2, 0x3, 0x3, 0x4, 0x4, 0x4, 0x4, 0x4,
                          0x4, 0x4, 0x4, 0x4, 0x4, 0x5, 0x5, 0x5};
        table.m_value = {0x0,  0x4,  0xC,  0x14, 0x24, 0x34, 0x44, 0x54,
                         0x64, 0x74, 0x84, 0x94, 0xA4, 0xB4, 0xD4, 0xF4};

        return table;
    }

    std::vector<uint16_t> init_char_code_table() {
        return std::vector<uint16_t>{
            0x100, 0x101, 0x102, 0x103, 0x104, 0x105, 0x106, 0x107, 0x108,
            0x109, 0x10A, 0x10B, 0x10C, 0x10D, 0x10E, 0x10F, 0x0,   0x20,
            0x30,  0xFF,  0x1,   0x2,   0x3,   0x4,   0x5,   0x6,   0x7,
            0x8,   0x9,   0xA,   0xB,   0xC,   0xD,   0xE,   0xF,   0x10,
            0x11,  0x12,  0x13,  0x14,  0x15,  0x16,  0x17,  0x18,  0x19,
            0x1A,  0x1B,  0x1C,  0x1D,  0x1E,  0x1F,  0x21,  0x22,  0x23,
            0x24,  0x25,  0x26,  0x27,  0x28,  0x29,  0x2A,  0x2B,  0x2C,
            0x2D,  0x2E,  0x2F,  0x31,  0x32,  0x33,  0x34,  0x35,  0x36,
            0x37,  0x38,  0x39,  0x3A,  0x3B,  0x3C,  0x3D,  0x3E,  0x3F,
            0x40,  0x41,  0x42,  0x43,  0x44,  0x45,  0x46,  0x47,  0x48,
            0x49,  0x4A,  0x4B,  0x4C,  0x4D,  0x4E,  0x4F,  0x50,  0x51,
            0x52,  0x53,  0x54,  0x55,  0x56,  0x57,  0x58,  0x59,  0x5A,
            0x5B,  0x5C,  0x5D,  0x5E,  0x5F,  0x60,  0x61,  0x62,  0x63,
            0x64,  0x65,  0x66,  0x67,  0x68,  0x69,  0x6A,  0x6B,  0x6C,
            0x6D,  0x6E,  0x6F,  0x70,  0x71,  0x72,  0x73,  0x74,  0x75,
            0x76,  0x77,  0x78,  0x79,  0x7A,  0x7B,  0x7C,  0x7D,  0x7E,
            0x7F,  0x80,  0x81,  0x82,  0x83,  0x84,  0x85,  0x86,  0x87,
            0x88,  0x89,  0x8A,  0x8B,  0x8C,  0x8D,  0x8E,  0x8F,  0x90,
            0x91,  0x92,  0x93,  0x94,  0x95,  0x96,  0x97,  0x98,  0x99,
            0x9A,  0x9B,  0x9C,  0x9D,  0x9E,  0x9F,  0xA0,  0xA1,  0xA2,
            0xA3,  0xA4,  0xA5,  0xA6,  0xA7,  0xA8,  0xA9,  0xAA,  0xAB,
            0xAC,  0xAD,  0xAE,  0xAF,  0xB0,  0xB1,  0xB2,  0xB3,  0xB4,
            0xB5,  0xB6,  0xB7,  0xB8,  0xB9,  0xBA,  0xBB,  0xBC,  0xBD,
            0xBE,  0xBF,  0xC0,  0xC1,  0xC2,  0xC3,  0xC4,  0xC5,  0xC6,
            0xC7,  0xC8,  0xC9,  0xCA,  0xCB,  0xCC,  0xCD,  0xCE,  0xCF,
            0xD0,  0xD1,  0xD2,  0xD3,  0xD4,  0xD5,  0xD6,  0xD7,  0xD8,
            0xD9,  0xDA,  0xDB,  0xDC,  0xDD,  0xDE,  0xDF,  0xE0,  0xE1,
            0xE2,  0xE3,  0xE4,  0xE5,  0xE6,  0xE7,  0xE8,  0xE9,  0xEA,
            0xEB,  0xEC,  0xED,  0xEE,  0xEF,  0xF0,  0xF1,  0xF2,  0xF3,
            0xF4,  0xF5,  0xF6,  0xF7,  0xF8,  0xF9,  0xFA,  0xFB,  0xFC,
            0xFD,  0xFE,  0x110, 0x111};
    }

    class Writer {
        std::vector<uint8_t> m_data;
        int m_pos = 0;

       public:
        Writer(int length) {
            for (int i = 0; i < length; i++) {
                m_data.push_back(0);
            }
        }

        uint8_t getByte(int offset) {
            if ((offset < 0) || (offset > m_data.size())) {
                // TODO
                return -1;
            }

            return m_data[offset];
        }

        auto getWriteOffset() { return m_pos; }

        auto getLength() { return m_data.size(); }

        auto eof() { return m_pos >= m_data.size(); }

        auto getLeftSize() { return m_data.size() - m_pos; }

        auto setByte(uint8_t value) {
            if ((m_pos < 0) || (m_pos > m_data.size())) {
                // TODO
                return false;
            }

            m_data[m_pos] = value;
            m_pos++;

            return true;
        }

        auto getData() { return m_data; }
    };

   public:
    Uncompress() { initTables(); }

    Reader unpack(std::shared_ptr<Reader> reader, int inOffset, int inLength,
                  int outLength) {
        auto huffmanTable = initHuffmanTable();

        std::vector<uint32_t> codeQuantities;
        for (int i = 0; i < 274; i++) codeQuantities.push_back(0);

        bool done = false;

        auto codeTable = init_char_code_table();

        BitReader bitReader(reader, inOffset, inLength);

        Writer writer(outLength);

        int counter = 0;

        while (!bitReader.eof()) {
            uint32_t codeType = bitReader.read(4);

            if (counter == 4096) {
                int x = 0;
                x++;
            }

            if (codeType < 0) {
                // TODO
                break;
            }

            uint32_t codeWord = 0;
            uint32_t codeWord_length = huffmanTable.m_length[codeType];
            uint32_t codeWord_index = huffmanTable.m_value[codeType];

            if (codeWord_length > 0) {
                codeWord_index += bitReader.read(codeWord_length);

                if (codeWord_index >= 274) {
                    done = false;
                    break;
                }
            }

            codeWord = codeTable[codeWord_index];

            counter++;
            // std::cout << counter << " " << codeWord << std::endl;

            codeQuantities[codeWord]++;

            if (codeWord < 256) {
                // TODO check buffer size here
                // data.push_back(codeWord);
                if (writer.eof()) {
                    break;
                }

                writer.setByte(codeWord);
            } else if (codeWord == 272) {
                createCodeTableFromQuantities(codeTable, codeQuantities);

                int base = 0;
                int length = 0;

                for (int i = 0; i < 16; i++) {
                    length--;

                    int bitValue = 0;
                    do {
                        length++;
                        bitValue = bitReader.read(1);
                    } while (bitValue == 0);

                    huffmanTable.m_value[i] = base;
                    huffmanTable.m_length[i] = length;

                    base += (1 << length);
                }
            } else if (codeWord == 273) {
                if (bitReader.sourceLeftLength() > 2) {
                    if (writer.eof()) {
                        // TODO
                        // std::cout << "End-of-stream but Data buffer is not
                        // empty ("
                        //	<< bitReader.sourceLeftLength() << " IN bytes
                        // left; " << writer.getLeftSize() << " i OUT bytes
                        // left)? Out of sync!";
                        break;
                    }

                    bitReader.resetBitBuffer();
                } else {
                    if (!bitReader.eof()) {
                        // TODO
                        // std::cout << "Done uncompress (" <<
                        // bitReader.sourceLeftLength() << " IN bytes left; " <<
                        // writer.getLeftSize() << " OUT bytes left)!" <<
                        // std::endl;
                    }

                    done = true;
                    break;
                }
            } else {
                //- copy from dictionary
                if (!fromDictionary(bitReader, writer, codeWord)) {
                    break;
                }
            }
        }

        if (!done) {
            OpenS4::getLogger().warn("Error during decompression");
            return Reader();
        }

        return Reader(writer.getData());
    }

    void createCodeTableFromQuantities(std::vector<uint16_t>& codes,
                                       std::vector<uint32_t>& quantities) {
        std::vector<uint32_t> tmpQuantity;
        for (int i = 0; i < 274; i++) tmpQuantity.push_back(0);

        for (int i = 0; i < 274; i++) {
            int count = quantities[i];

            tmpQuantity[i] = count;
            codes[i] = i;

            quantities[i] = count / 2;
        }

        sortCodeTable(codes, tmpQuantity, 274);
    }

    void sortCodeTable(std::vector<uint16_t>& codes,
                       std::vector<uint32_t>& quantities, uint32_t length) {
        int pos = 0;
        int delta = 0;
        int srcCode = 0;
        int srcQuantity = 0;
        int srcPos = 0;
        int stepSize = 40;

        do {
            for (int i = stepSize; i < length; i++) {
                srcCode = codes[i];
                srcQuantity = quantities[i];

                pos = i;

                while (pos >= stepSize) {
                    srcPos = pos - stepSize;
                    delta = quantities[srcPos] - srcQuantity;
                    if (delta == 0) {
                        delta = codes[srcPos] - srcCode;
                    }

                    if (delta >= 0) {
                        break;
                    }

                    codes[pos] = codes[srcPos];
                    quantities[pos] = quantities[srcPos];

                    pos = pos - stepSize;
                }

                codes[pos] = srcCode;
                quantities[pos] = srcQuantity;
            }

            stepSize = (stepSize / 3) | 0;  //- use floor!

        } while (stepSize > 0);
    }

    bool fromDictionary(BitReader& reader, Writer& writer, int codeword) {
        int entryLenght = 0;
        int bitValue = 0;
        int copyOffset = 0;

        if (codeword < 264) {
            entryLenght = codeword - 256;
        } else {
            int length = m_tab1.m_length[codeword - 264];

            int ReadInByte = reader.read(length);

            if (ReadInByte < 0) {
                // this.log.log("ReadInByte == 0 -> out of sync!");
                // TODO
                return false;
            }

            entryLenght = m_tab1.m_value[codeword - 264] + ReadInByte;
        }

        bitValue = reader.read(3);
        if (bitValue < 0) {
            // this.log.log("bitValue < 0 -> out of sync!");
            // TODO
            return false;
        }

        int length = m_tab0.m_length[bitValue] + 1;
        int baseValue = m_tab0.m_value[bitValue];

        bitValue = reader.read(8);
        copyOffset = bitValue << length;

        bitValue = reader.read(length);
        if (bitValue < 0) {
            // this.log.log("bit_value < 0 -> out of sync!");
            return false;
        }

        entryLenght += 4;

        //- check if destination is big enough:
        // if (writer.getWriteOffset() + entryLenght > writer.getLength()) {
        // this.log.log("buffer for outData is to small!");
        // return false;
        //}
        // TODO

        //- source position in buffer
        int srcPos = writer.getWriteOffset() -
                     ((bitValue | copyOffset) + (baseValue << 9));

        //- we need to use single-byte-copy the data case, the src and dest can
        // be overlaped
        for (int i = entryLenght; i > 0; i--) {
            writer.setByte(writer.getByte(srcPos));
            srcPos++;
        }

        return true;
    }
};

namespace OpenS4::Import::Map {
std::vector<uint8_t> MapChunk::decode_data(std::shared_ptr<Reader> reader,
                                           int offset, int size) {
    std::vector<uint8_t> data;

    int max_size = max(0, reader->size() - offset);
    size = min(max_size, size);

    AraCrypt crypt;

    for (int i = 0; i < size; i++) {
        data.push_back((reader->readByte(offset++) ^ crypt.get_next_key()) &
                       0xFF);
    }

    return data;
}

MapChunk::MapChunk(std::shared_ptr<Reader> reader, int offset) {
    int section_header_size = 24;
    std::vector<uint8_t> plain =
        decode_data(reader, offset, section_header_size);

    if (plain.size() != section_header_size) {
        m_valid = false;
        return;
    }

    Reader r(plain);

    m_chunk_type = (MapChunkType)r.readInt32(0);
    m_length = r.readInt32(4);

    if (m_chunk_type == MapChunkType::EndOfFile) {
        m_valid = false;
        return;
    }

    m_unpacked_length = r.readInt32(8);
    m_checksum = r.readInt32(12);
    m_unknown1 = r.readInt32(16);
    m_unknown2 = r.readInt32(20);

    m_offset = section_header_size + offset;

    m_reader = reader;
}

Reader MapChunk::get_data_reader() {
    Uncompress un;
    std::vector<uint8_t> dest;
    dest.resize(m_unpacked_length);
    return un.unpack(m_reader, m_offset, m_length, m_unpacked_length);
}

std::string Map::getAsString(std::shared_ptr<Reader> reader, int offset,
                             int length) {
    std::string str = "";

    for (int i = offset; i < offset + length; i++) {
        str += (char)reader->readByte(i);
    }

    return str;
}

bool Map::isSavegame(std::shared_ptr<Reader> reader) {
    std::string str = getAsString(reader, 0, 4);

    if (str == "MZ\0x90\x00") {
        return true;
    }
    return false;
}

int Map::readFileHeader(std::shared_ptr<Reader> reader, int offset) {
    m_checksum = reader->readInt32(offset);
    m_mapFileVersion = reader->readInt32(offset + 4);

    return 4 * 2;
}

void Map::readFileChunks(std::shared_ptr<Reader> reader, int offset) {
    while (offset > 0) {
        MapChunk chunk(reader, offset);
        if (!chunk.is_valid()) {
            return;
        }

        m_chunks.push_back(chunk);

        offset = chunk.calc_next_chunk_offset();
    }
}

Map::Map(std::shared_ptr<Reader> reader) {
    int offset = 0;
    if (isSavegame(reader)) offset = 6656;

    offset += readFileHeader(reader, offset);

    uint32_t actual_checksum =
        ChecksumCalculator::calculate_checksum(reader, offset);

    if (m_checksum != actual_checksum) {
        OpenS4::getLogger().warn(
            "Mapfile is broken: Checksums do not match (%d - %d)", m_checksum,
            actual_checksum);
        m_isValid = false;
        return;
    }

    readFileChunks(reader, offset);
}

MapChunk* Map::getChunkByType(MapChunkType type) {
    for (int i = 0; i < m_chunks.size(); i++) {
        if (m_chunks[i].get_type() == type) return &m_chunks[i];
    }
    return nullptr;
}

Map* readMap(std::string file) {
    Map* map;

    try {
        map = new Map(std::shared_ptr<Reader>(new Reader(file)));
    } catch (std::exception& e) {
        OpenS4::getLogger().err("read_map failed for file: %s", file.c_str());
        map = nullptr;
    }
    return map;
}

Landscape* fromMap(Map* map) {
    MapChunk* chunk =
        map->getChunkByType(OpenS4::Import::Map::MapChunkType::MapLandscape);

    if (chunk == nullptr) return nullptr;

    auto re = chunk->get_data_reader();

    int width = (int)sqrt(re.size() / 4);
    int height = (int)sqrt(re.size() / 4);

    Landscape* landscape = new Landscape();
    landscape->m_height = height;
    landscape->m_width = width;
    landscape->m_landscape.resize(width * height);

    for (uint_fast32_t y = 0; y < height; y++) {
        for (uint_fast32_t x = 0; x < width; x++) {
            LandscapePosition pos;
            pos.height = re.readByte(y * width * 4 + x * 4);
            pos.terrainSubtype = re.readByte(y * width * 4 + x * 4 + 1);
            pos.terrainType = re.readByte(y * width * 4 + x * 4 + 2);

            // + 3 is always 0x40
            landscape->m_landscape[y * width + x] = pos;
        }
    }

    return landscape;
}
}  // namespace MapReader
