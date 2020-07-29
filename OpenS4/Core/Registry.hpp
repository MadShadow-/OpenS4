#pragma once

#ifdef _WIN32
#include <Windows.h>
// TODO: needs global solution?
namespace OpenS4
{
    namespace Core
    {
        std::string getGamePath()
        {
            char buffer[256] = {0};
            DWORD buffer_size = 256;
            auto rc = RegGetValueA(
                HKEY_LOCAL_MACHINE,
                "SOFTWARE\\WOW6432Node\\Ubisoft\\Launcher\\Installs\\11785",
                "InstallDir",
                RRF_RT_REG_SZ,
                nullptr,
                buffer,
                &buffer_size);

            if (rc != ERROR_SUCCESS)
            {
                return "";
            }

            std::string path = buffer;

            return path;
        }
    }  // namespace Core
}  // namespace OpenS4
#else
namespace OpenS4::GraphicsReader
{
    std::string getGamePath() { return ""; }
}  // namespace OpenS4::GraphicsReader
#endif  // _WIN32

namespace OpenS4
{
    namespace Core
    {
        class Registry
        {
           public:
            static std::string getGamePath()
            {
                return OpenS4::Core::getGamePath();
            }
            static std::string getSoundFolder()
            {
                return OpenS4::Core::getGamePath() + std::string("Snd/");
            }
            static std::string getGraphicsFolder()
            {
                return OpenS4::Core::getGamePath() + std::string("Gfx/");
            }
        };
    }  // namespace Core
}  // namespace OpenS4
