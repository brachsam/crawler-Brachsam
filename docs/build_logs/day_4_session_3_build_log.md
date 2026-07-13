### Date: July 10 2026

### Duration: 2 Hours 30 Minutes (2:30 PM – 5:00 PM)

## Goal:
Verify the standalone behavior and correctness of basic utility modules (ConfigManager, Logger, URLNormalizer, URLValidator).

## Problem Encountered:
Integrating various parts with Project-01 Collections library collections (like custom HashMap and LinkedList) triggered syntax warnings that required proper type specifications.

## What I Tried:
Constructed mock configurations in `config.txt` and verified that ConfigManager trims extra spaces and loads variables correctly. Validated a test suite of URLs through URLNormalizer and URLValidator to check edge cases (empty strings, case variants, deep nesting paths, mailto exceptions). Ran tests on the Logger to ensure it correctly writes logs to console and file under load.

Discussed:
- Setup properties files and loading test keys.
- Correctly identifying relative paths containing directory traversals (`../`).
- Scheme normalization to handle capitalization changes.
- Multithreaded logger synchronization and file write consistency.

## Outcome:
Successfully verified the correctness of baseline utility libraries. All standalone checks for validation, configuration, normalization, and logging succeeded.
