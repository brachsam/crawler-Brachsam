// Implementation notes:
// - Each normalization rule is implemented as a separate helper.
// - normalize() applies the rules in order for clarity and maintainability.
// - Relative URLs are resolved first, followed by normalization and cleanup.

#include "URLNormalizer.h"

string URLNormalizer::normalize(string url, string baseURL){
    if (!isAbsolute(url)) {
        url = resolveRelativeURL(url, baseURL);
    }

    // step2: lowercase scheme and host, path can be case-sensitive on real servers.
    url = lowercaseSchemeAndHost(url);

    // step3: remove fragment
    url = removeFragment(url);

    // step4: remove redundant trailing slash
    url = removeRedundantTrailingSlash(url);

    // step5: remove empty query string that's just "?" with nothing after it
    url = removeEmptyQuery(url);

    return url;
}

bool URLNormalizer::isAbsolute(string url) {
    // gotta lowercase before comparing or "HTTPS://" slips through as "not absolute"
    string prefix = url.substr(0, url.size()<8 ? url.size() : 8);
    for (size_t i = 0; i< prefix.size(); i++){
        if (prefix[i]>='A' && prefix[i]<='Z'){
            prefix[i] = prefix[i] - 'A' + 'a';
        }
    }
    if (prefix.substr(0, 7) == "http://") return true;
    if (prefix == "https://") return true;

    return false;
    
}

// handling 3 kinds of relative urls here:
// /about, ../contact
