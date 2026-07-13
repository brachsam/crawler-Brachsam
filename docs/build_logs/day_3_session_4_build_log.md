### Date: July 9 2026

### Duration: 2 Hours (5:30 PM – 7:30 PM)

## Goal:
Implement a caching system in RobotsHandler to prevent repeatedly fetching and parsing robots.txt for the same host.

## Problem Encountered:
Downloading and parsing robots.txt on every single page fetch adds a massive network and CPU overhead.

## What I Tried:
Implemented a cache structure using static sizes `MAX_CACHED_HOSTS` (32) to track host domains and their associated disallow rules. Wrote cache helpers: `isHostCached`, `getCachedRules`, and `addToCache`. Created the `pathIsDisallowed()` validation method. Created a temporary network stub for `fetchRobotsTxt` which returns an empty string to allow crawling in local runs.

Discussed:
- In-memory cache limits and buffer indexing.
- Matching paths against multi-line disallow rules.
- Network fetch stubs for robots.txt files.
- Graceful default policies when robots.txt is missing.

## Outcome:
Successfully added caching to RobotsHandler, ensuring each host's robots.txt rules are parsed only once per crawl session.
