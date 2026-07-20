#include "Indexer/StopWordFilter.h"

StopWordFilter::StopWordFilter() {
    load();
}

// Adds all the stop words into the HashMap on startup.
// These are just very common English words that don't help with searching.
void StopWordFilter::load() {
    const char* list[] = {
        "a", "an", "the", "and", "or", "but", "if", "in", "on", "at",
        "to", "for", "of", "with", "by", "from", "is", "are", "was",
        "were", "be", "been", "being", "have", "has", "had", "do", "does",
        "did", "will", "would", "could", "should", "may", "might", "shall",
        "can", "not", "no", "nor", "so", "yet", "both", "either", "as",
        "up", "out", "about", "into", "than", "then", "that", "this",
        "these", "those", "it", "its", "i", "me", "my", "we", "our",
        "you", "your", "he", "she", "his", "her", "they", "their", "them",
        "what", "which", "who", "whom", "how", "when", "where", "why",
        "all", "each", "more", "also", "just", "over", "such", "after",
        "before", "between", "through", "during", "while", "although"
    };

    int count = sizeof(list) / sizeof(list[0]);
    for (int i = 0; i < count; i++) {
        words.set(std::string(list[i]), true);
    }
}

bool StopWordFilter::isStopWord(const std::string& word) const {
    return words.exists(word);
}
