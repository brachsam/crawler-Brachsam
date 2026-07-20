#include <iostream>
#include <string>
#include "Indexer/Indexer.h"

// Entry point for the indexer executable.
// Usage: Indexer <crawler.db path> <indexer.db path>
int main(int argc, char* argv[]) {
    if (argc < 3) {
        std::cerr << "Usage: " << argv[0] << " <crawler.db> <indexer.db>\n";
        std::cerr << "  crawler.db  - path to the crawler database\n";
        std::cerr << "  indexer.db  - path where the index will be saved\n";
        return 1;
    }

    std::string crawlerDb = argv[1];
    std::string indexerDb = argv[2];

    try {
        Indexer indexer(crawlerDb, indexerDb);
        indexer.run();
    }
    catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << "\n";
        return 1;
    }

    return 0;
}
