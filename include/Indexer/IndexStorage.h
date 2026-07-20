#pragma once
#include <string>
#include <stdexcept>
#include <sqlite3.h>
#include "DynamicArray/Dynamicarray.h"
#include "HashMap/hmap.h"
#include "Indexer/InvertedIndexBuilder.h"

// Exception for any indexer storage related errors
class IndexStorageException : public std::runtime_error {
public:
    explicit IndexStorageException(const std::string& msg) : std::runtime_error(msg) {}
};

// Holds the URL, ID, and cleaned words of a document.
// We store this in indexer.db so the search engine can resolve doc IDs to URLs and cleaned text.
struct DocMeta {
    int id;
    std::string url;
    std::string cleanedText;
};

// Writes the inverted index and document metadata into indexer.db.
// The search engine will later read from this same database.
class IndexStorage {
public:
    explicit IndexStorage(const std::string& dbPath);
    ~IndexStorage();

    // No copying - we own a live database connection
    IndexStorage(const IndexStorage&) = delete;
    IndexStorage& operator=(const IndexStorage&) = delete;

    // Saves the full inverted index and document list into indexer.db
    void store(const HashMap<std::string, DynamicArray<Posting>>& index,
               const DynamicArray<DocMeta>& docs);

private:
    sqlite3* db;

    void exec(const std::string& sql);
    void createTables();
};
