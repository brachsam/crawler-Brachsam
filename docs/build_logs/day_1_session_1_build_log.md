### Date: July 7 2026

### Duration: 2 Hours 30 Minutes (9:30 AM – 12:00 PM)

## Goal:
Finalize the high-level architecture of the Web Crawler and define the responsibilities of each major component.

## Problem Encountered:
The crawler architecture was still evolving, and it was necessary to clearly separate the responsibilities of components so that the system remained modular, reusable, and easy to extend in future projects.

## What I Tried:
Designed the complete crawler workflow and finalized the purpose of each component.

Discussed:

- Frontier
- SeenStore
- PageFetcher
- PageStorage
- HTMLParser
- URLNormalizer
- URLValidator
- Crawler

Defined how data flows between these components and established that the Crawler should only act as the orchestrator instead of containing business logic.

Also decided to:

- Use my Project-01 Collections Library.
- Keep HTMLParser completely reusable.
- Store crawled pages inside a database.
- Use Breadth-First Search through the Frontier.

## Outcome:
Completed architecture of crawler with clearly separated responsibilities and data flow.

Established a modular design where each component performs a single responsibility and can be independently tested and reused.