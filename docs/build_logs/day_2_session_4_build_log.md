### Date: July 8 2026

### Duration: 2 Hours 30 Minutes (5:00 PM – 7:30 PM)

## Goal:
Implement a thread-safe Logger component for tracking crawling activities.

## Problem Encountered:
Multithreaded execution or interleaved console prints could corrupt output, requiring mutex locks to serialize screen and file writes.

## What I Tried:
Implemented the `Logger` class with support for logging to both console (`std::cout`/`std::cerr`) and a file. Added log levels (`LDEBUG`, `LINFO`, `LWARN`, `LERROR`) and filtered out logs below the configured minimum level. Used `std::mutex` and `std::lock_guard` to enforce thread safety during write operations. Added timestamp formatting using `std::put_time`.

Discussed:
- Thread-safe console and file output streams.
- Log level filtering (LDEBUG, LINFO, LWARN, LERROR).
- Format strings with timestamp and log level tags.
- Safe closing of file handles inside the destructor.

## Outcome:
Completed the Logger component, ensuring structured outputs with levels and timestamps. The logger is fully safe for multithreaded network fetch operations.
