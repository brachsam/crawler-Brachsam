# Frontier

## What it does

Frontier holds the URLs I still need to crawl. Every entry has two things
stuck together — the URL, and how deep it is (how many clicks away from
the start page). These two never get separated. If they did, I'd lose
track of when to stop going deeper.

## Why a queue, and why FIFO

I want breadth-first crawling. Finish depth 0 first, then depth 1, then
depth 2, then depth 3, then stop. A normal queue (first in, first out)
gives me that for free.

If I used a stack instead, the crawler would go deep down one path before
coming back — that's depth-first. Not what I want here. Breadth-first
gives a more even picture of the site, easier to reason about and
benchmark.

## What's underneath

My Project 01 LinkedList, wrapped in a Frontier class. Nothing outside
this file ever touches the LinkedList directly, only push() and pop().

Why LinkedList and not DynamicArray:

- A queue's two operations are: add to the back, remove from the front.
- Removing from the front of a DynamicArray means shifting every remaining
  element left — that's O(n) every single pop.
- Removing from the front of a LinkedList is O(1) if we keep a head pointer
  and update it, since it's just "make head = head->next" and free the old
  node.
- We are already building this LinkedList with `isDoubly` support, so we get
  the head pointer basically for free.

So LinkedList is the right structural fit here, not just "the one we already
have."

## What a single entry looks like

```cpp
struct URLDepth {
    string url;
    int depth;
};
```

Each LinkedList node stores one `URLDepth`. Nothing fancy — this struct is
the thing that moves through the queue as a single unit, so depth never gets
lost or mismatched with the wrong URL.

## Public interface

```cpp
class Frontier {
public:
    void push(URLDepth item);   // add to the back
    URLDepth pop();              // remove and return from the front
    bool empty();
    int size();
};
```

## One thing I added later — robots.txt check happens before push, not here

Before a URL gets pushed into the Frontier, it now also has to pass a
robots.txt check (RobotsHandler). Frontier itself doesn't know anything
about robots.txt, same way it doesn't know about maxDepth — it just stores
whatever it's handed. All the filtering (valid? seen? allowed by
robots.txt? within depth?) happens before the push call, not inside
Frontier.