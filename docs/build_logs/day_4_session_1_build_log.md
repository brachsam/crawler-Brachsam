### Date: July 10 2026

### Duration: 2 Hours (9:30 AM – 11:30 AM)

## Goal:
Initialize the PageStorage database module using sqlite3 to store successfully crawled pages.

## Problem Encountered:
Creating table schemas manually or handling setup errors dynamically on launch requires automated execution utilities and custom exceptions.

## What I Tried:
Integrated the C++ `sqlite3` library. Implemented the constructor to call `sqlite3_open` and initialize database handles. Created a generic helper `execOrThrow()` to parse SQL statements and propagate errors. Structured a database schema setup query creating the `pages` table containing columns: `id` (autoincrement), `url` (unique), `depth` (integer), and `html` (text).

Discussed:
- SQLite db connection handling and resource deletion in the destructor.
- Executing SQL script schemas automatically on system startup.
- Safe pointer cleanup for sql error strings.
- Defining appropriate column data types and unique constraints on the URL.

## Outcome:
Successfully implemented PageStorage connection management and schema generation, providing standard storage handles for downloaded HTML payloads.
