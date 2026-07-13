#pragma once
#include <string>
#include <fstream>
#include <mutex>

// Different types of log messages.
enum class LogLevel { LDEBUG, LINFO, LWARN, LERROR };

// Logger
//
// This class is used to write log messages.
// Only one Logger object is created by the Crawler and passed to other
// classes when they need to write logs.
//
// Thread-safe: a mutex makes sure only one thread writes at a time.
class Logger {
private:
    bool logToConsole;          // True if logs should be shown on the console.
    bool logToFile;             // True if logs should be saved to a file.
    std::ofstream fileStream;   // File used for logging.
    LogLevel minLevel;          // Lowest log level that will be written.
    mutable std::mutex mtx;     // Prevents multiple threads from writing together.

    static std::string levelToString(LogLevel level);          // Converts log level to text.
    static std::string timestamp();                            // Returns the current date and time.
    void write(LogLevel level, const std::string& message);    // Writes the log message.

public:
    // Leave filePath empty ("") if you do not want file logging.
    // Messages below minLevel are ignored.
    explicit Logger(bool consoleOutput = true,
                     const std::string& filePath = "",
                     LogLevel minLevel = LogLevel::LINFO);
    ~Logger();

    // Copying is not allowed.
    Logger(const Logger&) = delete;
    Logger& operator=(const Logger&) = delete;

    // Write different types of log messages.
    void debug(const std::string& message);
    void info(const std::string& message);
    void warn(const std::string& message);
    void error(const std::string& message);
};