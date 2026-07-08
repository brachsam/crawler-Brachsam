### Date: July 7 2026

### Duration: 2 Hours (12:00 PM – 1:00 PM & 2:00 PM – 3:00 PM)

## Goal:
Research modern web crawling techniques required to support JavaScript-driven websites.

## Problem Encountered:
Traditional HTTP-based page fetching is insufficient for modern websites because much of the webpage content is generated dynamically through JavaScript and API calls.

## What I Tried:
Studied how modern browsers process webpages.

#### Discussed:

- Difference between cURL and Browser-based fetching.
- Headless Browser architecture.
- JavaScript execution.
- JSON/API requests triggered by JavaScript.
- DOM rendering process.

Understood how browsers automatically:

- Download HTML
- Download CSS
- Download JavaScript
- Fetch JSON/API responses
- Execute JavaScript
- Generate the final rendered HTML

Also explored how browser rendering differs from simply downloading raw HTML.

## Outcome:
Decided to implement PageFetcher using a Headless Browser instead of cURL so that the crawler can correctly crawl JavaScript-rendered webpages.

Updated the architecture to treat the browser as an internal implementation detail of PageFetcher while keeping the rest of the crawler independent from browser-specific logic.
