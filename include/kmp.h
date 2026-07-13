#pragma once
#include <string>
#include "Dynamicarray.h"

// KMP (Knuth-Morris-Pratt)
//
// This class is used to find a pattern inside a string.
// It is a general string searching algorithm and is not
// specific to HTML.
//
// It uses Project-01's DynamicArray instead of std::vector.
class KMP {
public:
    // Builds the LPS (Longest Prefix Suffix) array
    // used by the KMP algorithm.
    static DynamicArray<int> computeLPS(const std::string& pattern);

    // Finds all occurrences of the pattern in the text.
    // Returns the starting index of each match.
    static DynamicArray<int> search(const std::string& text, const std::string& pattern);

    // Finds the first occurrence of the pattern.
    // Returns -1 if the pattern is not found.
    static int searchFirst(const std::string& text, const std::string& pattern, int startPos = 0);
};