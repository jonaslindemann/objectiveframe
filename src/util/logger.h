#pragma once

#include <fstream>
#include <iostream>
#include <string>

enum class LogDestination {
	File,
	StdOut,
	StdErr
};

enum class LogLevel { 
	Debug,
	Info,
	Warning,
	Error
};

class Logger {
public:
	void log(LogLevel level, const std::string& message);
	void log(LogLevel level, const char* format, ...);
	Logger& operator<<(const std::string& message);
	static Logger* instance(LogDestination dest = LogDestination::StdOut);
private:
	Logger();
	Logger(const Logger& );
	Logger& operator=(const Logger&) { return *this; };
	static const std::string m_filename;
	static Logger* m_this;
	static std::ofstream m_logFile;

	LogDestination m_logDest;

};
