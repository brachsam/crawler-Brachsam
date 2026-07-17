# Daily Journal Day 5

### Section 1 — Specific Bug

During our first multi-page end-to-end crawl test, the crawler fell into infinite loops when encountering pages that linked to each other (e.g. Page A pointing to Page B, which linked back to Page A). This caused the system to fetch the same pages repeatedly. The bug occurred because we were only checking [SeenStore](file:///c:/Users/brach/OneDrive/Documents/Project-02/include/seenstore.h) to mark a URL as visited *after* dequeuing and fetching the page, rather than before adding it to the queue. I resolved this circular dependency bug by checking and marking the URL as visited in the [SeenStore](file:///c:/Users/brach/OneDrive/Documents/Project-02/include/seenstore.h) *before* enqueuing it to the [Frontier](file:///c:/Users/brach/OneDrive/Documents/Project-02/include/frontier.h).

---------------------------------------------------------

### Section 2 — Failed Attempt

Initially, our parser only looked for literal `"href="` matches using a simple index lookup. However, when we ran the crawler on malformed pages, it missed valid links that used single quotes or had spaces around the equals sign, such as `<a href  =  'url'>`. I first tried adding several string pattern checks to capture these, but the logic quickly became messy and fragile. Instead, I refactored [HTMLParser::extractLinks](file:///c:/Users/brach/OneDrive/Documents/Project-02/src/htmlParser.cpp#L71) to locate the `"href"` key using the [KMP](file:///c:/Users/brach/OneDrive/Documents/Project-02/include/kmp.h) matcher and then scan character-by-character to skip whitespace and capture the quote boundary. This reliably parsed both single/double quoted and unquoted attributes.

---------------------------------------------------------

### Section 3 — Code Reference

* **Git Commit:** `1056afc`, `8eab7fc`, `9cabdbf`, `e0c2cdb`, `97cbb2e`, `65221f7`, `37eb2da`
* **Major Files Modified:**
  - [kmp.h](file:///c:/Users/brach/OneDrive/Documents/Project-02/include/kmp.h) & [kmp.cpp](file:///c:/Users/brach/OneDrive/Documents/Project-02/src/kmp.cpp)
  - [htmlParser.h](file:///c:/Users/brach/OneDrive/Documents/Project-02/include/htmlParser.h) & [htmlParser.cpp](file:///c:/Users/brach/OneDrive/Documents/Project-02/src/htmlParser.cpp)
  - [crawler.h](file:///c:/Users/brach/OneDrive/Documents/Project-02/include/crawler.h) & [crawler.cpp](file:///c:/Users/brach/OneDrive/Documents/Project-02/src/crawler.cpp)
* **Components Implemented:**
  - [KMP](file:///c:/Users/brach/OneDrive/Documents/Project-02/include/kmp.h) String Matcher
  - [HTMLParser](file:///c:/Users/brach/OneDrive/Documents/Project-02/include/htmlParser.h)
  - [Crawler](file:///c:/Users/brach/OneDrive/Documents/Project-02/include/crawler.h) Pipeline

---------------------------------------------------------

### Section 4 — Learning Reflection

Today's string matching research taught me the value of using guaranteed linear-time algorithms like Knuth-Morris-Pratt (KMP) over naive scanning or hashing algorithms like Rabin-Karp when parsing massive HTML files. Precomputing the pattern's Longest Prefix Suffix (LPS) failure table completely avoids backtracking, ensuring $O(N + M)$ performance. I also learned that in graph traversals like BFS crawling, marking nodes as "visited" must happen at the time of *discovery* (when enqueuing), not at the time of *processing* (when dequeuing). Moving the duplicate check to the enqueuing phase immediately eliminated duplicate downloads, ensuring the crawler is network-efficient and safe from infinite circular loops.
