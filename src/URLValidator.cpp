// URLValidator.cpp
//
// Wrote each rejection reason as its own function instead of cramming
// everything into one if-chain, mainly because when this fails a test
// later I want to know exactly which rule caught it, not just "it
// returned false somewhere in there."

#include "URLValidator.h"

bool URLValidator::isValid(string url) {
    // empty link can't be a real link that's why reject right away
    if (url.empty()) return false;

    // rejects javascript links
    if (isJavascriptLink(url)) return false;
    // rejects mailto links
    if (isMailtoLink(url))     return false;
    // reject bare "#something" links
    if (isBareFragment(url))   return false;
    // reject anything that is not http/https
    if (!isHttpOrHttps(url))   return false;
    // if it survived all of that, it's fine
    return true;
}

// returns the lowercase version

string URLValidator::toLower(string s) {
    for (size_t i = 0; i < s.size(); i++) {
        if (s[i] >= 'A' && s[i] <= 'Z') {
            s[i] = s[i] - 'A' + 'a';
        }
    }
    return s;
}

// javascript is 11 characters long, so the url must be atleast that long to match.
// return lower.substr checks if the first 11 characters are exactly javascript.

bool URLValidator::isJavascriptLink(string url) {
    string lower = toLower(url);
    // checking the first 11 chars covers "javascript:"
    if (lower.size() < 11) return false;
    return lower.substr(0, 11) == "javascript:";
}

// check if the first 7 characters are exactly "malito:"
bool URLValidator::isMailtoLink(string url) {
    string lower = toLower(url);
    if (lower.size() < 7) return false;
    return lower.substr(0, 7) == "mailto:";
}

//
// A "bare fragment" is something like "#introduction" with nothing
// before the #. Note: by the time a URL gets here it's usually already
// gone through URLNormalizer, which strips fragments off real URLs
// (like "/about#team" becomes "/about"). This check mainly catches the
// case where the ENTIRE href was just "#something" with no path at all —
// URLNormalizer wouldn't have anything left to resolve there.

bool URLValidator::isBareFragment(string url) {
    return !url.empty() && url[0] == '#';
}

// ---------------------------------------------------------------------

bool URLValidator::isHttpOrHttps(string url) {
    string lower = toLower(url);
    if (lower.substr(0, 7) == "http://") return true;
    if (lower.size() >= 8 && lower.substr(0, 8) == "https://") return true;
    return false;
}