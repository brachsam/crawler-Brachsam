### Date: July 19, 2026

### Duration: 3 Hours (09:00 AM – 12:00 PM)

## Goal:
Implement index storage persistence, complete the orchestrator loop, compile the target executables, and verify execution on the crawled database.

## Problem Encountered:
1. Compiling the target failed inside `hmap.h` because `HashMap`'s internal debug `print()` function requires the value type to support the `operator<<` stream insertion operator, which `DynamicArray<Posting>` did not have.
2. Running the indexer on the database crashed around page ID 480 due to a `PageNotFoundException`. The crawler database had gaps in the auto-increment `id` column of the `pages` table (e.g. ID 480 was missing), causing sequential page lookup loops to fail.

## What I Tried:
1. I added inline stream insertion `operator<<` definitions for both the `Posting` struct and `DynamicArray<Posting>` in `InvertedIndexBuilder.h`, which resolved the compiler template instantiation errors.
2. I modified `DocumentReader` to query the database directly for existing IDs (`SELECT id FROM pages ORDER BY id ASC`) and modified the `Indexer` run loop to iterate over these specific IDs instead of counting sequentially, bypassing all database ID gaps.
3. I implemented `IndexStorage` to open `indexer.db`, create `documents` and `term_postings` tables, and write all records inside an SQLite transaction (`BEGIN` / `COMMIT`) for rapid indexing.
4. I wrote `indexer_main.cpp` and completed the `Indexer` orchestrator class to wire everything together.

## Outcome:
Successfully compiled the new standalone `Indexer.exe` and verified that running it on the raw crawled data successfully populates `indexer.db` with term postings and page URL mappings.

## Next Step:
Add thorough unit tests for tokenization, normalization, tag stripping, and database integration.
