#pragma once

#include <fstream>

#include <Core/Types.hpp>

namespace OpenS4 {

enum class LogLevel { ERROR, WARN, INFO };

class SimpleLogger {
    std::ofstream m_output;
    bool m_print_to_console = false;

    void print(LogLevel level, const char* buffer);

   public:
    SimpleLogger();
    ~SimpleLogger();

    void warn(const char* fmt, ...);
    void info(const char* fmt, ...);
    void err(const char* fmt, ...);

    void flush();

    void printToConsole(bool b) { m_print_to_console = b; }
};

SimpleLogger& getLogger();
}  // namespace OpenS4
