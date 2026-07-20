#include "Indexer/TermFrequencyCounter.h"

HashMap<std::string, int> TermFrequencyCounter::count(const DynamicArray<std::string>& tokens) const {
    HashMap<std::string, int> freq;

    for (int i = 0; i < tokens.size(); i++) {
        const std::string& word = tokens.get(i);

        if (freq.exists(word)) {
            // Word already seen, just bump its count
            freq.get(word)++;
        } else {
            // First time seeing this word
            freq.set(word, 1);
        }
    }

    return freq;
}
