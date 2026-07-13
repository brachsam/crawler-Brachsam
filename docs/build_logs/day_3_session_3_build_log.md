### Date: July 9 2026

### Duration: 2 Hours 30 Minutes (2:30 PM – 5:00 PM)

## Goal:
Implement the basic RobotsHandler parser to identify and parse allowed paths in robots.txt.

## Problem Encountered:
Extracting host names and paths from mixed URLs is tricky because protocols or trailing segments can cause parsing issues if not split properly.

## What I Tried:
Wrote helper `extractHost()` to get base host strings (e.g. `example.com`), and `extractPath()` to retrieve the path portion. Implemented `extractDisallowedPaths()` to read raw robots.txt files, identify wildcard rules under `User-agent: *`, and capture subsequent `Disallow:` path strings.

Discussed:
- Protocol separation and host identification logic.
- Parsing line-by-line using positions and removing leading whitespace.
- Restricting rules extraction to wildcard user-agents (`*`).
- Accumulating disallowed path directories into a structured buffer.

## Outcome:
Completed host/path extraction and robots.txt wildcard rule filtering. Successfully parsed Disallow paths from simulated files into internal strings.
