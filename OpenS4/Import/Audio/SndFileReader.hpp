#pragma once
#include <vector>
#include "WaveFile.hpp"

namespace OpenS4::Import
{
    class SndFileReader
    {
       public:
        SndFileReader();

       private:
        std::vector<WaveFile*> m_wavFiles;
    };
}  // namespace OpenS4::Import
