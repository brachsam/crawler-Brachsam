#pragma once
#include <string>
#include "Dynamicarray.h"
#include "kmp.h"

// HTMLParser
//
// This class extracts all href links from HTML.
// It uses the KMP algorithm to find <a and href= in the HTML.
//
// It only extracts links.
// It does not check or modify them.
class HTMLParser {
public:
    // Creates the HTMLParser object.
    HTMLParser();
    ~HTMLParser();

    // Extracts all href links from the HTML.
    // Returns the links in the same order as they appear.
    DynamicArray<std::string> extractLinks(const std::string& html) const;

private:
    // Checks if the found "<a" is really an anchor tag.
    bool isValidAnchorTagBoundary(const std::string& lowerHtml, int pos) const;

    // Extracts the href value from the HTML.
    std::string extractHrefValue(const std::string& html, int hrefPos) const;

    // Converts a string to lowercase.
    static std::string toLowerAscii(const std::string& s);
};