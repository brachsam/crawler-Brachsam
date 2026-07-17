# Daily Journal Day 2

### Section 1 — Specific Bug

When parsing the configuration file, we ran into an issue where the keys and values were being read with trailing carriage returns (`\r`), comments (prefixed with `#`), or extra spaces. For example, a line like `crawl_delay_ms= 0 # comment` would extract `crawl_delay_ms` as the key but fail to convert `" 0 # comment"` to a valid integer. This threw parser conversion exceptions in [ConfigManager](file:///c:/Users/brach/OneDrive/Documents/Project-02/include/configmanager.h). We resolved this by building a dedicated string trimming utility to strip comments and trailing whitespaces from the parsed tokens before map insertion.

---------------------------------------------------------

### Section 2 — Failed Attempt

In my first implementation of the [URLValidator](file:///c:/Users/brach/OneDrive/Documents/Project-02/include/URLValidator.h) checks, I tried to write a single, complex boolean logic expression containing nested string matches to validate URL schemes and protocols. This quickly became hard to read and debug. When checking mailto links and hashes, the logic incorrectly let empty hashes and case-variant protocols (`MAILTO:`) slip through. Recognizing this, I refactored the validator class to use a modular check structure with dedicated helper functions such as `isJavascriptLink`, `isMailtoLink`, `isBareFragment`, and `isHttpOrHttps`. This made the code self-documenting and allowed unit tests to pinpoint exact bugs in our parsing rules.

---------------------------------------------------------

### Section 3 — Code Reference

* **Git Commit:** `884fd0c`, `e9d20ae`, `abba098`, `1aa0d09`
* **Major Files Modified:**
  - [URLNormalizer.h](file:///c:/Users/brach/OneDrive/Documents/Project-02/include/URLNormalizer.h) & [URLNormalizer.cpp](file:///c:/Users/brach/OneDrive/Documents/Project-02/src/URLNormalizer.cpp)
  - [URLValidator.h](file:///c:/Users/brach/OneDrive/Documents/Project-02/include/URLValidator.h) & [URLValidator.cpp](file:///c:/Users/brach/OneDrive/Documents/Project-02/src/URLValidator.cpp)
  - [configmanager.h](file:///c:/Users/brach/OneDrive/Documents/Project-02/include/configmanager.h) & [configmanager.cpp](file:///c:/Users/brach/OneDrive/Documents/Project-02/src/configmanager.cpp)
  - [logger.h](file:///c:/Users/brach/OneDrive/Documents/Project-02/include/logger.h) & [logger.cpp](file:///c:/Users/brach/OneDrive/Documents/Project-02/src/logger.cpp)
* **Components Implemented:**
  - [URLNormalizer](file:///c:/Users/brach/OneDrive/Documents/Project-02/include/URLNormalizer.h)
  - [URLValidator](file:///c:/Users/brach/OneDrive/Documents/Project-02/include/URLValidator.h)
  - [ConfigManager](file:///c:/Users/brach/OneDrive/Documents/Project-02/include/configmanager.h)
  - [Logger](file:///c:/Users/brach/OneDrive/Documents/Project-02/include/logger.h)

---------------------------------------------------------

### Section 4 — Learning Reflection

Today's work highlighted the critical value of building utility components with clear, isolated scopes. Handling dirty real-world data like malformed query parameters and path traversals (`../`) in [URLNormalizer](file:///c:/Users/brach/OneDrive/Documents/Project-02/include/URLNormalizer.h) requires systematic parsing rules rather than loose heuristics. I also learned about thread safety in logging. Under multithreaded network fetch operations, concurrent writes to console output and log files would overlap and corrupt the logs. Using a scoped mutex lock (`std::lock_guard` with a private `std::mutex`) inside the [Logger](file:///c:/Users/brach/OneDrive/Documents/Project-02/include/logger.h) serialization calls was crucial to preserve readable output. Finally, writing defensive code in [ConfigManager](file:///c:/Users/brach/OneDrive/Documents/Project-02/include/configmanager.h) with safe fallbacks ensures our application remains resilient even if configuration entries are malformed or missing.
