#include "pagestorage.h"

// Opens the SQLite database and creates the tables if they do not exist.
PageStorage::PageStorage(const std::string& path) : db(nullptr), dbPath(path) {
    int rc = sqlite3_open(path.c_str(), &db);
    if (rc != SQLITE_OK) {
        std::string msg = "PageStorage: could not open database '" + path + "': "
                           + (db ? sqlite3_errmsg(db) : "unknown error");
        if (db) sqlite3_close(db);
        db = nullptr;
        throw PageStorageException(msg);
    }

    execOrThrow(
        "CREATE TABLE IF NOT EXISTS pages ("
        "  id INTEGER PRIMARY KEY AUTOINCREMENT,"
        "  url TEXT UNIQUE NOT NULL,"
        "  depth INTEGER NOT NULL,"
        "  html TEXT NOT NULL"
        ");"
    );

    execOrThrow(
        "CREATE TABLE IF NOT EXISTS pending_urls ("
        "  id INTEGER PRIMARY KEY AUTOINCREMENT,"
        "  url TEXT UNIQUE NOT NULL,"
        "  depth INTEGER NOT NULL,"
        "  parent_url TEXT,"
        "  status INTEGER DEFAULT 0,"
        "  priority INTEGER DEFAULT 0,"
        "  retry_count INTEGER DEFAULT 0,"
        "  last_attempt_time INTEGER DEFAULT 0"
        ");"
    );
}

// Closes the database connection.
PageStorage::~PageStorage() {
    if (db) sqlite3_close(db);
}

// Executes an SQL statement and throws an exception if it fails.
void PageStorage::execOrThrow(const std::string& sql) {
    char* errMsg = nullptr;
    int rc = sqlite3_exec(db, sql.c_str(), nullptr, nullptr, &errMsg);
    if (rc != SQLITE_OK) {
        std::string msg = "PageStorage: SQL error: " + std::string(errMsg ? errMsg : "unknown");
        sqlite3_free(errMsg);
        throw PageStorageException(msg);
    }
}

// Saves a page to the database.
// If the URL already exists, its data is updated.
void PageStorage::savePage(const Page& page) {
    const char* sql = "INSERT INTO pages (url, depth, html) VALUES (?, ?, ?) "
                      "ON CONFLICT(url) DO UPDATE SET depth = excluded.depth, html = excluded.html;";

    sqlite3_stmt* stmt = nullptr;
    if (sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr) != SQLITE_OK) {
        throw PageStorageException("PageStorage::savePage - prepare failed: " + std::string(sqlite3_errmsg(db)));
    }

    // Bind page data to the SQL query.
    sqlite3_bind_text(stmt, 1, page.url.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_int(stmt, 2, page.depth);
    sqlite3_bind_text(stmt, 3, page.html.c_str(), -1, SQLITE_TRANSIENT);

    int rc = sqlite3_step(stmt);
    sqlite3_finalize(stmt);

    if (rc != SQLITE_DONE) {
        throw PageStorageException("PageStorage::savePage - insert/update failed: " + std::string(sqlite3_errmsg(db)));
    }
}

// Retrieves a page from the database using its URL.
Page PageStorage::getPage(const std::string& url) const {
    const char* sql = "SELECT depth, html FROM pages WHERE url = ? LIMIT 1;";

    sqlite3_stmt* stmt = nullptr;
    if (sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr) != SQLITE_OK) {
        throw PageStorageException("PageStorage::getPage - prepare failed: " + std::string(sqlite3_errmsg(db)));
    }

    // Bind the URL to the SQL query.
    sqlite3_bind_text(stmt, 1, url.c_str(), -1, SQLITE_TRANSIENT);

    int rc = sqlite3_step(stmt);
    if (rc != SQLITE_ROW) {
        sqlite3_finalize(stmt);
        throw PageNotFoundException("PageStorage::getPage - no page stored for url: " + url);
    }

    // Populate the Page object from the query result.
    Page page;
    page.url = url;
    page.depth = sqlite3_column_int(stmt, 0);
    const unsigned char* htmlText = sqlite3_column_text(stmt, 1);
    page.html = htmlText ? reinterpret_cast<const char*>(htmlText) : "";

    sqlite3_finalize(stmt);
    return page;
}

// Checks whether a page with the given URL exists in the database.
bool PageStorage::exists(const std::string& url) const {
    const char* sql = "SELECT 1 FROM pages WHERE url = ? LIMIT 1;";

    sqlite3_stmt* stmt = nullptr;
    if (sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr) != SQLITE_OK) {
        throw PageStorageException("PageStorage::exists - prepare failed: " + std::string(sqlite3_errmsg(db)));
    }

    // Bind the URL to the SQL query.
    sqlite3_bind_text(stmt, 1, url.c_str(), -1, SQLITE_TRANSIENT);

    int rc = sqlite3_step(stmt);
    bool found = (rc == SQLITE_ROW);

    sqlite3_finalize(stmt);
    return found;
}

// Returns the total number of pages stored in the database.
int PageStorage::count() const {
    const char* sql = "SELECT COUNT(*) FROM pages;";

    sqlite3_stmt* stmt = nullptr;
    if (sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr) != SQLITE_OK) {
        throw PageStorageException("PageStorage::count - prepare failed: " + std::string(sqlite3_errmsg(db)));
    }

    int result = 0;
    if (sqlite3_step(stmt) == SQLITE_ROW) {
        result = sqlite3_column_int(stmt, 0);
    }

    sqlite3_finalize(stmt);
    return result;
}

// Checks if we already have a crawled page for this URL.
// Just calls exists() — this wrapper is here so the public API
// matches the name we promised in the design proposal.
bool PageStorage::hasPage(const std::string& url) const {
    return exists(url);
}

// Looks up a page by its auto-increment database ID and returns the URL.
// Project-03's indexer needs this: it loops from id = 1 up to count()
// and fetches every stored page one by one using its serial number.
std::string PageStorage::getURLByID(int id) const {
    // We only need the url column — the id column is the primary key,
    // so SQLite finds the row almost instantly (indexed lookup).
    const char* sql = "SELECT url FROM pages WHERE id = ? LIMIT 1;";

    sqlite3_stmt* stmt = nullptr;
    if (sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr) != SQLITE_OK) {
        throw PageStorageException(
            "PageStorage::getURLByID - prepare failed: "
            + std::string(sqlite3_errmsg(db)));
    }

    // Bind the ID we're looking for into the query.
    sqlite3_bind_int(stmt, 1, id);

    // Run the query — if a row comes back, the ID exists.
    int rc = sqlite3_step(stmt);
    if (rc != SQLITE_ROW) {
        // No row means this ID was never assigned to any page.
        sqlite3_finalize(stmt);
        throw PageNotFoundException(
            "PageStorage::getURLByID - no page with id: "
            + std::to_string(id));
    }

    // Read the url string out of column 0 (the only column we selected).
    const unsigned char* urlText = sqlite3_column_text(stmt, 0);
    std::string url = urlText ? reinterpret_cast<const char*>(urlText) : "";

    sqlite3_finalize(stmt);
    return url;
}

// Adds a URL to the pending_urls table.
void PageStorage::savePendingUrl(const std::string& url, int depth, const std::string& parentUrl) {
    const char* sql = "INSERT INTO pending_urls (url, depth, parent_url, status) VALUES (?, ?, ?, 0) "
                      "ON CONFLICT(url) DO NOTHING;";

    sqlite3_stmt* stmt = nullptr;
    if (sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr) != SQLITE_OK) {
        throw PageStorageException("PageStorage::savePendingUrl - prepare failed: " + std::string(sqlite3_errmsg(db)));
    }

    sqlite3_bind_text(stmt, 1, url.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_int(stmt, 2, depth);
    if (parentUrl.empty()) {
        sqlite3_bind_null(stmt, 3);
    } else {
        sqlite3_bind_text(stmt, 3, parentUrl.c_str(), -1, SQLITE_TRANSIENT);
    }

    int rc = sqlite3_step(stmt);
    sqlite3_finalize(stmt);

    if (rc != SQLITE_DONE) {
        throw PageStorageException("PageStorage::savePendingUrl - insert failed: " + std::string(sqlite3_errmsg(db)));
    }
}

// Updates the status of a pending URL.
void PageStorage::updatePendingUrlStatus(const std::string& url, int status) {
    const char* sql = "UPDATE pending_urls SET status = ? WHERE url = ?;";

    sqlite3_stmt* stmt = nullptr;
    if (sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr) != SQLITE_OK) {
        throw PageStorageException("PageStorage::updatePendingUrlStatus - prepare failed: " + std::string(sqlite3_errmsg(db)));
    }

    sqlite3_bind_int(stmt, 1, status);
    sqlite3_bind_text(stmt, 2, url.c_str(), -1, SQLITE_TRANSIENT);

    int rc = sqlite3_step(stmt);
    sqlite3_finalize(stmt);

    if (rc != SQLITE_DONE) {
        throw PageStorageException("PageStorage::updatePendingUrlStatus - update failed: " + std::string(sqlite3_errmsg(db)));
    }
}

// Resets all IN_PROGRESS (status=1) URLs back to PENDING (status=0).
void PageStorage::resetInProgressUrls() {
    execOrThrow("UPDATE pending_urls SET status = 0 WHERE status = 1;");
}

// Loads pending URLs from the database up to a certain limit, optionally filtered by host.
DynamicArray<URLDepth> PageStorage::loadPendingUrls(int limit, const std::string& hostFilter) {
    DynamicArray<URLDepth> result;
    
    std::string sql;
    if (hostFilter.empty()) {
        sql = "SELECT url, depth FROM pending_urls WHERE status = 0 ORDER BY priority DESC, id ASC LIMIT ?;";
    } else {
        sql = "SELECT url, depth FROM pending_urls WHERE status = 0 AND url LIKE ? ORDER BY priority DESC, id ASC LIMIT ?;";
    }

    sqlite3_stmt* stmt = nullptr;
    if (sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, nullptr) != SQLITE_OK) {
        throw PageStorageException("PageStorage::loadPendingUrls - prepare failed: " + std::string(sqlite3_errmsg(db)));
    }

    if (hostFilter.empty()) {
        sqlite3_bind_int(stmt, 1, limit);
    } else {
        // Match any URL containing the host name (e.g. "%ssipmt.edu.in%")
        std::string likePattern = "%" + hostFilter + "%";
        sqlite3_bind_text(stmt, 1, likePattern.c_str(), -1, SQLITE_TRANSIENT);
        sqlite3_bind_int(stmt, 2, limit);
    }

    while (sqlite3_step(stmt) == SQLITE_ROW) {
        URLDepth item;
        const unsigned char* urlText = sqlite3_column_text(stmt, 0);
        item.url = urlText ? reinterpret_cast<const char*>(urlText) : "";
        item.depth = sqlite3_column_int(stmt, 1);
        result.append(item);
    }

    sqlite3_finalize(stmt);
    return result;
}

// Returns the count of pending URLs (status=0).
int PageStorage::pendingCount() const {
    const char* sql = "SELECT COUNT(*) FROM pending_urls WHERE status = 0;";

    sqlite3_stmt* stmt = nullptr;
    if (sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr) != SQLITE_OK) {
        throw PageStorageException("PageStorage::pendingCount - prepare failed: " + std::string(sqlite3_errmsg(db)));
    }

    int result = 0;
    if (sqlite3_step(stmt) == SQLITE_ROW) {
        result = sqlite3_column_int(stmt, 0);
    }

    sqlite3_finalize(stmt);
    return result;
}

// Returns a list of all URLs present in the database (crawled or pending).
DynamicArray<std::string> PageStorage::getAllSeenUrls() const {
    DynamicArray<std::string> result;
    const char* sql = "SELECT url FROM pages UNION SELECT url FROM pending_urls;";

    sqlite3_stmt* stmt = nullptr;
    if (sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr) != SQLITE_OK) {
        throw PageStorageException("PageStorage::getAllSeenUrls - prepare failed: " + std::string(sqlite3_errmsg(db)));
    }

    while (sqlite3_step(stmt) == SQLITE_ROW) {
        const unsigned char* urlText = sqlite3_column_text(stmt, 0);
        if (urlText) {
            result.append(reinterpret_cast<const char*>(urlText));
        }
    }

    sqlite3_finalize(stmt);
    return result;
}

// Clears the pending_urls table (useful for testing or full resets).
void PageStorage::clearPendingUrls() {
    execOrThrow("DELETE FROM pending_urls;");
}