#pragma once
#include <string>

#ifdef _WIN32
#define WIN32_LEAN_AND_MEAN
#define NOMINMAX
#include <Windows.h>
#else
// TODO :?
#endif  // _WIN32

namespace OpenS4
{
    namespace Core
    {
        std::string getGamePath();

        class Registry
        {
           public:
            static std::string Registry::getGamePath()
            {
                return OpenS4::Core::getGamePath();
            }
            static std::string Registry::getSoundFolder()
            {
                return OpenS4::Core::getGamePath() + std::string("Snd/");
            }
            static std::string Registry::getGraphicsFolder()
            {
                return OpenS4::Core::getGamePath() + std::string("Gfx/");
            }
        };
    }  // namespace Core
}  // namespace OpenS4
