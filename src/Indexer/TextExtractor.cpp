#include "Indexer/TextExtractor.h"
#include <cctype>

// Checks if the HTML at position i starts with a given tag name like "script" or "style".
// It handles both lowercase and uppercase tag names.
static bool matchTag(const std::string& html, int i, const char* tag) {
    int j = 0;
    while (tag[j] != '\0') {
        if (i >= static_cast<int>(html.size())) return false;
        if (std::tolower(static_cast<unsigned char>(html[i])) != tag[j]) return false;
        i++;
        j++;
    }
    // After the tag name there should be a space, '>', or '/' - not another letter
    if (i < static_cast<int>(html.size())) {
        char next = html[i];
        if (std::isalpha(static_cast<unsigned char>(next))) return false;
    }
    return true;
}

// Skips forward until we find the closing tag like </script> or </style>.
static int skipUntilClosingTag(const std::string& html, int i, const char* tag) {
    while (i < static_cast<int>(html.size())) {
        // Look for '</'
        if (html[i] == '<' && i + 1 < static_cast<int>(html.size()) && html[i + 1] == '/') {
            int j = i + 2;
            if (matchTag(html, j, tag)) {
                // Skip past the whole closing tag
                while (i < static_cast<int>(html.size()) && html[i] != '>') i++;
                return i + 1;
            }
        }
        i++;
    }
    return i;
}

std::string TextExtractor::extract(const std::string& html) const {
    std::string result;
    int i = 0;
    int len = static_cast<int>(html.size());

    while (i < len) {
        if (html[i] == '<') {
            // We just hit an opening angle bracket
            int j = i + 1;

            // Skip any whitespace right after '<'
            while (j < len && html[j] == ' ') j++;

            // If this is a <script> block, skip everything until </script>
            if (matchTag(html, j, "script")) {
                i = skipUntilClosingTag(html, j, "script");
                continue;
            }

            // If this is a <style> block, skip everything until </style>
            if (matchTag(html, j, "style")) {
                i = skipUntilClosingTag(html, j, "style");
                continue;
            }

            // For any other tag, just skip past the closing '>'
            while (i < len && html[i] != '>') i++;
            i++; // move past '>'

            // Add a space so words from different tags don't get glued together
            if (!result.empty() && result.back() != ' ') {
                result += ' ';
            }

        } else {
            // Normal character - just copy it to result
            result += html[i];
            i++;
        }
    }

    return result;
}
