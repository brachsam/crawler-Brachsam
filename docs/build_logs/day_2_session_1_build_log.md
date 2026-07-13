### Date: July 8 2026

### Duration: 2 Hours (9:30 AM – 11:30 AM)

## Goal:
Implement the URLNormalizer component to clean up and standardize URLs before the crawler processes them.

## Problem Encountered:
Handling absolute vs relative URLs and properly resolving base paths (like `../` and root-relative paths starting with `/`) without external URI parser libraries is complex and prone to path formatting errors.

## What I Tried:
Implemented the `normalize()` method that sequentializes normalization rules. Designed helper functions to lowercase the scheme/host, remove fragments (the `#` part), remove trailing slashes when redundant, and remove empty trailing queries.

Discussed:
- Difference between absolute and relative URLs.
- Scheme and host normalization (case-insensitivity).
- Resolving relative paths (root-relative, grandparent paths, sibling paths).
- Removing fragments and empty queries.

## Outcome:
Successfully implemented a robust URLNormalizer class that standardizes URLs and resolves relative links. Verified that base URLs are parsed correctly to form valid, absolute destinations.
