#include "Indexer/Normalizer.h"
#include <cctype>

// Returns true if a character is punctuation we want to strip from a word.
static bool isPunct(char c) {
    return c == '.' || c == ',' || c == '!' || c == '?' || c == ';' ||
           c == ':' || c == '"' || c == '\'' || c == '(' || c == ')' ||
           c == '[' || c == ']' || c == '{' || c == '}' || c == '-' ||
           c == '_' || c == '/' || c == '\\' || c == '@' || c == '#' ||
           c == '$' || c == '%' || c == '^' || c == '&' || c == '*' ||
           c == '+' || c == '=' || c == '<' || c == '>' || c == '`' ||
           c == '~' || c == '|';
}

std::string Normalizer::normalize(const std::string& tok) const {
    std::string result = tok;

    // Convert every character to lowercase
    for (char& c : result) {
        c = static_cast<char>(std::tolower(static_cast<unsigned char>(c)));
    }

    // Trim punctuation from the left side
    int start = 0;
    while (start < static_cast<int>(result.size()) && isPunct(result[start])) {
        start++;
    }

    // Trim punctuation from the right side
    int end = static_cast<int>(result.size()) - 1;
    while (end >= start && isPunct(result[end])) {
        end--;
    }

    // Return the cleaned-up word
    if (start > end) return "";
    return result.substr(start, end - start + 1);
}
