# PageStorage

## What it does

Holds every page we've crawled — url, depth, raw html — and lets other code
(mainly Project 03's Indexer, later) read it back

## Why a database instead of DynamicArray/HashMap in memory

Storing raw HTML for hundreds or thousands of pages entirely in RAM works fine for small test crawls, but doesn't survive the program ending, and can
run into memory limits on bigger crawls. A database persists to disk, so:
- the crawl survives a crash or restart
- Project 03 can run later, as a completely separate program, and still read everything we crawled
- we're not holding gigabytes of HTML in memory at once

We are using database to store the information instead of hash map that do add complexity instead of simplicity but the data in saved permanently. This is very important for project-03.

## Public interface (must match what Project 03 expects)

```cpp
class PageStorage {
public:
    void   storePage(string url, string html, int depth);
    string getPage(string url);
    bool   hasPage(string url);
    string getURLByID(int id);
    int    pageCount();
};
```

Two different access patterns need to both work:

- **By URL** — `getPage(url)` / `hasPage(url)` — used when the crawler wants to check or fetch a specific page.
- **Sequential by ID** — `getURLByID(id)` / `pageCount()` - because here the pages are given a permanent serial ID (1,2,3,....) as project - 03's indexer loops from 1 to pageCount() so that it can be accessed easily.

## What we store per page

| Field | Why |
|-------|-----|
| id    | stable numeric key, assigned in insertion order |
| url   | the page's canonical (normalized) address |
| depth | how many hops from the seed — used later as a relevance signal |
| html  | the raw rendered HTML, tags and all — Project 03 strips tags itself |

## Why raw HTML, not extracted text

PageStorage saves the complete HTML page, including all HTML tags. It does not extract or save only the text because text extraction is the responsibility of Project 03. Keeping the original HTML preserves all the page's information and structure, so future projects can decide how to use it. If we removed the tags now, we would lose useful information such as headings, links, and formatting.

## What's NOT decided here

- Exact database and schema — an implementation detail underneath these five
methods, can change later without touching anything else in the crawler.
