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

    // Add the first URL to the Frontier.
    URLDepth seed;
    seed.url = seedUrl;
    seed.depth = 0;

    frontier.enqueue(seed);
    seenStore.markSeen(seedUrl);

    logger.info("Crawler: seeded with " + seedUrl);
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
        return;
    }

    // Fetch the page.
    std::string html;

    try {
        html = pageFetcher.fetch(item.url);
    } catch (const PageFetcherException& e) {
        logger.error("Crawler: fetch failed for " + item.url + ": " + e.what());
        return;
    }

    // Save the page.
    Page page;
    page.url = item.url;
    page.depth = item.depth;
    page.html = html;

    try {
        pageStorage.savePage(page);
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