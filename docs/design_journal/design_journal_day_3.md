# Daily Journal Day 3

### Section 1 — Specific Bug

During host parsing in [RobotsHandler](file:///c:/Users/brach/OneDrive/Documents/Project-02/include/robotsHandler.h), we hit bugs when splitting hostnames out of URLs that contained port specifications (e.g. `http://localhost:8080/index.html`) or lacked protocol prefixes entirely. The extraction helper method `extractHost()` was failing to isolate the hostname properly, resulting in incorrect matching against disallowed paths because it included the port number or subsequent URL fragments. I fixed this by writing more robust parsing logic inside the helper to look for the protocol double-slash `//`, find the next slash or colon, and extract only the substring representing the host domain.

---------------------------------------------------------

### Section 2 — Failed Attempt

Initially, I used a `DynamicArray` to hold URLs inside the [Frontier](file:///c:/Users/brach/OneDrive/Documents/Project-02/include/frontier.h) queue. However, when dequeuing elements from the front of a `DynamicArray`, every operation requires shifting all subsequent elements down by one position, which takes $O(N)$ linear time. When dealing with a large crawling queue, this would quickly degrade performance. Recognizing this inefficiency, I refactored the [Frontier](file:///c:/Users/brach/OneDrive/Documents/Project-02/include/frontier.h) class to wrap Project-01's `LinkedList` structure. This allowed us to perform both enqueuing (tail insertion) and dequeuing (head deletion) in $O(1)$ constant time.

---------------------------------------------------------

### Section 3 — Code Reference

* **Git Commit:** `e15b1cb`, `9a6cc73`, `2fc272e`
* **Major Files Modified:**
  - [frontier.h](file:///c:/Users/brach/OneDrive/Documents/Project-02/include/frontier.h) & [frontier.cpp](file:///c:/Users/brach/OneDrive/Documents/Project-02/src/frontier.cpp)
  - [seenstore.h](file:///c:/Users/brach/OneDrive/Documents/Project-02/include/seenstore.h) & [seenstore.cpp](file:///c:/Users/brach/OneDrive/Documents/Project-02/src/seenstore.cpp)
  - [robotsHandler.h](file:///c:/Users/brach/OneDrive/Documents/Project-02/include/robotsHandler.h) & [robotsHandler.cpp](file:///c:/Users/brach/OneDrive/Documents/Project-02/src/robotsHandler.cpp)
  - [URLDepth.h](file:///c:/Users/brach/OneDrive/Documents/Project-02/include/URLDepth.h)
* **Components Implemented:**
  - [Frontier](file:///c:/Users/brach/OneDrive/Documents/Project-02/include/frontier.h)
  - [SeenStore](file:///c:/Users/brach/OneDrive/Documents/Project-02/include/seenstore.h)
  - [RobotsHandler](file:///c:/Users/brach/OneDrive/Documents/Project-02/include/robotsHandler.h)

---------------------------------------------------------

### Section 4 — Learning Reflection

Today's session reinforced the importance of choosing the correct data structure for core algorithms. Building the [Frontier](file:///c:/Users/brach/OneDrive/Documents/Project-02/include/frontier.h) queue showed that wrapping low-level collections (like `LinkedList`) in clean interfaces hides implementation details and guarantees key performance bounds. Similarly, wrapping `HashMap` inside [SeenStore](file:///c:/Users/brach/OneDrive/Documents/Project-02/include/seenstore.h) ensures average $O(1)$ lookup times, which is essential to handle duplicate checks efficiently. We also addressed the latency overhead of fetching `robots.txt` rules by implementing a static in-memory cache for up to 32 hosts (`MAX_CACHED_HOSTS`). This design pattern significantly reduces redundant HTTP fetches, demonstrating that smart in-memory caches are critical when building high-performance, respectful web crawlers.
