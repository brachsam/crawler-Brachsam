### Date: July 18, 2026

### Duration: 1 Hour (12:00 PM – 01:00 PM)

## Goal:
Formulate the folder structure for the Indexer module, configure compilation targets in CMake, and implement the basic word Normalizer component.

## Problem Encountered:
We need to add the Indexer to the existing codebase in a way that respects standard conventions and isolates compilation targets. Putting headers and source files in flat, single directories would clutter the root directories and make future separation of the Indexer into a standalone binary difficult.

## What I Tried:
I mapped out a clean design layout:
- All indexer header files reside in `include/Indexer/`
- All indexer source files reside in `src/Indexer/`

I modified `CMakeLists.txt` to register these source files under a new executable target `Indexer` and linked it with our custom `Collections` and `SQLite3` libraries. I then implemented the `Normalizer` class. The normalizer loops through characters in a word to convert them to lowercase and trims surrounding punctuation elements (like quotes, commas, brackets) from both ends of the string.

## Outcome:
Successfully configured CMake to register the new `Indexer` target and created the `Normalizer` component.

## Next Step:
Implement character-based tokenization and stop word filtering logic.
