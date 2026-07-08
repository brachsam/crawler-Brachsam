# PageFetcher

## What it does

Turns a URL into rendered HTML. That's it. Everything about how — browser,
network stuff, waiting for JavaScript — is hidden behind one function.

```cpp
class PageFetcher {
public:
    string fetchPage(string url);  // returns rendered HTML, or empty string on failure
};
```

The crawler never knows or cares what's inside this class.

## Why a headless browser instead of a plain HTTP fetch

A plain request only gets the HTML the server sends before any JavaScript
runs. Some sites build their actual content after the page loads, using
JS. A headless browser opens the page for real, waits for it to finish,
and gives back the HTML after JS has run. So it works on both plain sites
and JS-heavy ones.

A headless browser actually opens the page like a real browser would, waits
for it to finish loading, and then gives us the HTML as it exists after
JavaScript has run. This means our crawler works on both plain HTML sites
and JS-heavy ones.

## Failure handling
Returns "" when anything goes wrong — bad URL, timeout, server error,
page that never loads. The Crawler checks for empty and skips, doesn't
crash.

## Why its own class

If I ever swap the browser out, or drop back to a plain fetch for speed,
this is the only file that changes. Nothing else in the crawler needs to
know or care.

## What changed since the first draft

The politeness delay (waiting a second between fetches) used to be
mentioned as something that "lives in the Crawler loop." I moved that into
its own class instead — CrawlScheduler. PageFetcher still doesn't know
anything about pacing or delays, it just fetches whatever URL it's given,
whenever it's called. The Crawler now calls CrawlScheduler right before it
calls PageFetcher, keeping the two jobs (waiting vs. fetching) separate.