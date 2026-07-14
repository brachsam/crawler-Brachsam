### Date: July 11, 2026

### Duration: 2 Hours (11:00 AM – 01:00 PM)

## Goal:
Implement the KMP search algorithm and write the HTMLParser logic to extract raw link strings.

## Problem Encountered:
HTML pages can be malformed, containing variations in formatting (such as extra spaces around equals signs, like `href = "url"`, or mixing single and double quotes). Standalone string matching on `href="` alone would skip these variants, causing the parser to miss valid links or extract broken substrings.

## What I Tried:
I wrote the KMP algorithm implementation, including the prefix table precomputation logic. Then, I developed `HTMLParser::extractLinks` to first search for the `<a` substring using the KMP utility. Once an anchor tag is found, the parser scans within that tag's bounds using KMP to find the `href` attribute. I added specific checks to handle spacing variance around the attribute assignments and handled single quotes (`'`), double quotes (`"`), and unquoted values dynamically. I also added defensive index boundary checks to prevent segmentation faults when parsing malformed tags at the very end of files.

## Outcome:
Successfully implemented the KMP algorithm and HTMLParser link extraction, and verified that it correctly extracts raw links from a suite of mock malformed HTML pages.

## Next Step:
Integrate HTMLParser with the existing crawler modules (PageStorage, URLNormalizer, URLValidator).
