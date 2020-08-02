
#include "Logger.hpp"

#include <stdarg.h>
#include <stdio.h>

#include <ctime>
#include <iostream>
#include <sstream>

namespace OpenS4 {

void SimpleLogger::print(LogLevel level, const char* buffer) {
    std::stringstream sstream;

    time_t rawtime;
    struct tm* timeinfo;
    char buf[80];

    time(&rawtime);
    timeinfo = localtime(&rawtime);

    strftime(buf, sizeof(buf), "[%H:%M:%S] ", timeinfo);

    sstream << buf;

    switch (level) {
        case LogLevel::LERROR:
            sstream << "[ERROR] ";
            break;
        case LogLevel::WARN:
            sstream << "[WARNING] ";
            break;
        case LogLevel::INFO:
            sstream << "[INFO] ";
            break;
        default:
            sstream << "[UNKNOWN LOGLEVEL " << (int64_t)level << "] ";
            break;
    }

    sstream << buffer << "\r\n";

    m_output << sstream.str();
    if (m_print_to_console) std::cout << sstream.str();
}

void SimpleLogger::warn(const char* fmt, ...) {
    va_list args;
    va_start(args, fmt);
    int len = vsnprintf(NULL, 0, fmt, args) + 1;
    char* buffer = new char[len * sizeof(char)];
    if (NULL != buffer) {
        vsprintf_s(buffer, len, fmt, args);
        print(LogLevel::WARN, buffer);
        delete[] buffer;
    } else {
        print(LogLevel::LERROR, "failed to log warning!");
    }
    va_end(args);
}
void SimpleLogger::info(const char* fmt, ...) {
    va_list args;
    va_start(args, fmt);
    int len = vsnprintf(NULL, 0, fmt, args) + 1;
    char* buffer = new char[len * sizeof(char)];
    if (NULL != buffer) {
        vsprintf_s(buffer, len, fmt, args);
        print(LogLevel::INFO, buffer);
        delete[] buffer;
    } else {
        print(LogLevel::LERROR, "failed to log warning!");
    }
    va_end(args);
}
void SimpleLogger::err(const char* fmt, ...) {
    va_list args;
    va_start(args, fmt);
    int len = vsnprintf(NULL, 0, fmt, args) + 1;
    char* buffer = new char[len * sizeof(char)];
    if (NULL != buffer) {
        vsprintf_s(buffer, len, fmt, args);
        print(LogLevel::LERROR, buffer);
        delete[] buffer;
    } else {
        print(LogLevel::LERROR, "failed to log warning!");
    }
    va_end(args);
}

void SimpleLogger::flush() { m_output.flush(); }

SimpleLogger::SimpleLogger() {
    m_output = std::ofstream("log.txt", std::ofstream::out);
}

SimpleLogger::~SimpleLogger() { m_output.close(); }

SimpleLogger g_logger;

SimpleLogger& getLogger() { return g_logger; }
}  // namespace OpenS4
