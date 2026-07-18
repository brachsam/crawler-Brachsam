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
    int idx = hrefPos + 4; // Skip "href"
    while (idx < (int)html.size() && std::isspace((unsigned char)html[idx])) {
        idx++;
    }
    if (idx >= (int)html.size() || html[idx] != '=') {
        return "";
    }
    idx++; // Skip '='
    while (idx < (int)html.size() && std::isspace((unsigned char)html[idx])) {
        idx++;
    }
    if (idx >= (int)html.size()) {
        return "";
    }

    int valueStart = idx;
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
        int hrefOffsetInSlice = KMP::searchFirst(tagSlice, "href");

        while (hrefOffsetInSlice != -1) {
            // Verify that the "href" we found is not part of another word (e.g. "sharehref").
            // It should be preceded by a whitespace character or the start of the tag after "<a".
            bool validPrefix = false;
            if (hrefOffsetInSlice > 0) {
                char prev = tagSlice[hrefOffsetInSlice - 1];
                if (std::isspace((unsigned char)prev)) {
                    validPrefix = true;
                }
            }

            // Check if it is followed by '=' (possibly with spaces in between).
            bool hasEquals = false;
            int idx = hrefOffsetInSlice + 4; // Skip "href"
            while (idx < (int)tagSlice.size() && std::isspace((unsigned char)tagSlice[idx])) {
                idx++;
            }
            if (idx < (int)tagSlice.size() && tagSlice[idx] == '=') {
                hasEquals = true;
            }

            if (validPrefix && hasEquals) {
                break;
            }

            // Search for the next "href" in tagSlice
            hrefOffsetInSlice = KMP::searchFirst(tagSlice, "href", hrefOffsetInSlice + 4);
        }

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