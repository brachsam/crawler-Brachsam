#include "SeenStore.h"

SeenStore::SeenStore() = default;

SeenStore::~SeenStore() = default;

void SeenStore::markSeen(const std::string& url) {
    seen.set(url, true);
}

bool SeenStore::isSeen(const std::string& url) const {
    return seen.exists(url);
}

int SeenStore::size() const {
    return seen.size();
}