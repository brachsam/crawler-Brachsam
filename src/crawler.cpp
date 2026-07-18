#include "crawler.h"

#include <cctype>

// Converts the log level string into LogLevel.
LogLevel Crawler::parseLogLevel(const std::string& levelStr) {
    std::string lower = levelStr;

    // Convert to lowercase.
    for (size_t i = 0; i < lower.size(); i++) {
        lower[i] = (char) std::tolower((unsigned char) lower[i]);
    }

    if (lower == "debug") return LogLevel::LDEBUG;
    if (lower == "warn")  return LogLevel::LWARN;
    if (lower == "error") return LogLevel::LERROR;

    // Default log level.
    return LogLevel::LINFO;
}

// Creates the Crawler object.
Crawler::Crawler(const std::string& configPath, const std::string& seedUrl)
    : config(configPath),
      logger(/*consoleOutput=*/true,
             config.getString("log_file", ""),
             parseLogLevel(config.getString("log_level", "info"))),
      frontier(),
      seenStore(),
      pageStorage(config.getString("db_path", "data/crawler.db")),
      pageFetcher(config.getString("browser_adapter_url", "http://localhost:3000"),
                  config.getInt("fetch_timeout_ms", 30000),
                  &logger),
      htmlParser(),
      urlNormalizer(),
      urlValidator(),
      robotsHandler(),
      crawlScheduler(config.getInt("crawl_delay_ms", 500), &logger),
      maxDepth(config.getInt("max_depth", 3)),
      pagesCrawledCount(0)
{
    // Check if the seed URL is empty.
    if (seedUrl.empty()) {
        throw CrawlerException("Crawler: seed URL is empty");
    }

    logger.info("Crawler: initialized (maxDepth=" + std::to_string(maxDepth) +
                ", crawlDelayMs=" + std::to_string(crawlScheduler.getDelayMs()) + ")");

    // Recover any URLs that crashed during a previous run.
    try {
        pageStorage.resetInProgressUrls();
    } catch (const std::exception& e) {
        logger.error("Crawler: failed to reset in-progress URLs: " + std::string(e.what()));
    }

    // Populate SeenStore with all previously seen/crawled URLs from database.
    try {
        DynamicArray<std::string> allSeen = pageStorage.getAllSeenUrls();
        for (int i = 0; i < allSeen.size(); i++) {
            seenStore.markSeen(allSeen.get(i));
        }
    } catch (const std::exception& e) {
        logger.error("Crawler: failed to load seen URLs: " + std::string(e.what()));
    }

    // Load any pending URLs from database to resume crawling (filtered by domain).
    try {
        // Extract host name from seedUrl to filter pending URLs.
        std::string hostFilter = "";
        size_t schemeEnd = 0;
        if (seedUrl.substr(0, 8) == "https://") schemeEnd = 8;
        else if (seedUrl.substr(0, 7) == "http://") schemeEnd = 7;

        if (schemeEnd > 0) {
            size_t hostEnd = seedUrl.find('/', schemeEnd);
            if (hostEnd == std::string::npos) {
                hostFilter = seedUrl.substr(schemeEnd);
            } else {
                hostFilter = seedUrl.substr(schemeEnd, hostEnd - schemeEnd);
            }
        }

        DynamicArray<URLDepth> pending = pageStorage.loadPendingUrls(5000, hostFilter);
        if (pending.size() > 0) {
            for (int i = 0; i < pending.size(); i++) {
                frontier.enqueue(pending.get(i));
            }
            logger.info("Crawler: resumed crawling with " + std::to_string(pending.size()) + 
                        " pending URLs for domain " + hostFilter + " from database");
        } else {
            // No pending URLs found, check the seed URL.
            if (pageStorage.exists(seedUrl)) {
                std::cout << "already crawled" << std::endl;
                logger.info("already crawled");
            } else {
                // Save seed URL as pending in database.
                pageStorage.savePendingUrl(seedUrl, 0, "");

                // Add the first URL to the Frontier.
                URLDepth seed;
                seed.url = seedUrl;
                seed.depth = 0;

                frontier.enqueue(seed);
                seenStore.markSeen(seedUrl);

                logger.info("Crawler: seeded with " + seedUrl);
            }
        }
    } catch (const std::exception& e) {
        logger.error("Crawler: initialization database query failed: " + std::string(e.what()));
        // Fallback: Seed with seedUrl if database fails to query.
        URLDepth seed;
        seed.url = seedUrl;
        seed.depth = 0;
        frontier.enqueue(seed);
        seenStore.markSeen(seedUrl);
    }
}

// Destroys the Crawler object.
Crawler::~Crawler() {
    logger.info("Crawler: shutting down. Total pages crawled: " +
                std::to_string(pagesCrawledCount));
}

// Starts the crawling process.
void Crawler::run() {
    logger.info("Crawler: starting crawl loop");

    while (!frontier.isEmpty()) {
        URLDepth item;

        try {
            item = frontier.dequeue();
        } catch (const FrontierEmptyException& e) {
            // Stop if the Frontier becomes empty.
            logger.warn(std::string("Crawler: ") + e.what());
            break;
        }

        // Skip pages beyond the maximum depth.
        if (item.depth > maxDepth) {
            logger.debug("Crawler: skipping (past depth limit) " + item.url);
            continue;
        }

        processOne(item);
    }

    logger.info("Crawler: crawl finished. Total pages crawled: " +
                std::to_string(pagesCrawledCount));
}

// Processes one page.
void Crawler::processOne(const URLDepth& item) {

    // Wait before sending the next request.
    crawlScheduler.waitBeforeNextRequest();

    logger.info("Crawler: processing " + item.url +
                " (depth " + std::to_string(item.depth) + ")");

    // Mark the URL as IN_PROGRESS (1) in database.
    try {
        pageStorage.updatePendingUrlStatus(item.url, 1);
    } catch (const std::exception& e) {
        logger.error("Crawler: failed to update status to IN_PROGRESS for " + item.url + ": " + e.what());
    }

    // Check robots.txt.
    bool allowed = true;

    try {
        allowed = robotsHandler.isAllowed(item.url);
    } catch (const std::exception& e) {

        // Allow the page if the robots check fails.
        logger.warn("Crawler: robots.txt check failed for " + item.url +
                    ": " + e.what() + " — allowing by default");

        allowed = true;
    }

    // Skip blocked pages.
    if (!allowed) {
        logger.info("Crawler: blocked by robots.txt: " + item.url);
        try {
            pageStorage.updatePendingUrlStatus(item.url, 2); // Mark completed/skipped
        } catch (...) {}
        return;
    }

    // Fetch the page.
    std::string html;

    try {
        html = pageFetcher.fetch(item.url);
    } catch (const PageFetcherException& e) {
        logger.error("Crawler: fetch failed for " + item.url + ": " + e.what());
        try {
            pageStorage.updatePendingUrlStatus(item.url, 3); // Mark FAILED (3)
        } catch (...) {}
        return;
    }

    // Save the page.
    Page page;
    page.url = item.url;
    page.depth = item.depth;
    page.html = html;

    try {
        pageStorage.savePage(page);
        pageStorage.updatePendingUrlStatus(item.url, 2); // Mark COMPLETED (2)
    } catch (const PageStorageException& e) {

        // Continue even if saving fails.
        logger.error("Crawler: savePage failed for " + item.url + ": " + e.what());
    }

    pagesCrawledCount++;

    // Extract links from the page.
    DynamicArray<std::string> rawLinks;

    try {
        rawLinks = htmlParser.extractLinks(html);
    } catch (const std::exception& e) {
        logger.error("Crawler: link extraction failed for " + item.url +
                    ": " + e.what());
        return;
    }

    // Process every extracted link.
    int childDepth = item.depth + 1;

    for (int i = 0; i < rawLinks.size(); i++) {
        handleDiscoveredLink(rawLinks.get(i), item.url, childDepth);
    }
}

// Processes one discovered link.
void Crawler::handleDiscoveredLink(const std::string& rawLink,
                                   const std::string& baseUrl,
                                   int childDepth) {

    // Skip links beyond the maximum depth.
    if (childDepth > maxDepth) {
        return;
    }

    // Convert the link into a complete URL.
    std::string normalized = urlNormalizer.normalize(rawLink, baseUrl);

    // Skip invalid URLs.
    if (!urlValidator.isValid(normalized)) {
        return;
    }

    // Skip URLs that were already seen.
    if (seenStore.isSeen(normalized)) {
        return;
    }

    // Mark the URL as seen.
    seenStore.markSeen(normalized);

    // Save the discovered URL as pending in database.
    try {
        pageStorage.savePendingUrl(normalized, childDepth, baseUrl);
    } catch (const std::exception& e) {
        logger.error("Crawler: failed to save pending URL " + normalized + ": " + e.what());
    }

    // Add the URL to the Frontier.
    URLDepth child;
    child.url = normalized;
    child.depth = childDepth;

    frontier.enqueue(child);

    logger.debug("Crawler: enqueued " + normalized +
                " (depth " + std::to_string(childDepth) + ")");
}

// Returns the number of pages crawled.
int Crawler::pagesCrawled() const {
    return pagesCrawledCount;
}