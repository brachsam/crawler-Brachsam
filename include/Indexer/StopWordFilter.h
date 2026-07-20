#pragma once
#include <string>
#include "HashMap/hmap.h"

// Checks if a word is a stop word (like "the", "is", "and").
// Stop words are very common words that add no value to search results.
// We load a fixed list of them on startup and use a HashMap for fast lookup.
class StopWordFilter {
public:
    StopWordFilter();
    bool isStopWord(const std::string& word) const;

private:
    HashMap<std::string, bool> words;
    void load();
};
