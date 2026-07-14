### Date: July 11, 2026

### Duration: 2 Hours (02:00 PM – 04:00 PM)

## Goal:
Integrate HTMLParser with the existing crawler modules (PageStorage, URLNormalizer, URLValidator).

## Problem Encountered:
Extracted raw links often contain relative paths, fragments (`#`), or invalid schemes (such as `javascript:void(0)` and `mailto:` links). Passing these directly into the C++ crawler loops caused database write failures in `PageStorage` and enqueued garbage endpoints into the queue.

## What I Tried:
I wired the HTMLParser output to feed into the URLNormalizer. This converts relative paths into fully qualified URLs and strips off layout fragment hashes. Next, I updated the validation step to run every normalized link through `URLValidator`, which filters out mailto links, javascript actions, and invalid protocol schemes. Finally, I connected the parser step to the main database persistence logic so that the crawler saves the successfully fetched page HTML payload and depth information into `PageStorage` before extracting its links for subsequent crawls.

## Outcome:
Connected HTMLParser with URLNormalizer, URLValidator, and PageStorage, ensuring only fully resolved, valid URLs are processed and stored.

## Next Step:
Connect the integrated components into the main crawler pipeline and begin end-to-end testing of the full crawl loop.
