# URLNormalizer

## What it does

Takes a URL and rewrites it into one consistent, canonical form. Two URLs
that point at the same page should come out of this function looking
identical, even if they didn't go in looking identical.

```cpp
class URLNormalizer {
public:
    string normalize(string url, string baseURL);
};
```

`baseURL` is the URL of the page we found this link on — needed because
relative links (`/about`) only make sense once we know what they're relative
to.

## Why this matters

These four all point at the same page:

```
https://example.com/page
https://example.com/page/
HTTPS://EXAMPLE.COM/PAGE
https://example.com/page?
```


If we don't normalize before checking SeenStore, the crawler treats these as
four different URLs, crawls the same page four times, and stores four
duplicate copies. Normalization happens once, right after we pull a raw href
out of the HTML, before the URL ever touches SeenStore or the Frontier.

## What normalization steps we're doing (minimum required set)

- Lowercase the scheme and host (`HTTPS://EXAMPLE.COM` -> `https://example.com`)
- Convert relative URLs to absolute, using the base URL of the page we found
  them on (`/about` on `https://example.com` -> `https://example.com/about`)
- Remove trailing slash when it's redundant (`/page/` -> `/page`)
- Remove empty query strings (`/page?` -> `/page`)
- Remove fragments (`/page#section` -> `/page`) — since a fragment points at
  a spot on the same page, not a different page

## What we're explicitly NOT handling (and why)

- We're not sorting or de-duplicating query parameters
  (`?a=1&b=2` vs `?b=2&a=1`) — this is a real normalization case in
  production crawlers, but it's rare on the test sites we're using and adds
  parsing complexity we don't need for this project's scope.
- We're not stripping tracking parameters like `?utm_source=...` — same
  reasoning, out of scope for now, may add later.

## Important: this does not remove duplicates

URLNormalizer's only job is to make two equivalent URLs *look* the same. It
does not check SeenStore, does not decide if a URL has been crawled before.
That check happens right after, in the crawl loop, by asking SeenStore
directly. Keeping these separate means URLNormalizer stays reusable outside
this specific crawler — some other project might want normalized URLs
without wanting duplicate-tracking behavior attached.