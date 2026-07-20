#include "Indexer/InvertedIndexBuilder.h"

void InvertedIndexBuilder::add(int docId, const HashMap<std::string, int>& freqs) {
    // Go through every word in this document's frequency map
    DynamicArray<std::string> terms = freqs.keys();

    for (int i = 0; i < terms.size(); i++) {
        const std::string& term = terms.get(i);
        int tf = freqs.get(term);

        Posting p;
        p.docId = docId;
        p.tf    = tf;

        if (idxMap.exists(term)) {
            // This word was already seen in a previous document, just add a new posting
            idxMap.get(term).append(p);
        } else {
            // First time we see this word, create a fresh postings list for it
            DynamicArray<Posting> list;
            list.append(p);
            idxMap.set(term, list);
        }
    }
}

const HashMap<std::string, DynamicArray<Posting>>& InvertedIndexBuilder::getIndex() const {
    return idxMap;
}

void InvertedIndexBuilder::clear() {
    idxMap.clear();
}
