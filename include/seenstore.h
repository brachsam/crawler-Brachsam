#pragma once
#include <stdexcept>
#include <string>

// Project-01 HashMap
#include "hmap.h"

// Exception thrown for SeenStore related errors.
class SeenStoreException : public std::runtime_error {
public:
    explicit SeenStoreException(const std::string& msg)
        : std::runtime_error(msg) {}
};

// Stores all visited URLs during crawling.
// Internally uses HashMap<string, bool>.
class SeenStore {
private:
    // Stores URL as key and a dummy bool value.
    HashMap<std::string, bool> seen;

public:
    SeenStore();
    ~SeenStore();

    // Prevent copying of SeenStore.
    SeenStore(const SeenStore&) = delete;
    SeenStore& operator=(const SeenStore&) = delete;

    // Marks a URL as visited.
    void markSeen(const std::string& url);

    // Returns true if the URL has already been visited.
    bool isSeen(const std::string& url) const;

    // Returns the number of visited URLs.
    int size() const;
};