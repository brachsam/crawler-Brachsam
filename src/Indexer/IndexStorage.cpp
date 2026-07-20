#include "Indexer/IndexStorage.h"

IndexStorage::IndexStorage(const std::string& dbPath) : db(nullptr) {
    int rc = sqlite3_open(dbPath.c_str(), &db);
    if (rc != SQLITE_OK) {
        std::string msg = "IndexStorage: could not open database '" + dbPath + "': "
                          + (db ? sqlite3_errmsg(db) : "unknown error");
        if (db) sqlite3_close(db);
        db = nullptr;
        throw IndexStorageException(msg);
    }
    createTables();
}

IndexStorage::~IndexStorage() {
    if (db) sqlite3_close(db);
}

// Runs a plain SQL statement and throws if something goes wrong
void IndexStorage::exec(const std::string& sql) {
    char* err = nullptr;
    int rc = sqlite3_exec(db, sql.c_str(), nullptr, nullptr, &err);
    if (rc != SQLITE_OK) {
        std::string msg = "IndexStorage: SQL error: " + std::string(err ? err : "unknown");
        sqlite3_free(err);
        throw IndexStorageException(msg);
    }
}

// Creates the two tables we need in indexer.db if they don't already exist
void IndexStorage::createTables() {
    // Stores the URL and clean text for each document
    exec(
        "CREATE TABLE IF NOT EXISTS documents ("
        "  id           INTEGER PRIMARY KEY,"
        "  url          TEXT UNIQUE NOT NULL,"
        "  cleaned_text TEXT NOT NULL"
        ");"
    );

    // Stores which documents contain each word and how often
    exec(
        "CREATE TABLE IF NOT EXISTS term_postings ("
        "  term      TEXT    NOT NULL,"
        "  doc_id    INTEGER NOT NULL,"
        "  frequency INTEGER NOT NULL,"
        "  PRIMARY KEY (term, doc_id),"
        "  FOREIGN KEY (doc_id) REFERENCES documents(id) ON DELETE CASCADE"
        ");"
    );

    // Index on term so the search engine can look up a word instantly
    exec(
        "CREATE INDEX IF NOT EXISTS idx_term ON term_postings(term);"
    );
}

void IndexStorage::store(const HashMap<std::string, DynamicArray<Posting>>& index,
                         const DynamicArray<DocMeta>& docs) {
    // Do everything in one transaction - much faster than one insert at a time
    exec("BEGIN TRANSACTION;");

    // Save all document URLs and clean text first
    const char* docSql = "INSERT OR IGNORE INTO documents (id, url, cleaned_text) VALUES (?, ?, ?);";
    sqlite3_stmt* docStmt = nullptr;
    sqlite3_prepare_v2(db, docSql, -1, &docStmt, nullptr);

    for (int i = 0; i < docs.size(); i++) {
        const DocMeta& d = docs.get(i);
        sqlite3_bind_int(docStmt, 1, d.id);
        sqlite3_bind_text(docStmt, 2, d.url.c_str(), -1, SQLITE_TRANSIENT);
        sqlite3_bind_text(docStmt, 3, d.cleanedText.c_str(), -1, SQLITE_TRANSIENT);
        sqlite3_step(docStmt);
        sqlite3_reset(docStmt);
    }
    sqlite3_finalize(docStmt);

    // Save all postings (word -> document, frequency)
    const char* postSql = "INSERT OR IGNORE INTO term_postings (term, doc_id, frequency) VALUES (?, ?, ?);";
    sqlite3_stmt* postStmt = nullptr;
    sqlite3_prepare_v2(db, postSql, -1, &postStmt, nullptr);

    DynamicArray<std::string> terms = index.keys();
    for (int i = 0; i < terms.size(); i++) {
        const std::string& term = terms.get(i);
        const DynamicArray<Posting>& postings = index.get(term);

        for (int j = 0; j < postings.size(); j++) {
            const Posting& p = postings.get(j);
            sqlite3_bind_text(postStmt, 1, term.c_str(), -1, SQLITE_TRANSIENT);
            sqlite3_bind_int(postStmt, 2, p.docId);
            sqlite3_bind_int(postStmt, 3, p.tf);
            sqlite3_step(postStmt);
            sqlite3_reset(postStmt);
        }
    }
    sqlite3_finalize(postStmt);

    exec("COMMIT;");
}
