#pragma once
#include <string>
#include "DynamicArray/Dynamicarray.h"

// Splits a piece of text into individual words (tokens).
// It breaks on whitespace and common separators like tabs and newlines.
// Example: "Hello world" -> ["Hello", "world"]
class Tokenizer {
public:
    DynamicArray<std::string> tokenize(const std::string& text) const;
};
