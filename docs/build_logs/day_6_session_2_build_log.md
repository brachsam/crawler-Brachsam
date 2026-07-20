### Date: July 18, 2026

### Duration: 2 Hours (02:00 PM – 04:00 PM)

## Goal:
Implement clean string tokenization, stop word filtering, and the HTML text extraction utility.

## Problem Encountered:
Standard C++ libraries like `<regex>` can introduce heavy parsing overhead and binary bloat. Additionally, HTML pages contain extensive formatting scripts (JavaScript) and CSS styles, which are noise for search queries and must be completely skipped during text extraction.

## What I Tried:
I built the `Tokenizer` using character-by-character classification to split text on whitespace characters, returning a custom `DynamicArray<std::string>`. I implemented `StopWordFilter` by pre-loading 80 common English stop words into the custom `HashMap<std::string, bool>` for O(1) existence checks. For `TextExtractor`, I wrote a single-pass HTML tag-stripping state machine. It removes content between '<' and '>' and has specialized rules to skip all text content inside `<script>` and `<style>` blocks.

## Outcome:
Completed the core utility components (`Tokenizer`, `StopWordFilter`, and `TextExtractor`) using our custom Collections library without using STL containers.

## Next Step:
Implement page-level term frequency counting and the global inverted index builder.
