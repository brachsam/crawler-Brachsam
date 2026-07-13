#include "Logger.h"
#include <iostream>
#include <ctime>
#include <iomanip>
#include <sstream>

// Creates the Logger object.
Logger::Logger(bool consoleOutput, const std::string& filePath, LogLevel minLevel)
    : logToConsole(consoleOutput), logToFile(!filePath.empty()), minLevel(minLevel) {
    if (logToFile) {
        // Open the log file in append mode.
        fileStream.open(filePath, std::ios::app);

        // If the file cannot be opened, use console logging only.
        if (!fileStream.is_open()) {
            logToFile = false;
            std::cerr << "Logger: could not open log file '" << filePath
                      << "', falling back to console-only logging.\n";
        }
    }
}

// Closes the log file.
Logger::~Logger() {
    if (fileStream.is_open()) fileStream.close();
}

// Converts the log level to text.
std::string Logger::levelToString(LogLevel level) {
    switch (level) {
        case LogLevel::LDEBUG: return "DEBUG";
        case LogLevel::LINFO:  return "INFO";
        case LogLevel::LWARN:  return "WARN";
        case LogLevel::LERROR: return "ERROR";
    }
    return "UNKNOWN";
}

// Returns the current date and time.
std::string Logger::timestamp() {
    std::time_t now = std::time(nullptr);
    std::tm tmBuf;

#if defined(_WIN32)
    // Windows
    localtime_s(&tmBuf, &now);
#else
    // Linux/macOS
    localtime_r(&now, &tmBuf);
#endif

    std::ostringstream oss;
    oss << std::put_time(&tmBuf, "%Y-%m-%d %H:%M:%S");
    return oss.str();
}

// Writes a log message.
void Logger::write(LogLevel level, const std::string& message) {

    // Ignore messages below the minimum log level.
    if (static_cast<int>(level) < static_cast<int>(minLevel)) return;

    // Only one thread can write at a time.
    std::lock_guard<std::mutex> lock(mtx);

    // Create the final log message.
    std::string line = "[" + timestamp() + "] [" + levelToString(level) + "] " + message;

    // Print the log to the console.
    if (logToConsole) {
        if (level == LogLevel::LERROR) std::cerr << line << "\n";
        else                            std::cout << line << "\n";
    }

    // Save the log to the file.
    if (logToFile) {
        fileStream << line << "\n";
        fileStream.flush();   // Save the data immediately.
    }
}

// Writes a DEBUG log.
void Logger::debug(const std::string& message) { write(LogLevel::LDEBUG, message); }

// Writes an INFO log.
void Logger::info(const std::string& message)  { write(LogLevel::LINFO,  message); }

// Writes a WARNING log.
void Logger::warn(const std::string& message)  { write(LogLevel::LWARN,  message); }

// Writes an ERROR log.
void Logger::error(const std::string& message) { write(LogLevel::LERROR, message); }