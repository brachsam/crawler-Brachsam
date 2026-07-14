### Date: July 11, 2026

### Duration: 2 Hours (04:00 PM – 06:00 PM)

## Goal:
Connect the integrated components into the main crawler pipeline and begin end-to-end testing of the full crawl loop.

## Problem Encountered:
During multi-page crawl testing, circular link references (where page A links to page B, and page B links back to page A) caused redundant page fetches and infinite crawl loops. This happened because the visited URL check (`SeenStore`) was being called after fetching instead of before enqueuing.

## What I Tried:
I integrated all the implemented modules (Frontier, SeenStore, PageFetcher, HTMLParser, PageStorage) into the main crawler execution loop inside `Crawler.cpp`. I updated the flow so that a discovered URL is checked against `SeenStore` and marked as seen *before* it is enqueued to `Frontier`, preventing duplicate entries. I ran end-to-end testing of the crawl loop using a local mock site setup. I verified that the crawler successfully fetches the seed page, stores it, parses its links, validates and normalizes them, enqueues the child links, and terminates when it reaches `max_depth` (which was updated to 2).

## Outcome:
Successfully verified basic interaction and flow between all crawler components. The integrated crawler successfully executes depth-limited multi-page crawling on mock environments, though further robustness testing is needed.

## Next Step:
Implement domain filtering limits in the validation step and optimize performance bottlenecks under concurrent page loads.
