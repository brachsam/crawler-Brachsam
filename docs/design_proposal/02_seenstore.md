# SeenStore

## What it does

The SeenStore answers exactly one question, fast: "have we already visited
this URL?" That's it. It doesn't care about depth, doesn't care about HTML,
doesn't care about anything except: seen it or not.

## Why this component even exists

Without it, the crawler loops forever the second two pages link to each
other. Every time I finish a URL, I mark it seen. Every time I'm about to
queue a URL, I check if it's seen first. That check is what makes the
whole thing eventually stop.

## Why HashMap and not LinkedList

Every single link on every single page gets checked against this. A
LinkedList means walking the whole list to check one URL — O(n) per
check. At 10,000 URLs that's up to 100 million comparisons total. The
crawler basically freezes.

A HashMap hashes the URL once and jumps right to the bucket. O(1) average,
whether I've seen 200 URLs or 200,000. This is the actual reason the
HashMap is required here, not just because we happen to have one from
Project 01.

## What's underneath

Wraps our Project 01 HashMap<string, bool> (or just HashMap<string, char> if
we don't need a real bool type — value doesn't matter, only key presence
does). The crawler and every other component never see the HashMap directly.
They only see `SeenStore`.

## Public interface

```cpp
class SeenStore {
public:
    bool contains(string url);   // O(1) average — is this URL already seen?
    void markVisited(string url); // O(1) average — record it
    int  count();                 // how many URLs seen so far
    void clear();                 // wipe it (mainly useful for testing)
};
```

## Important detail: normalization happens before this, not inside this

The SeenStore treats "https://Example.com/Page" and "https://example.com/page"
as two completely different strings, because that's just what a HashMap
does — it hashes exact bytes. If we don't normalize URLs before calling
`contains()` / `markVisited()`, the same page gets crawled multiple times
under slightly different spellings.

That's why URLNormalizer runs before every SeenStore check (see
`05_urlnormalizer.md`). The SeenStore's job stays simple: exact-string
tracking. Making URLs comparable is somebody else's job.

We rely on our own Project 01 HashMap's resizing (load factor 0.7, doubling
on resize) to keep lookups close to O(1) even as the crawl grows into the
thousands of URLs.

## What changed since the first draft

Nothing structural changed here. The only new thing is that a URL now has
to also pass RobotsHandler before it even reaches SeenStore — so SeenStore
sees slightly fewer URLs than before, but its own logic didn't change at
all
