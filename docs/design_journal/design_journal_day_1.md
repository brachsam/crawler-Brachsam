# Daily Journal Day 1

### Section 1 — Specific Bug

During our architectural design phase, we encountered a significant design conflict regarding the page-fetching mechanism. If we fetched web pages using a simple HTTP client like cURL, we would completely miss content from modern, JavaScript-driven websites where the DOM is dynamically rendered. Initially, the team debated if we should build standard HTTP requests. However, when we analyzed how modern platforms operate, we realized that downloading raw HTML alone would result in an empty page container for any Single Page Application (SPA). To solve this, we had to ensure that the browser execution engine remains isolated inside our crawler architecture.

---------------------------------------------------------

### Section 2 — Failed Attempt

Initially, I wanted to design the [Crawler](file:///c:/Users/brach/OneDrive/Documents/Project-02/include/crawler.h) to interact directly with browser-rendering commands. I thought that keeping all runtime orchestration in one class would simplify debugging. However, when trying to map this layout on paper, I realized it tightly coupled our orchestration logic to a specific browser API. If we decided to switch browser backends or use a lightweight HTTP fetcher for simple static sites later, we would have to rewrite the core crawler pipeline. To resolve this, I refactored the design to treat the headless browser as an internal implementation detail of [PageFetcher](file:///c:/Users/brach/OneDrive/Documents/Project-02/include/pagefetcher.h), exposing a simple fetch interface to the rest of the crawler and keeping it independent from browser-specific logic.

---------------------------------------------------------

### Section 3 — Code Reference

* **Git Commit:** `8418b3c`, `3b50cbf`
* **Major Files Modified:** None (Architecture & Roadmap Design Phase)
* **Components Implemented:**
  - [Frontier](file:///c:/Users/brach/OneDrive/Documents/Project-02/include/frontier.h) (Conceptual structure)
  - [SeenStore](file:///c:/Users/brach/OneDrive/Documents/Project-02/include/seenstore.h) (Conceptual structure)
  - [PageFetcher](file:///c:/Users/brach/OneDrive/Documents/Project-02/include/pagefetcher.h) (Conceptual structure)
  - [PageStorage](file:///c:/Users/brach/OneDrive/Documents/Project-02/include/pagestorage.h) (Conceptual structure)
  - [HTMLParser](file:///c:/Users/brach/OneDrive/Documents/Project-02/include/htmlParser.h) (Conceptual structure)
  - [URLNormalizer](file:///c:/Users/brach/OneDrive/Documents/Project-02/include/URLNormalizer.h) (Conceptual structure)
  - [URLValidator](file:///c:/Users/brach/OneDrive/Documents/Project-02/include/URLValidator.h) (Conceptual structure)

---------------------------------------------------------

### Section 4 — Learning Reflection

Today's architectural work deepened my understanding of the Separation of Concerns principle. Designing a production-style crawler taught me that mixing network communication, HTML parsing, and state management in one place creates a fragile codebase. By isolating the dynamic page fetching to a dedicated headless browser backend under the [PageFetcher](file:///c:/Users/brach/OneDrive/Documents/Project-02/include/pagefetcher.h) abstraction, the rest of our crawler remains unaffected. Furthermore, deciding to use our custom Project-01 Collections library for our queue and map primitives will test our data structure interfaces under real-world workloads. Designing the page storage database schema first ensures that the crawled HTML files can support downstream applications like an Indexer or Search Engine without requiring redundant network requests.
