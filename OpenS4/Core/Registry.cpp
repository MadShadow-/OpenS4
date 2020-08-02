#pragma once
#include "Registry.hpp"

#ifdef _WIN32

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
namespace OpenS4::Core
{
    std::string getGamePath() { return ""; }
}  // namespace OpenS4::Core
#endif  // _WIN32
