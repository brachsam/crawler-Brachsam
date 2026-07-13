### Date: July 8 2026

### Duration: 2 Hours (2:30 PM – 4:30 PM)

## Goal:
Implement the ConfigManager component to load and parse settings from a properties-style configuration file.

## Problem Encountered:
Configuration files can have extra whitespace, empty lines, and comments starting with `#`, which can break parser logic if not properly trimmed and handled.

## What I Tried:
Built parsing logic reading a file line-by-line using `std::ifstream` and `std::getline`. Implemented `trim` to remove leading/trailing spaces and ignore empty lines/comments. Extracted keys and values by splitting at `=` and stored them in Project-01's HashMap collection. Created accessors like `getString`, `getInt`, `getBool`, `requireString`, and `requireInt` with safe type conversion fallback.

Discussed:
- Properties file parsing format (`key = value`).
- Trimming whitespace from parsed tokens.
- Ignoring comments and blank lines.
- Safe type conversion and fallback default values.
- Throwing exceptions for missing required configurations.

## Outcome:
Successfully built the ConfigManager class that reads configuration options, handles invalid lines gracefully, and provides fallback values if keys are missing.
