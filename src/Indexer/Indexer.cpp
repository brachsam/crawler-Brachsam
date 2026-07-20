#include "Indexer/Indexer.h"
#include <iostream>

Indexer::Indexer(const std::string& crawlerDb, const std::string& indexerDb)
    : cDb(crawlerDb),
      iDb(indexerDb),
      reader(crawlerDb),
      storage(indexerDb) {
}

void Indexer::run() {
    DynamicArray<int> ids = reader.getDocIds();
    int total = ids.size();
    std::cout << "[Indexer] Found " << total << " pages in crawler.db\n";

    // We collect doc metadata (id + url) here so we can write it to indexer.db later
    DynamicArray<DocMeta> docs;

    for (int i = 0; i < total; i++) {
        int id = ids.get(i);
        Page page = reader.read(id);

        std::cout << "[Indexer] Processing " << (i + 1) << "/" << total
                  << " - " << page.url << "\n";

        // Step 1: strip HTML tags, get clean text
        std::string text = extractor.extract(page.html);

        // Step 2: split text into raw words
        DynamicArray<std::string> rawTokens = tokenizer.tokenize(text);

        // Step 3: normalize and filter each word
        DynamicArray<std::string> cleanTokens;
        for (int i = 0; i < rawTokens.size(); i++) {
            std::string word = normalizer.normalize(rawTokens.get(i));

            // Skip empty strings and stop words
            if (word.empty()) continue;
            if (filter.isStopWord(word)) continue;
            if (word.size() < 2) continue;

            cleanTokens.append(word);
        }

        // Step 4: count how often each word appears in this page
        HashMap<std::string, int> freqs = counter.count(cleanTokens);

        // Step 5: add this page's word frequencies to the global index
        builder.add(id, freqs);

        // Build a space-separated string of all clean words for this page
        std::string cleanedText;
        for (int j = 0; j < cleanTokens.size(); j++) {
            if (j > 0) cleanedText += " ";
            cleanedText += cleanTokens.get(j);
        }

        // Remember this document's metadata for later
        DocMeta meta;
        meta.id          = id;
        meta.url         = page.url;
        meta.cleanedText = cleanedText;
        docs.append(meta);
    }

    // Step 6: write everything to indexer.db in one go
    std::cout << "[Indexer] Saving index to indexer.db...\n";
    storage.store(builder.getIndex(), docs);

    std::cout << "[Indexer] Done.\n";
}
