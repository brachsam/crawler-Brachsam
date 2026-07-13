// RobotsHandler.h
// Checks whether a URL is allowed to be crawled based on robots.txt.

#ifndef ROBOTS_HANDLER_H
#define ROBOTS_HANDLER_H

#include<string>
using namespace std;

class RobotsHandler{
public:
    // Returns true if the url is allowed to be crawled.
    bool isAllowed(string url);

    //Extracts all disallowed paths from robots.txt.
    string extractDisallowedPaths(string robotsText);

    // Checks ifa  path matches any disallowed rule.
    bool pathIsDisallowed(string path, string disallowedPaths);

private:
    // Gets the host name from a URL.
    string extractHost(string url);

    // Gets the path from a URL.
    string extractPath(string url);

    // Downloads the robots.txt file for a host.
    string fetchRobotsTxt(string host);

    // Cache to avoid downloading the same robots.txt again.
    static const int MAX_CACHED_HOSTS = 100;
    string cachedHost[MAX_CACHED_HOSTS];
    string cachedRules[MAX_CACHED_HOSTS];
    int cachedCount = 0;

    // Returns cached rules for a host.
    string getCachedRules(string host);

    // Checks whether the host is already in the cache.
    bool isHostCached(string host);

    // Saves a host and its rules in the cache.
    void addToCache(string host, string rules);
};

#endif