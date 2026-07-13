#include "pagestorage.h"

// Opens the SQLite database and creates the pages table if it does not exist.
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