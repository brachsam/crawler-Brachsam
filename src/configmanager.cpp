#include "configmanager.h"
#include <fstream>
#include <sstream>
#include <cctype>

// Creates the ConfigManager object.
ConfigManager::ConfigManager(const std::string& path) : configPath(path) {
    reload();   // Load the config file.
}

// Destroys the ConfigManager object.
ConfigManager::~ConfigManager() {}

// Removes extra spaces from the beginning and end of a string.
std::string ConfigManager::trim(const std::string& s) {
    size_t start = 0;
    size_t end = s.size();

    while (start < end && std::isspace((unsigned char) s[start])) start++;
    while (end > start && std::isspace((unsigned char) s[end - 1])) end--;

    return s.substr(start, end - start);
}

// Reads one line from the config file.
void ConfigManager::parseLine(const std::string& rawLine) {
    std::string line = trim(rawLine);

    // Ignore empty lines and comments.
    if (line.empty() || line[0] == '#') return;

    size_t eq = line.find('=');

    // Ignore invalid lines.
    if (eq == std::string::npos) return;

    std::string key = trim(line.substr(0, eq));
    std::string val = trim(line.substr(eq + 1));

    // Ignore lines with an empty key.
    if (key.empty()) return;

    // Store the key and value.
    // If the key already exists, its value is updated.
    values.set(key, val);
}

// Reads the config file again.
void ConfigManager::reload() {
    std::ifstream file(configPath);

    // Stop if the file cannot be opened.
    if (!file.is_open()) {
        throw ConfigException("ConfigManager::reload - could not open config file: " + configPath);
    }

    // Remove old values.
    values.clear();

    std::string line;

    // Read the file one line at a time.
    while (std::getline(file, line)) {
        parseLine(line);
    }
}

// Checks if a key exists.
bool ConfigManager::hasKey(const std::string& key) const {
    return values.exists(key);
}

// Returns the string value.
// Returns the default value if the key is not found.
std::string ConfigManager::getString(const std::string& key, const std::string& defaultVal) const {
    if (!values.exists(key)) return defaultVal;
    return values.get(key);
}

// Returns the integer value.
// Returns the default value if the key is missing or invalid.
int ConfigManager::getInt(const std::string& key, int defaultVal) const {
    if (!values.exists(key)) return defaultVal;

    try {
        return std::stoi(values.get(key));
    } catch (...) {
        return defaultVal;
    }
}

// Returns the boolean value.
// Returns the default value if the key is missing or invalid.
bool ConfigManager::getBool(const std::string& key, bool defaultVal) const {
    if (!values.exists(key)) return defaultVal;

    std::string v = values.get(key);

    // Convert the value to lowercase.
    for (auto& c : v) c = (char) std::tolower((unsigned char) c);

    if (v == "true" || v == "1" || v == "yes") return true;
    if (v == "false" || v == "0" || v == "no") return false;

    return defaultVal;
}

// Returns the value of a required string key.
// Throws an exception if the key is missing.
std::string ConfigManager::requireString(const std::string& key) const {
    if (!values.exists(key)) {
        throw ConfigException("ConfigManager::requireString - missing required key: " + key);
    }

    return values.get(key);
}

// Returns the value of a required integer key.
// Throws an exception if the key is missing or not a valid integer.
int ConfigManager::requireInt(const std::string& key) const {
    if (!values.exists(key)) {
        throw ConfigException("ConfigManager::requireInt - missing required key: " + key);
    }

    try {
        return std::stoi(values.get(key));
    } catch (...) {
        throw ConfigException("ConfigManager::requireInt - key '" + key + "' is not a valid integer");
    }
}