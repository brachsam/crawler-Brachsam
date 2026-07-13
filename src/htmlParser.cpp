#include "htmlParser.h"
#include <cctype>

// Creates the HTMLParser object.
HTMLParser::HTMLParser() {}

// Destroys the HTMLParser object.
HTMLParser::~HTMLParser() {}

// Converts a string to lowercase.
std::string HTMLParser::toLowerAscii(const std::string& s) {
    std::string result = s;

    for (size_t i = 0; i < result.size(); i++) {
        result[i] = (char) std::tolower((unsigned char) result[i]);
    }

    return result;
}

// Checks if "<a" is really an anchor tag.
bool HTMLParser::isValidAnchorTagBoundary(const std::string& lowerHtml, int pos) const {
    int checkIndex = pos + 2;

    // Return false if the tag is incomplete.
    if (checkIndex >= (int) lowerHtml.size()) return false;

    char c = lowerHtml[checkIndex];

    // Valid characters after "<a".
    return c == ' ' || c == '\t' || c == '\n' ||
           c == '\r' || c == '>' || c == '/';
}

// Extracts the href value from an anchor tag.
std::string HTMLParser::extractHrefValue(const std::string& html, int hrefPos) const {
    int valueStart = hrefPos + 5; // Skip "href="

    if (valueStart >= (int) html.size()) return "";

    char quoteChar = html[valueStart];

    // Handle quoted href values.
    if (quoteChar == '"' || quoteChar == '\'') {
        size_t closeQuote = html.find(quoteChar, valueStart + 1);

        // Return empty if the closing quote is missing.
        if (closeQuote == std::string::npos) return "";

        return html.substr(valueStart + 1, closeQuote - valueStart - 1);
    }

    // Handle unquoted href values.
    int end = valueStart;

    while (end < (int) html.size() &&
           html[end] != ' ' &&
           html[end] != '\t' &&
           html[end] != '\n' &&
           html[end] != '\r' &&
           html[end] != '>' &&
           !(html[end] == '/' &&
             end + 1 < (int) html.size() &&
             html[end + 1] == '>')) {
        end++;
    }

    return html.substr(valueStart, end - valueStart);
}

// Extracts all links from the HTML page.
DynamicArray<std::string> HTMLParser::extractLinks(const std::string& html) const {
    DynamicArray<std::string> links;

    // Convert HTML to lowercase for searching.
    std::string lowerHtml = toLowerAscii(html);

    // Find all "<a" tags.
    DynamicArray<int> anchorPositions = KMP::search(lowerHtml, "<a");

    for (int i = 0; i < anchorPositions.size(); i++) {
        int pos = anchorPositions.get(i);

        // Skip invalid anchor tags.
        if (!isValidAnchorTagBoundary(lowerHtml, pos))
            continue;

        // Find the end of the tag.
        size_t tagEnd = lowerHtml.find('>', pos);

        // Skip incomplete tags.
        if (tagEnd == std::string::npos)
            continue;

        std::string tagSlice = lowerHtml.substr(pos, tagEnd - pos + 1);

        // Find the href attribute.
        int hrefOffsetInSlice = KMP::searchFirst(tagSlice, "href=");

        // Skip if href is not present.
        if (hrefOffsetInSlice == -1)
            continue;

        int hrefPosInHtml = pos + hrefOffsetInSlice;

        // Extract the URL.
        std::string url = extractHrefValue(html, hrefPosInHtml);

        // Save the URL if it is not empty.
        if (!url.empty()) {
            links.append(url);
        }
    }

    return links;
}