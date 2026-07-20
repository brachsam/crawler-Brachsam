#pragma once
#include <string>

// Strips all HTML tags from a page and returns only the readable text.
// It also skips the content inside <script> and <style> blocks entirely
// since those contain code, not readable words.
// Example: "<p>Hello <b>world</b></p>" -> "Hello world"
class TextExtractor {
public:
    std::string extract(const std::string& html) const;
};
