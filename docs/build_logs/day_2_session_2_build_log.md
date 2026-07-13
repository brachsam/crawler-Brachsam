### Date: July 8 2026

### Duration: 2 Hours (12:00 PM – 2:00 PM)

## Goal:
Implement the URLValidator component to identify and filter out unsupported or invalid URLs.

## Problem Encountered:
Certain links such as mailto or empty hashes would slip through, and parsing logic had to be case-insensitive to handle schemes like `mailto:` vs `MAILTO:`.

## What I Tried:
Structured `URLValidator` with modular checks instead of a single massive boolean logic chain for easier debugging. Wrote helper functions: `isJavascriptLink`, `isMailtoLink`, `isBareFragment` (retaining path but discarding hash-only references), and `isHttpOrHttps`. Added string manipulation utilities such as `toLower` for case-insensitive validation.

Discussed:
- Filtering javascript: and mailto: protocols.
- Fragment checking and stripping bare fragments.
- Rejecting non-http/https protocols.
- Using helper functions for validation rules to improve readability.

## Outcome:
Created `URLValidator::isValid` which correctly filters out javascript:, mailto:, empty links, bare fragments, and non-http/https protocols.
