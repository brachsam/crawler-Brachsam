#pragma once
#include <string>
#include <stdexcept>
#include "hmap.h"

// Exception used for config errors.
class ConfigException : public std::runtime_error {
public:
    explicit ConfigException(const std::string& msg) : std::runtime_error(msg) {}
};

// ConfigManager
//
// This class reads the config file and stores all settings.
// Each line in the file should be in the form:
// key=value
//
// It only reads and returns config values.
// Other classes use ConfigManager to get the settings they need.
class ConfigManager {
private:
    std::string configPath;                         // Path of the config file.
    HashMap<std::string, std::string> values;       // Stores all key-value pairs.

    void parseLine(const std::string& line);        // Reads one line from the config file.
    static std::string trim(const std::string& s);  // Removes extra spaces.

public:
    // Creates a ConfigManager for the given config file.
    explicit ConfigManager(const std::string& path);
    ~ConfigManager();

    // Copying is not allowed.
    ConfigManager(const ConfigManager&) = delete;
    ConfigManager& operator=(const ConfigManager&) = delete;

    // Reads the config file again.
    // Throws ConfigException if the file cannot be opened.
    void reload();

    // Returns true if the key exists.
    bool hasKey(const std::string& key) const;

    // Returns the value if the key exists.
    // Otherwise, returns the default value.
    std::string getString(const std::string& key, const std::string& defaultVal) const;
    int         getInt(const std::string& key, int defaultVal) const;
    bool        getBool(const std::string& key, bool defaultVal) const;

    // Returns the value for a required key.
    // Throws ConfigException if the key is missing.
    std::string requireString(const std::string& key) const;
    int         requireInt(const std::string& key) const;
};