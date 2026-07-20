### Date: July 18, 2026

### Duration: 2 Hours (04:00 PM – 06:00 PM)

## Goal:
Implement page-level term frequency calculations, the global postings list builder, and the database reader wrapper.

## Problem Encountered:
When aggregating terms into a global index in memory, we need to map term strings to lists of document postings. We must make sure that `InvertedIndexBuilder` merges document term frequencies using our custom `HashMap` and `DynamicArray` classes without causing template instantiation problems or memory leaks.

## What I Tried:
I created the `TermFrequencyCounter` component to count frequencies of clean tokens on a page. I implemented `InvertedIndexBuilder` with a nested `Posting` struct, mapping unique terms to a dynamic list of postings. I also wrote `DocumentReader` wrapping the existing `PageStorage` class. This allows us to retrieve pages and count tables from `crawler.db` without changing the crawler source code.

## Outcome:
Constructed the core in-memory global indexing logic and the database reader component (completing approximately 70% of the Indexer module design).

## Next Step:
Implement index persistence to a separate database file and orchestrate the full pipeline.
