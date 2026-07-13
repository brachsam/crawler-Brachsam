#include "kmp.h"

// Builds the LPS array for the pattern.
DynamicArray<int> KMP::computeLPS(const std::string& pattern) {
    int m = (int) pattern.size();
    DynamicArray<int> lps;

    // Fill the array with zeros.
    for (int i = 0; i < m; i++) lps.append(0);

    // Return if the pattern is empty.
    if (m == 0) return lps;

    int len = 0; // Length of the current matching prefix.
    lps.get(0) = 0;
    int i = 1;

    while (i < m) {
        if (pattern[i] == pattern[len]) {
            len++;
            lps.get(i) = len;
            i++;
        } else if (len != 0) {
            len = lps.get(len - 1);
        } else {
            lps.get(i) = 0;
            i++;
        }
    }

    return lps;
}

// Finds all matches of the pattern in the text.
DynamicArray<int> KMP::search(const std::string& text, const std::string& pattern) {
    DynamicArray<int> matches;

    int n = (int) text.size();
    int m = (int) pattern.size();

    // Return if the pattern is empty or longer than the text.
    if (m == 0 || n < m) return matches;

    DynamicArray<int> lps = computeLPS(pattern);

    int i = 0; // Current position in the text.
    int j = 0; // Current position in the pattern.

    while (i < n) {
        if (text[i] == pattern[j]) {
            i++;
            j++;
        }

        // Pattern found.
        if (j == m) {
            matches.append(i - j);
            j = lps.get(j - 1);
        }
        else if (i < n && text[i] != pattern[j]) {
            if (j != 0)
                j = lps.get(j - 1);
            else
                i++;
        }
    }

    return matches;
}

// Finds the first match of the pattern.
int KMP::searchFirst(const std::string& text, const std::string& pattern, int startPos) {
    int n = (int) text.size();
    int m = (int) pattern.size();

    // Return if the input is invalid.
    if (m == 0 || startPos < 0 || startPos >= n) return -1;

    DynamicArray<int> lps = computeLPS(pattern);

    int i = startPos;
    int j = 0;

    while (i < n) {
        if (text[i] == pattern[j]) {
            i++;
            j++;
        }

        // First match found.
        if (j == m) {
            return i - j;
        }
        else if (i < n && text[i] != pattern[j]) {
            if (j != 0)
                j = lps.get(j - 1);
            else
                i++;
        }
    }

    // Pattern not found.
    return -1;
}