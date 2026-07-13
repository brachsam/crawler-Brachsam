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

string URLNormalizer::resolveRelativeURL(string url, string baseURL) {
    // handling 3 kinds of relative urls here:
    // /about, ../contact,
    string scheme, rest;

    if (baseURL.substr(0, 8) == "https://") {
        scheme = "https://";
        rest = baseURL.substr(8);
    }
    else if (baseURL.substr(0, 7) == "http://") {
        scheme = "http://";
        rest = baseURL.substr(7);
    }
    else {
        return url; // baseURL is broken, can;t really do anything smart here
    }

    size_t slashPos = rest.find('/');
    string host = (slashPos == string::npos) ? rest : rest.substr(0,slashPos);
    string path = (slashPos == string::npos) ? "" : rest.substr(slashPos);

    if (!url.empty() && url [0]=='/'){
        return scheme + host + url;
    }

    if (url.substr(0,3)=="../"){
        size_t lastSlash = path.rfind('/');
        string parentPath = (lastSlash == string::npos) ? "" : path.substr (0, lastSlash);

        // one more level up because ../ means "go up"
        size_t secondLastSlash = parentPath.find_last_of('/');
        string grandParentPath = (secondLastSlash == string::npos)? "" : parentPath.substr(0, secondLastSlash);

        string remainder = url.substr(3);
        return scheme + host + grandParentPath + "/" + remainder;
    }

    // plain relative — sits next to whatever page we're currently on
    size_t lastSlash = path.find_last_of('/');
    string currentDir = (lastSlash == string::npos) ? "" : path.substr(0, lastSlash);
    return scheme + host + currentDir + "/" + url;
}

string URLNormalizer::removeFragment(string url){
    size_t hashPos = url.find('#');
    if (hashPos == string::npos) return url; //no # anywhere, nothing to do
    return url.substr(0, hashPos);
}

string URLNormalizer::removeEmptyQuery(string url){
    //only care if the ? is literally the last char
    if (!url.empty() && url[url.size() - 1] == '?'){
        return url.substr(0, url.size()-1);
    }
    return url;
}

string URLNormalizer::lowercaseSchemeAndHost(string url) {
    if (url.size() < 8) {
        return url;
    }

    size_t schemeEnd = string::npos;
    if (url.substr(0, 7) == "http://") {
        schemeEnd = 7;
    } else if (url.substr(0, 8) == "https://") {
        schemeEnd = 8;
    }

    if (schemeEnd == string::npos) {
        return url;
    }

    size_t hostEnd = url.find('/', schemeEnd);
    if (hostEnd == string::npos) {
        hostEnd = url.size();
    }

    for (size_t i = schemeEnd; i < hostEnd; ++i) {
        if (url[i] >= 'A' && url[i] <= 'Z') {
            url[i] = static_cast<char>(url[i] - 'A' + 'a');
        }
    }

    return url;
}

string URLNormalizer::removeRedundantTrailingSlash(string url){
    if (url.empty()) return url;
    if (url[url.size() -1]!='/') return url; //no trailing slash found.

    // Don't remove the trailing slash from the root URL.
    // In "https://example.com/", the slash represents the website's root,
    // not an unnecessary trailing slash.

    size_t schemeEnd =0;
    if (url.substr(0,8)=="https://") schemeEnd =8;
    else if (url.substr(0,7) == "http://") schemeEnd =7;

    size_t firstSlashAfterHost = url.find('/', schemeEnd);

    if (firstSlashAfterHost = url.size() - 1){
        return url; //this slash is the root path itself, leave it alone
    }
    return url.substr(0, url.size() -1);
}