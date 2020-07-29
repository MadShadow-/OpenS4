#include "SndFileReader.hpp"

#include "../Reader.hpp"
#include "Core/Registry.hpp"
#include "Core/Types.hpp"

#include <fstream>
#include <Core/Timer.hpp>
#include <iostream>

namespace OpenS4::Import
{
    SndFileReader::SndFileReader()
    {
        OpenS4::Core::Timer timer;
        std::string sndFolder = OpenS4::Core::Registry::getSoundFolder();

        // indexes start at byte 24
        u32 silOffset = 24;
        Reader soundIndexList(sndFolder + "0.sil");

        // number of wave files
        u32 nEntries = (soundIndexList.size() - silOffset) / 4;

        // save offset values
        std::vector<u32> offsets;
        offsets.reserve(nEntries);
        u32 offset;
        for (s32 i = 0; i < nEntries; i++)
        {
            offset = soundIndexList.read_int32(silOffset);
            if (offset != 0)
                offsets.push_back(offset);
            silOffset += 4;
        }
        nEntries = offsets.size();

        // reading the wave files
        m_wavFiles.reserve(nEntries);
        Reader wavContainer(sndFolder + "0.snd");
        uint8_t* pData{};
        u32 fileStart;
        u32 fileEnd;
        size_t i;
        for (i = 0; i < (nEntries - 1); i++)
        {
            
            // 8 bytes after the start offset a wave file starts
            // and ends at the beginning of the next file
            fileStart = offsets.at(i) + 8;
            fileEnd = offsets.at(i + 1) - fileStart;

            pData = wavContainer.read_area(fileStart, fileEnd);
            m_wavFiles.push_back(new WaveFile(pData, fileEnd));
        }

        // read last file
        fileStart = offsets.at(i) + 8;
        fileEnd = wavContainer.size() - fileStart;

        pData = wavContainer.read_area(fileStart, fileEnd);
        m_wavFiles.push_back(new WaveFile(pData, fileEnd));

        // logger bugged
        std::cout << "Reading audio files finished in " << timer.seconds() << " seconds. " << std::endl;
        //OpenS4::getLogger().info("Reading audio files finished in %d seconds", timer.seconds());
        /*
        std::ofstream file;
        u32 count{};
        for (auto waveFile : m_wavFiles)
        {
            char* ptr = (char*)waveFile->getData();
            file.open(std::string("audio/") + std::to_string(count) + ".wav",
                      std::ios_base::out | std::ios_base::binary);
            file.write(ptr, waveFile->getSize());
            file.close();
            count++;
        }
        */
    }
}  // namespace OpenS4::Import
