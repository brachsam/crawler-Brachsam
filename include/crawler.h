#pragma once
#include <string>
#include <stdexcept>

#include "configmanager.h"
#include "logger.h"
#include "frontier.h"
#include "seenstore.h"
#include "pagestorage.h"
#include "pagefetcher.h"
#include "htmlParser.h"
#include "URLNormalizer.h"
#include "URLValidator.h"
#include "robotsHandler.h"
#include "crawlScheduler.h"
#include "common.h"
#include "URLDepth.h"

// Exception for Crawler setup errors.
class CrawlerException : public std::runtime_error {
public:
    explicit CrawlerException(const std::string& msg) : std::runtime_error(msg) {}
};

// Crawler
//
// This is the main class of the project.
// It connects all the other components and controls the crawling process.
class Crawler {
private:
    // Project components.
    // These are created in the same order as written here.
    ConfigManager   config;
    Logger          logger;
    Frontier        frontier;
    SeenStore       seenStore;
    PageStorage     pageStorage;
    PageFetcher     pageFetcher;
    HTMLParser      htmlParser;
    URLNormalizer   urlNormalizer;
    URLValidator    urlValidator;
    RobotsHandler   robotsHandler;
    CrawlScheduler  crawlScheduler;

    // Maximum crawl depth from config.txt.
    int maxDepth;

    // Number of pages crawled.
    int pagesCrawledCount;

    // Converts the log level string into LogLevel.
    static LogLevel parseLogLevel(const std::string& levelStr);

    // Processes one URL from the Frontier.
    void processOne(const URLDepth& item);

    // Checks and adds a discovered link to the Frontier.
    void handleDiscoveredLink(const std::string& rawLink,
                              const std::string& baseUrl,
                              int childDepth);

public:
    // Creates the Crawler object.
    // Reads the config file and starts from the seed URL.
    Crawler(const std::string& configPath, const std::string& seedUrl);
    ~Crawler();

    // Copying a Crawler object is not allowed.
    Crawler(const Crawler&) = delete;
    Crawler& operator=(const Crawler&) = delete;

    // Starts the crawling process.
    void run();

    // Returns the number of pages crawled.
    int pagesCrawled() const;
};