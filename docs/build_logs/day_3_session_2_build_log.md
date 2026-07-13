### Date: July 9 2026

### Duration: 2 Hours (12:00 PM – 2:00 PM)

## Goal:
Implement the SeenStore component using a hash table to ensure URLs are not visited more than once.

## Problem Encountered:
Checking duplicates against thousands of visited URLs could become slow unless mapped to an average O(1) container structure.

## What I Tried:
Wrapped Project-01's HashMap collection inside the SeenStore class. Implemented `markSeen(url)` to record a URL by mapping it to a boolean flag, and `isSeen(url)` to check for existence in the HashMap via `exists()`. Provided `size()` method to monitor the count of stored visited links.

Discussed:
- Average O(1) duplicate checking complexity.
- Mapping unique string URLs to boolean values in the internal HashMap.
- Interface separation so crawl orchestration doesn't access the raw HashMap.
- Preventing memory growth optimizations by tracking exact sizes.

## Outcome:
Developed a high-performance SeenStore check layer, allowing quick duplicate resolutions in the crawl loop with average O(1) lookup times.
