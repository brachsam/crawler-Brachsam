#pragma once
#include <string>
#include <stdexcept>
#include <sqlite3.h>
#include "common.h"

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
};