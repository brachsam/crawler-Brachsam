// URLValidator.h
// This one's simple on purpose. After URLNormalizer has already cleaned a URL up, URLValidator just looks at it and says yes or no - is this actually something we shpold try to crawl?
// Things we reject:
//   - javascript: links (not a page, it's a script action)
//   - mailto: links (opens an email client, not a webpage)
//   - bare fragment links like "#something" (just a scroll position)
//   - anything that isn't http:// or https:// after normalization

#ifndef URL_VALIDATOR_H
#define URL_VALIDATOR_H
 
#include <string>
using namespace std;
 
class URLValidator {
public:
    bool isValid(string url);
 
private:
    // each check is its own little function so isValid() reads like a
    // checklist instead of one long if statement
    bool isJavascriptLink(string url);
    bool isMailtoLink(string url);
    bool isBareFragment(string url);
    bool isHttpOrHttps(string url);
 
    // lowercases a string so we're not tripped up by JavaScript:void(0)
    // or MAILTO:someone@site.com
    string toLower(string s);
};
 
#endif