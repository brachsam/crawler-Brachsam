#pragma once
#include <string>
#include <ostream>
#include "DynamicArray/Dynamicarray.h"
#include "HashMap/hmap.h"

// One entry in the postings list.
// Tells us: in document docId, this word appeared tf times.
struct Posting {
    int docId;
    int tf;
};

// The HashMap's print() internally tries to stream its value type.
// We add this so the template compiles even though we never call print().
inline std::ostream& operator<<(std::ostream& os, const Posting& p) {
    return os << "(" << p.docId << "," << p.tf << ")";
}

inline std::ostream& operator<<(std::ostream& os, const DynamicArray<Posting>&) {
    return os << "[postings]";
}

// Builds the inverted index in memory.
// For each word it keeps a list of documents where that word was found.
// Example: "cat" -> [{docId:1, tf:3}, {docId:5, tf:1}]
class InvertedIndexBuilder {
public:
    // Add all the words from one document into the index
    void add(int docId, const HashMap<std::string, int>& freqs);

    // Get the full index after all documents are processed
    const HashMap<std::string, DynamicArray<Posting>>& getIndex() const;

    // Wipe the index clean (useful for testing)
    void clear();

private:
    HashMap<std::string, DynamicArray<Posting>> idxMap;
};
