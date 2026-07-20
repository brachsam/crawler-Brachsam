#pragma once
#include <string>
#include "Indexer/DocumentReader.h"
#include "Indexer/TextExtractor.h"
#include "Indexer/Tokenizer.h"
#include "Indexer/Normalizer.h"
#include "Indexer/StopWordFilter.h"
#include "Indexer/TermFrequencyCounter.h"
#include "Indexer/InvertedIndexBuilder.h"
#include "Indexer/IndexStorage.h"

// The main orchestrator. It drives the full indexing pipeline:
// read pages from crawler.db -> extract text -> tokenize -> normalize
// -> filter stop words -> count frequencies -> build index -> save to indexer.db
class Indexer {
public:
    Indexer(const std::string& crawlerDb, const std::string& indexerDb);
    void run();

private:
    std::string cDb;
    std::string iDb;

    DocumentReader      reader;
    TextExtractor       extractor;
    Tokenizer           tokenizer;
    Normalizer          normalizer;
    StopWordFilter      filter;
    TermFrequencyCounter counter;
    InvertedIndexBuilder builder;
    IndexStorage        storage;
};
