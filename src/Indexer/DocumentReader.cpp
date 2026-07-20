#include "Indexer/DocumentReader.h"

#include <sqlite3.h>
#include <stdexcept>

DocumentReader::DocumentReader(const std::string& dbPath)
    : dbPath(dbPath), storage(dbPath) {
}

int DocumentReader::count() const {
    return storage.count();
}

Page DocumentReader::read(int id) const {
    // Get the URL for this ID, then fetch the full page using that URL
    std::string url = storage.getURLByID(id);
    return storage.getPage(url);
}

DynamicArray<int> DocumentReader::getDocIds() const {
    DynamicArray<int> ids;
    sqlite3* db = nullptr;

    int rc = sqlite3_open_v2(dbPath.c_str(), &db, SQLITE_OPEN_READONLY, nullptr);
    if (rc != SQLITE_OK) {
        if (db) sqlite3_close(db);
        throw std::runtime_error("DocumentReader: cannot open database to read IDs");
    }

    const char* sql = "SELECT id FROM pages ORDER BY id ASC;";
    sqlite3_stmt* stmt = nullptr;

    if (sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr) == SQLITE_OK) {
        while (sqlite3_step(stmt) == SQLITE_ROW) {
            ids.append(sqlite3_column_int(stmt, 0));
        }
    }

    sqlite3_finalize(stmt);
    sqlite3_close(db);

    return ids;
}
