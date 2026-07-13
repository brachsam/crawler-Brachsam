#pragma once
#include <ostream>
#include <string>

struct URLDepth {
    std::string url;
    int depth;
};

inline bool operator==(const URLDepth& lhs, const URLDepth& rhs) {
    return lhs.url == rhs.url && lhs.depth == rhs.depth;
}

inline std::ostream& operator<<(std::ostream& os, const URLDepth& u) {
    return os << u.url << " (depth " << u.depth << ")";
}