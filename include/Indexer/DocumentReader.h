#pragma once
#include <string>
#include "pagestorage.h"
#include "common.h"

// Reads crawled pages from crawler.db one by one.
// It wraps PageStorage so the rest of the indexer never touches the database directly.
class DocumentReader {
public:
    explicit DocumentReader(const std::string& dbPath);

    // Returns total number of crawled pages stored in crawler.db
    int count() const;

    // Reads a single page by its database ID (1, 2, 3 ...)
    Page read(int id) const;

    // Gets the exact list of existing IDs in the pages table to handle gaps
    DynamicArray<int> getDocIds() const;

private:
    std::string dbPath;
    PageStorage storage;
};
