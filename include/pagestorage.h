#pragma once
#include <string>
#include <stdexcept>
#include <sqlite3.h>
#include "common.h"
#include "Dynamicarray.h"

// Exception thrown for PageStorage related errors
class PageStorageException : public std::runtime_error {
public:
    explicit PageStorageException(const std::string& msg) : std::runtime_error(msg) {}
};

// Exception thrown when a requested page is not found.
class PageNotFoundException : public std::runtime_error {
public:
    explicit PageNotFoundException(const std::string& msg) : std::runtime_error(msg) {}
};

// Stores and retrieves crawled pages using a SQLite database.
class PageStorage {
private:
    // SQLite database connection.
    sqlite3* db;

    // Path of the database file.
    std::string dbPath;

    // Executes an SQL query and throws an exception on failure.
    void execOrThrow(const std::string& sql);

public:
    explicit PageStorage(const std::string& path);
    ~PageStorage();

    // Prevent copying of PageStorage.
    PageStorage(const PageStorage&) = delete;
    PageStorage& operator=(const PageStorage&) = delete;

    // Saves a page to the database.
    // Updates the existing record if the URL already exists.
    void savePage(const Page& page);

    // Throws PageNotFoundException if the URL has never been stored.
    Page getPage(const std::string& url) const;

    // Checks whether a page exists in the database.
    bool exists(const std::string& url) const;

    // Returns the total number of stored pages.
    int count() const;

    // Returns true if a page with this URL has been crawled and stored.
    // This is the name used in the design proposal (wraps exists() internally).
    bool hasPage(const std::string& url) const;

    // Given a page's database ID (1, 2, 3, ...), returns its URL.
    // Project-03's indexer will loop from 1 to count() calling this.
    // Throws PageNotFoundException if no page has that ID.
    std::string getURLByID(int id) const;

    // Adds a URL to the pending_urls table.
    void savePendingUrl(const std::string& url, int depth, const std::string& parentUrl = "");

    // Updates the status of a pending URL.
    void updatePendingUrlStatus(const std::string& url, int status);

    // Resets all IN_PROGRESS (status=1) URLs back to PENDING (status=0).
    void resetInProgressUrls();

    // Loads pending URLs from the database up to a certain limit, optionally filtered by host/domain.
    DynamicArray<URLDepth> loadPendingUrls(int limit = 100, const std::string& hostFilter = "");

    // Returns the count of pending URLs (status=0).
    int pendingCount() const;

    // Returns a list of all URLs present in the database (crawled or pending).
    DynamicArray<std::string> getAllSeenUrls() const;

    // Clears the pending_urls table (useful for testing or full resets).
    void clearPendingUrls();
};