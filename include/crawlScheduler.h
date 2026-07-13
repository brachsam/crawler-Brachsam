#pragma once
#include <chrono>
#include "logger.h"

// ─────────────────────────────────────────────────────────────────────────────
// CrawlScheduler
//
// Single Responsibility: request-timing regulation only. It does NOT decide
// what to fetch, does NOT check robots.txt, does NOT fetch pages — it only
// enforces a minimum gap between successive requests, so the crawler behaves
// politely toward the servers it visits.
//
// Per the finalized Main Loop, the Crawler calls waitBeforeNextRequest()
// once per URL, right before every fetch attempt (this includes fetching
// robots.txt itself, not just the page).
//
// No custom exception type here (unlike Frontier/SeenStore/PageStorage):
// this class wraps no fallible external library — the std::chrono/std::this_
// thread calls used internally do not throw under normal conditions, so
// there is nothing meaningful to translate into a wrapper-specific exception.
// ─────────────────────────────────────────────────────────────────────────────
class CrawlScheduler {
private:
    std::chrono::milliseconds delay;
    std::chrono::steady_clock::time_point lastRequestTime;
    bool hasRequestedBefore;

    // Optional, unlike the mandatory-by-reference Logger used elsewhere in
    // the crawler. Made a nullable pointer (default nullptr) specifically so
    // CrawlScheduler can be unit-tested in complete isolation, without
    // needing to construct a real Logger just to exercise timing behavior.
    Logger* logger;

public:
    // delayMs: minimum milliseconds required between the start of one
    // request and the start of the next. Typically read from ConfigManager
    // (e.g. config.getInt("crawl_delay_ms", 500)) by the Crawler, not
    // hardcoded here — CrawlScheduler itself has no knowledge of ConfigManager.
    explicit CrawlScheduler(int delayMs, Logger* logger = nullptr);
    ~CrawlScheduler();

    // One scheduler drives the timing for the entire crawl; copying would
    // create two independent "last request time" clocks, defeating the
    // whole purpose of politeness enforcement.
    CrawlScheduler(const CrawlScheduler&) = delete;
    CrawlScheduler& operator=(const CrawlScheduler&) = delete;

    // Blocks the calling thread until `delay` milliseconds have passed since
    // the previous call to this method. The very first call never blocks —
    // there is no prior request to space out from.
    void waitBeforeNextRequest();

    int getDelayMs() const;
};