# URLValidator

## What it does

Looks at a (already normalized) URL and decides one thing: is this even worth trying to crawl? Yes or no.

```cpp
class URLValidator {
public:
    bool isValid(string url);
};
```

## Why this is separate from URLNormalizer

Normalizing reshapes a URL so it matches other equal URLs. Validating decides if it's even a real page worth fetching. A URL can be perfectly normalized and still be junk.Two different jobs, two different reasons to change, so two different classes.

## What gets rejected

- `mailto:` links — opens an email client, not a page
- Fragment-only links (`#introduction`) with no path — this is just a
  scroll position on the current page, not a different page. (Fragments
  attached to otherwise-real URLs, like `/about#team`, get their fragment
  stripped by URLNormalizer before this check even runs, so this rule
  mainly protects against a bare `#something` link.)
- Anything that isn't `http://` or `https://` after normalization (covers
  `ftp:`, `tel:`, `data:`, and similar schemes we can't fetch anyway)

## What passes

- Any absolute `http://` or `https://` URL after normalization.

## Where this runs in the crawl loop

Order matters here:

```
raw href from HTMLParser
   -> URLNormalizer.normalize()   (make it canonical, resolve relative paths)
   -> URLValidator.isValid()      (reject junk before it wastes a SeenStore/Frontier slot)
   -> SeenStore.contains()        (only real, valid URLs get this far)
```

We validate after normalizing, not before, because a relative URL like
`/about` isn't recognizable as valid or invalid until it's been resolved
against its base URL — before resolution we don't even know if it's an
`http` link or something else.

## Why RobotsHandler isn't part of this class

Could've thrown the robots.txt check in here too, since both are about
"should I crawl this." Kept them separate because they check completely
different things — URLValidator is about the URL's shape (is it even a
real link), RobotsHandler is about permission (is the site owner OK with
me crawling this). If robots.txt rules ever get more complicated later
(different rules per user-agent, crawl-delay directives, etc), that
complexity stays contained in its own file instead of bloating this one.