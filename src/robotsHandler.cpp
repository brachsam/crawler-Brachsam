// RobotsHandler.cpp
//
// Uses a small cache to avoid fetching the same robots.txt repeatedly.
// fetchRobotsTxt() is currently a stub and will later use the same
// network layer as PageFetcher.

#include "RobotsHandler.h"

bool RobotsHandler::isAllowed(string url) {
    string host = extractHost(url);
    string path = extractPath(url);

    if (host.empty()) {
        // Couldn't extract a valid host,so nothing to check.
        return true;
    }

    string rules = getCachedRules(host);

    if (!isHostCached(host)) {
        // Fetch robots.txt for a new host, since first time visiting this website.
        string robotsText = fetchRobotsTxt(host);

        if (robotsText.empty()) {
            // If robots.txt is missing, allow crawling.
            addToCache(host, "");
            return true;
        }

        rules = extractDisallowedPaths(robotsText);
        addToCache(host, rules);
    }

    return !pathIsDisallowed(path, rules);
}

// extracts only the host name from the given URL
// example:
// https://example.com/page  ->  example.com

string RobotsHandler::extractHost(string url) {
    size_t schemeEnd = 0;

    if (url.substr(0, 8) == "https://") schemeEnd = 8;
    else if (url.substr(0, 7) == "http://") schemeEnd = 7;
    else return "";   // Unsupported URL.

    size_t hostEnd = url.find('/', schemeEnd);

    if (hostEnd == string::npos)
        return url.substr(schemeEnd);

    return url.substr(schemeEnd, hostEnd - schemeEnd);
}

// extracts only the path from the URL
// example:
// https://example.com/about  ->  /about

string RobotsHandler::extractPath(string url) {
    size_t schemeEnd = 0;

    if (url.substr(0, 8) == "https://") schemeEnd = 8;
    else if (url.substr(0, 7) == "http://") schemeEnd = 7;
    else return "/";

    size_t hostEnd = url.find('/', schemeEnd);

    if (hostEnd == string::npos)
        return "/";   // No path found, use root.

    return url.substr(hostEnd);
}

// Placeholder implementation.
// Will later fetch https://<host>/robots.txt.

string RobotsHandler::fetchRobotsTxt(string host) {
    return "";
}

// Reads robots.txt and extracts all so the crawler does not visit the page
// "Disallow" rules under "User-agent: *".

string RobotsHandler::extractDisallowedPaths(string robotsText) {
    string result = "";
    bool insideWildcardBlock = false;

    size_t pos = 0;

    while (pos < robotsText.size()) {
        size_t lineEnd = robotsText.find('\n', pos);

        if (lineEnd == string::npos)
            lineEnd = robotsText.size();

        string line = robotsText.substr(pos, lineEnd - pos);
        pos = lineEnd + 1;

        // Remove leading spaces.
        size_t start = 0;

        while (start < line.size() && line[start] == ' ')
            start++;

        line = line.substr(start);

        if (line.substr(0, 11) == "User-agent:") {
            string agent = line.substr(11);

            // remove the space after "user-agents:"
            if (!agent.empty() && agent[0] == ' ')
                agent = agent.substr(1);

            insideWildcardBlock = (agent == "*"); // only process rules for all crawlers
            continue;
        }

        if (insideWildcardBlock && line.substr(0, 9) == "Disallow:") {
            string disallowedPath = line.substr(9);

            // remove the space after "Disallow:"
            if (!disallowedPath.empty() && disallowedPath[0] == ' ')
                disallowedPath = disallowedPath.substr(1);

            if (!disallowedPath.empty())
                //saves the disallowed path
                result += disallowedPath + "\n";
        }
    }

    return result;
}

// checks whether the path matches
// any disallowed rule

bool RobotsHandler::pathIsDisallowed(string path, string disallowedPaths) {
    if (disallowedPaths.empty())
        return false;

    size_t pos = 0;

    while (pos < disallowedPaths.size()) {
        size_t lineEnd = disallowedPaths.find('\n', pos);

        if (lineEnd == string::npos)
            lineEnd = disallowedPaths.size();

        string rule = disallowedPaths.substr(pos, lineEnd - pos);
        pos = lineEnd + 1;

        if (rule.empty())
            continue;

        // if the path starts with a blocked rule,
        // crawling is not allowed
        if (path.substr(0, rule.size()) == rule)
            return true;
    }

    return false;
}

// returns cached rules for the given host

string RobotsHandler::getCachedRules(string host) {
    for (int i = 0; i < cachedCount; i++) {
        if (cachedHost[i] == host)
            return cachedRules[i];
    }

    return "";
}

// ---------------------------------------------------------------------

bool RobotsHandler::isHostCached(string host) {
    for (int i = 0; i < cachedCount; i++) {
        if (cachedHost[i] == host)
            return true;
    }

    return false;
}

// ---------------------------------------------------------------------

void RobotsHandler::addToCache(string host, string rules) {
    // Ignore if the cache is full.
    if (cachedCount >= MAX_CACHED_HOSTS)
        return;

    cachedHost[cachedCount] = host;
    cachedRules[cachedCount] = rules;
    cachedCount++;
}