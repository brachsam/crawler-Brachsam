#pragma once
#include <string>

// Takes a raw token and returns a clean, lowercase version of it.
// It removes any leading or trailing punctuation like commas, dots, quotes etc.
// Example: "Hello," -> "hello"   |   "'world'" -> "world"
class Normalizer {
public:
    std::string normalize(const std::string& tok) const;
};
