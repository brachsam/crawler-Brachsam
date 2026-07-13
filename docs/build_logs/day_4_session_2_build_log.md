### Date: July 10 2026

### Duration: 2 Hours (12:00 PM – 2:00 PM)

## Goal:
Implement database retrieval and save operations within the PageStorage class.

## Problem Encountered:
HTML pages contain single/double quotes, which can break raw SQL query formatting and cause injection flaws unless safe placeholder binds are applied.

## What I Tried:
Implemented `savePage` using sqlite prepared statements and placeholder parameters. Bound URL, depth, and HTML content using `sqlite3_bind_text` and `sqlite3_bind_int` to safely sanitize inputs. Designed an `ON CONFLICT` merge query updating values on duplicate URL crawls. Implemented query helpers `getPage` and `exists` to fetch contents, and `count` to return the stored page database size.

Discussed:
- Prepared statements and query binding lifecycle.
- Resolving duplicate URL conflicts in SQL (`ON CONFLICT(url) DO UPDATE`).
- Exception safety on query preparation failures.
- Marshalling database returns back into C++ Page models.

## Outcome:
Successfully developed PageStorage CRUD operations, ensuring secure, parameterized persistence and retrieval of crawled webpages.
