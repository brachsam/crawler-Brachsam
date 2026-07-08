# HTMLParser

## What it does

Takes HTML text and pulls links out of it. Not naming it "LinkExtractor"
on purpose — later on (Project 03) I'll want to pull other stuff out of
the same HTML too, like plain text, not just links.

## Why this has to know nothing about the crawler

This is the one component with a hard rule: it must not know Frontier,
SeenStore, HashMap, Queue, or Crawler even exist. It only understands HTML
text in, structured data out.

The reason is reuse. Project 03's Indexer is going to want to strip tags and
pull out words from the exact same stored HTML we're producing now. If
HTMLParser is written generically now, Project 03 reuses it directly instead
of writing its own tag-stripping logic from scratch.

## Public interface (for this project)

```cpp
class HTMLParser {
public:
    // returns every href value found in <a> tags, unresolved and unfiltered
    DynamicArray<string> extractLinks(string html);
};
```
Returns raw hrefs exactly as they appear, unresolved, unfiltered. No
skipping fragments or javascript: links here — that's URLNormalizer and
URLValidator's job, downstream.

## How it actually finds the links - using KMP
I'm using the Knuth-Morris-Pratt algorithm to search the HTML text for the
patterns <a and href=. Once href= is found, I grab everything
between the quotes right after it.

Why KMP and not just a plain string search: a plain search can waste time
re-scanning characters it already looked at when a partial match fails
partway through (like matching "hre" then hitting something that isn't
"f"). KMP builds a small lookup table upfront so it never backs up — it
always moves forward through the HTML exactly once. That keeps the whole
scan at a clean O(H), where H is the length of the HTML, no matter how the
text is shaped.

I looked at a few other options before landing on this (regex, a full
tokenizer/FSM, Boyer-Moore, Rabin-Karp). KMP won out because it's simple
enough to actually implement correctly without STL, and its worst-case
guarantee is exactly what I need — no surprise slowdowns on weird HTML.


## Future extractors (not built now, just planned for)

Because HTMLParser is generic, later projects can add sibling extractors
without touching this class:

- `TextExtractor` — strips tags, keeps the words (what Project 03 needs)
- `ImageExtractor` — pulls image URLs from `<img>` tags
- `MetadataExtractor` — pulls `<title>`, meta description, etc.

## Future extractors (planned, not built yet)
Since this stays generic, later I can add without touching this file:
TextExtractor — strips tags, keeps words
ImageExtractor — pulls image URLs
MetadataExtractor — pulls title, meta tags


## What's not decided yet
Whether single-quoted hrefs (href='...' instead of href="...") need
handling. Starting with double quotes since that's what I've seen on the
test sites, will add single-quote support if testing shows I need it.
