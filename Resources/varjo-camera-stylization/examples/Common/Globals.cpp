
#include "Globals.hpp"

namespace VarjoExamples
{
// Log level
LogLevel g_logLevel = LogLevel::Debug;

// Optional logging function to allow e.g. UI logging.
LogFunc g_logFunc = nullptr;

void initLog(LogFunc logFunc, LogLevel logLevel)
{
    g_logFunc = logFunc;
    g_logLevel = logLevel;
}

void writeLog(LogLevel level, const char* funcName, int lineNum, const char* prefix, const char* format, ...)
{
    if (g_logLevel < level) {
        return;
    }

    constexpr size_t lineLimit = 4096;
    char lineBuf[lineLimit];
    va_list args;
    std::string formatStr;
    // formatStr  += std::string(funcName) + "():" + std::to_string(lineNum) + ": ";
    formatStr += std::string(prefix) + format;
    va_start(args, format);
    vsprintf_s(lineBuf, lineLimit, formatStr.data(), args);
    va_end(args);

    std::string line(lineBuf);
    if (g_logFunc != nullptr) {
        g_logFunc(level, line);
    } else {
        printf("%s\n", line.c_str());
    }

    // Throw exception
    if (level == LogLevel::Critical) {
        throw std::runtime_error(line);
    }
}

}  // namespace VarjoExamples
