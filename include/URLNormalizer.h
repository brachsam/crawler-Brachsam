// Rules applied as we decided in the design proposal:
// 1. Lowercase the scheme and host.
// 2. Convert relative URL's to absolute, using the page's base URL.
// 3. Remove trailing slash when it's redundant.
// 4. Remove empty query strings and fragments.

#ifndef URL_NORMALIZER_H
#define URL_NORMALIZER_H

#include <string>
using namespace std;

class URLNormalizer {
public:
    // url - it is the raw URL that we pulled out of the HTML
    // baseURL - the URL of the pages we found this href on
    string normalize (string url, string baseURL);

private:
// this is the helpers, each one does exactly one step at a time.
    // true if URL already starts with https:// or http://
    bool isAbsolute(string url);

    // resolves a relative url against base URL and returns an absolte one
    string resolveRelativeURL(string url, string baseURL);

    // lowercases just the scheme+host portion, leaves path/query untouched
    string lowercaseSchemeAndHost(string url);

    //cuts off everything from the first # onwards
    string removeFragment(string url);

    // removes the trailing slash if it is redundant
    string removeRedundantTrailingSlash(string url);

    // removes the trailing "?" with nothing after it
    string removeEmptyQuery(string url);

};

#endif // URL_NORMALIZER_H