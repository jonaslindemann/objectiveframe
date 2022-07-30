#pragma once

#include <fstream>
#include <functional>
#include <iostream>
#include <string>

enum class LogDestination
{
    File,
    StdOut,
    StdErr
};

enum class LogLevel
{
    Debug,
    Info,
    Warning,
    Error
};

typedef std::function<void(const std::string message)> LoggerMessageFunc;

class Logger
{
public:
    void log(LogLevel level, const std::string& message);
    void log(LogLevel level, const char* format, ...);
    Logger& operator<<(const std::string& message);
    static Logger* instance(LogDestination dest = LogDestination::StdOut);

    void assignOnMessage(std::function<void(const std::string message)>& onMessageFunc);
    void assignOnMessageShort(std::function<void(const std::string message)>& onMessageFunc);

private:
    Logger();
    Logger(const Logger&);
    Logger& operator=(const Logger&) { return *this; };
    static const std::string m_filename;
    static Logger* m_this;
    static std::ofstream m_logFile;

    LogDestination m_logDest;

    std::function<void(const std::string message)> m_onMessage;
    std::function<void(const std::string message)> m_onMessageShort;
};
