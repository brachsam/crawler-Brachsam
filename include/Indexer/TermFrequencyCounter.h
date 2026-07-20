#pragma once
#include <string>
#include "DynamicArray/Dynamicarray.h"
#include "HashMap/hmap.h"

// Counts how many times each word appears in a list of tokens.
// The result is a map of word -> count for a single document.
// Example: ["cat", "dog", "cat"] -> {"cat": 2, "dog": 1}
class TermFrequencyCounter {
public:
    HashMap<std::string, int> count(const DynamicArray<std::string>& tokens) const;
};
