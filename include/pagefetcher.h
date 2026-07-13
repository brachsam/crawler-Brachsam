#pragma once

#include <string>
#include <stdexcept>

#include "logger.h"



// Exception class for PageFetcher errors.
//
// Hides curl, HTTP and Browser Adapter errors.
// Other components only receive a simple error message.
class PageFetcherException : public std::runtime_error {

public:

    explicit PageFetcherException(const std::string& msg)
        : std::runtime_error(msg) {}

};





// PageFetcher
//
// Responsibility:
// Sends URL requests to Browser Adapter and returns rendered HTML.
//
// PageFetcher only knows HTTP communication.
// It does not know about Playwright, Chromium, Node.js or browser handling.
//
// If Browser Adapter technology changes, PageFetcher remains unchanged
// as long as the HTTP contract stays the same.
//
// PageFetcher does not start or stop Browser Adapter.
// It only communicates with an already running adapter service.
class PageFetcher {

private:

    // Browser Adapter base URL.
    // Example: http://localhost:3000
    std::string adapterBaseUrl;


    // Maximum wait time for one fetch request.
    long timeoutMs;


    // Optional logger for debugging and testing.
    Logger* logger;



    // Sends HTTP POST request to Browser Adapter.
    // Returns response body.
    // Throws PageFetcherException on HTTP/curl failures.
    std::string postJson(
        const std::string& path,
        const std::string& jsonBody
    ) const;



public:

    // Creates PageFetcher with adapter URL and timeout value.
    explicit PageFetcher(
        const std::string& adapterBaseUrl,
        long timeoutMs = 30000,
        Logger* logger = nullptr
    );


    ~PageFetcher();



    // Copying is disabled because PageFetcher manages
    // a single curl handle.
    PageFetcher(const PageFetcher&) = delete;

    PageFetcher& operator=(const PageFetcher&) = delete;



    // Fetches rendered HTML for given URL.
    //
    // Throws PageFetcherException if:
    // - Adapter is unreachable
    // - Request times out
    // - Adapter returns an error
    // - Response format is invalid
    std::string fetch(const std::string& url) const;



    // Checks whether Browser Adapter is running.
    //
    // Returns true only when adapter responds correctly.
    // Does not throw exceptions.
    bool isAdapterReachable() const;

};