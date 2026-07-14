### Date: July 11, 2026

### Duration: 2 Hours (09:00 AM – 11:00 AM)

## Goal:
Research and select an efficient string searching algorithm for locating anchor tags and links within HTML documents, and begin designing the HTMLParser component.

## Problem Encountered:
HTML documents can be very large, containing millions of characters. Using a naive character-by-character search leads to frequent backtracks and re-comparisons, which makes the parser highly inefficient when processing large, complex webpages.

## What I Tried:
I studied and compared several string searching algorithms to determine which would be the best fit for our HTMLParser:
- Naive String Matching: While simple, it requires comparing the same characters multiple times, making its running time proportional to the HTML size multiplied by the pattern length.
- Rabin-Karp: It is efficient on average by using hashing, but hash collisions can slow it down to quadratic time in the worst case.
- Boyer-Moore: It is very fast because it can skip large blocks of characters, but it requires additional preprocessing tables and is complex to implement robustly.
- Knuth-Morris-Pratt (KMP): It preprocesses the search pattern once to build a prefix lookup table (failure function), then scans the HTML text only once. This guarantees linear performance proportional to the HTML and pattern sizes, completely avoiding unnecessary re-comparisons. 

I decided that KMP is the ideal algorithm for repeatedly searching patterns like `"<a"` and `"href="` in large HTML source files. I clarified that KMP is not the HTML parser itself; the HTMLParser manages the high-level structural traversal, and KMP is used internally as a matching utility to locate raw tags. I drafted the C++ interface declarations for both `HTMLParser` and the `KMP` helper.

## Outcome:
Selected KMP as our pattern-matching engine due to its predictable linear execution time and completed the design declarations for the new parsing components.

## Next Step:
Implement the KMP search algorithm and write the HTMLParser logic to extract raw link strings.
