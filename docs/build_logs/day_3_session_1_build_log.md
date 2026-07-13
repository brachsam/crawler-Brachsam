### Date: July 9 2026

### Duration: 2 Hours (9:30 AM – 11:30 AM)

## Goal:
Implement the Frontier component to manage URLs waiting to be crawled in FIFO order.

## Problem Encountered:
Standard arrays are inefficient (O(N)) for removing items from the front of the queue, making a linked list the correct structure.

## What I Tried:
Wrapped Project-01's LinkedList library inside the `Frontier` class. Implemented `enqueue()` which inserts at the back of the queue, and `dequeue()` which reads the front item before deleting it. Designed a custom `URLDepth` struct that bundles URL strings with integer depths to prevent data mismatch.

Discussed:
- BFS vs DFS queue structures.
- Why LinkedList is O(1) for enqueue and dequeue compared to DynamicArray O(N) shifts.
- Interfacing clean queue operations (`enqueue`, `dequeue`, `isEmpty`, `size`) and isolating the underlying list.
- Handling empty frontier exceptions during dequeue.

## Outcome:
Successfully implemented the Frontier queue, wrapping the Project-01 LinkedList and providing O(1) complexity for front/back queue operations.
