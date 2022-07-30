#include "logger.h"

#include <cstdarg>
#include <cstdio>
#include <cstdlib>
#include <ctime>

const std::string Logger::m_filename = "log.txt";
Logger* Logger::m_this = nullptr;
std::ofstream Logger::m_logFile;

using namespace std;

const std::string currentDateTime()
{
    time_t now = time(NULL);
    struct tm tstruct;
    char buf[80];
    localtime_s(&tstruct, &now);
    strftime(buf, sizeof(buf), "%Y-%m-%d.%X", &tstruct);
    return buf;
}

const std::string logLevelText(LogLevel level)
{
    if (level == LogLevel::Debug)
        return "[DEBUG]";
    if (level == LogLevel::Info)
        return "[INFO]";
    if (level == LogLevel::Warning)
        return "[WARNING]";
    if (level == LogLevel::Error)
        return "[ERROR]";
    return "[INFO]";
}

Logger::Logger()
    : m_logDest { LogDestination::StdOut }
{
}

void Logger::log(LogLevel level, const std::string& message)
{
    if (m_logDest == LogDestination::File)
    {
        m_logFile << currentDateTime() << ": " << logLevelText(level) << ": ";
        m_logFile << message << "\n";
    }
    else
    {
        cout << currentDateTime() << ": " << logLevelText(level) << ": ";
        cout << message << "\n";

        if (m_onMessage)
            m_onMessage(currentDateTime() + ": " + logLevelText(level) + ": " + message);
        if (m_onMessageShort)
            m_onMessageShort(message);
    }
}

void Logger::log(LogLevel level, const char* format, ...)
{
    char* message = nullptr;
    int length = 0;
    va_list args;
    va_start(args, format);
    length = _vscprintf(format, args) + 1;
    message = new char[length];
    vsprintf_s(message, length, format, args);
    if (m_logDest == LogDestination::File)
    {
        m_logFile << currentDateTime() << ": " << logLevelText(level) << ": ";
        m_logFile << message << "\n";
    }
    else
    {
        cout << currentDateTime() << ": " << logLevelText(level) << ": ";
        cout << message << "\n";

        if (m_onMessage)
            m_onMessage(currentDateTime() + ": " + logLevelText(level) + ": " + message);
        if (m_onMessageShort)
            m_onMessageShort(message);
    }
    va_end(args);
    delete[] message;
}

Logger& Logger::operator<<(const std::string& message)
{
    if (m_logDest == LogDestination::File)
    {
        m_logFile << currentDateTime() << ": ";
        m_logFile << message << "\n";
    }
    else
    {
        cout << currentDateTime() << ": ";
        cout << message << "\n";
    }
    return *this;
}

Logger* Logger::instance(LogDestination dest)
{
    if (m_this == nullptr)
    {
        m_this = new Logger();

        if (dest == LogDestination::File)
            m_logFile.open(m_filename.c_str(), ios::out | ios::app);
    }
    return m_this;
}

void Logger::assignOnMessage(std::function<void(const std::string message)>& onMessageFunc)
{
    m_onMessage = onMessageFunc;
}

void Logger::assignOnMessageShort(std::function<void(const std::string message)>& onMessageFunc)
{
    m_onMessageShort = onMessageFunc;
}
