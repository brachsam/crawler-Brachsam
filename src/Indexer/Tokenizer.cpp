#include "Indexer/Tokenizer.h"

// Returns true if the character is a separator we should split on.
static bool isSep(char c) {
    return c == ' ' || c == '\t' || c == '\n' || c == '\r';
}

DynamicArray<std::string> Tokenizer::tokenize(const std::string& text) const {
    DynamicArray<std::string> tokens;

    std::string word;

    for (int i = 0; i < static_cast<int>(text.size()); i++) {
        char c = text[i];

        if (isSep(c)) {
            // We hit a space or newline, so save the word we built so far
            if (!word.empty()) {
                tokens.append(word);
                word.clear();
            }
        } else {
            // Keep building the current word
            word += c;
        }
    }

    // Don't forget the last word if the text doesn't end with a space
    if (!word.empty()) {
        tokens.append(word);
    }

    return tokens;
}
