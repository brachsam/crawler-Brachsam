# Daily Journal Day 4

### Section 1 — Specific Bug

When saving page payloads to the database, the raw SQL insertion strings were broken by HTML source code containing single and double quotes. In my initial implementation, this resulted in SQL syntax errors or database write failures. To resolve this syntax security bug, I refactored the [PageStorage](file:///c:/Users/brach/OneDrive/Documents/Project-02/include/pagestorage.h) class to use parameterized SQLite prepared statements (`sqlite3_prepare_v2`) and bound the arguments using `sqlite3_bind_text` and `sqlite3_bind_int`. This safely sanitized the dynamic HTML input and prevented any potential SQL injection vulnerabilities.

---------------------------------------------------------

### Section 2 — Failed Attempt

During initial integration testing of the database write loop, we hit database lock errors when trying to write consecutive mock pages into SQLite in quick succession. The database was throwing lock errors because every write was triggering an independent database connection cycle or overlapping disk operations. I initially thought of introducing random thread sleep delays. Instead, I realized that properly structuring the SQLite connection persistence and ensuring database handles are cleanly finalized via `sqlite3_finalize` immediately after each query resolved the lock contentions without adding artificial latency.

---------------------------------------------------------

### Section 3 — Code Reference

* **Git Commit:** `fc77bd3`, `ae7c638`, `8eb2b83`, `4f8475b`
* **Major Files Modified:**
  - [pagestorage.h](file:///c:/Users/brach/OneDrive/Documents/Project-02/include/pagestorage.h) & [pagestorage.cpp](file:///c:/Users/brach/OneDrive/Documents/Project-02/src/pagestorage.cpp)
  - [CMakeLists.txt](file:///c:/Users/brach/OneDrive/Documents/Project-02/CMakeLists.txt)
  - [config.txt](file:///c:/Users/brach/OneDrive/Documents/Project-02/config.txt)
* **Components Implemented:**
  - [PageStorage](file:///c:/Users/brach/OneDrive/Documents/Project-02/include/pagestorage.h)
  - SQLite Integration Layer

---------------------------------------------------------

### Section 4 — Learning Reflection

Today's database work highlighted the importance of using Resource Acquisition Is Initialization (RAII) when wrapping C libraries in C++. Managing C-style pointers (`sqlite3*` and `sqlite3_stmt*`) requires careful cleanup in destructors and error blocks to prevent memory leaks and dangling file descriptors. I also learned the value of utilizing standard SQL clauses like `ON CONFLICT(url) DO UPDATE` to gracefully merge duplicate crawls instead of querying the database first. Integrating all the utility modules with Project-01 Collections library collections also triggered template syntax warnings, reminding me that strict type checking and explicit template parameters are essential for compiler-independent C++ code.
