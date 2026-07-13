#include "CrawlScheduler.h"
#include <thread>

CrawlScheduler::CrawlScheduler(int delayMs, Logger* logger)
    : delay(std::chrono::milliseconds(delayMs)),
      hasRequestedBefore(false),
      logger(logger) {}

CrawlScheduler::~CrawlScheduler() {}

void CrawlScheduler::waitBeforeNextRequest() {
    auto now = std::chrono::steady_clock::now();

    if (hasRequestedBefore) {
        auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(now - lastRequestTime);
        if (elapsed < delay) {
            auto remaining = delay - elapsed;
            if (logger) {
                logger->debug("CrawlScheduler: waiting " + std::to_string(remaining.count()) + "ms before next request");
            }
            std::this_thread::sleep_for(remaining);
        }
    }

    // Record the time as of right after waiting, not before — this is the
    // moment the next request is actually allowed to begin.
    lastRequestTime = std::chrono::steady_clock::now();
    hasRequestedBefore = true;
}

int CrawlScheduler::getDelayMs() const {
    return (int) delay.count();
}