# Config and Logger

Putting these two together in one file because they're both small,cross-cutting things — every other component might need either one, but neither one has much going on internally.

## Configuration 
### What it does
Reads settings before the crawl even starts, so nothing in the crawler has a number like maxDepth = 3 typed directly into its code somewhere. 

```cpp
class Config {
public:
    void   load(string configFilePath);
    int    getMaxDepth();
    int    getTimeoutSeconds();
    string getDatabasePath();
};
```

## Why this exists

If maxDepth or a timeout needs to change, I don't want to go dig through
five files to find where it's hardcoded. One place, one load, everyone
asks Config for what they need.

## What happens if the config file is missing or broken

Falls back to defaults — maxDepth 3, a reasonable timeout — instead of
crashing the whole program on startup. A missing settings file shouldn't
be able to take down the crawler before it even begins.

# Logger
## What it does
The Logger records important events while the crawler is running. All parts of the crawler (such as the Crawler, PageFetcher, and HTMLParser) use the same Logger object. This keeps all messages in one place instead of using many cout statements throughout the code.

```cpp
class Logger {
public:
    void log(string event, string url, string detail = "");
};
```
## Why it exists
The logger helps me understand what the crawler is doing while it is running. Without it, it is difficult to know whether the crawler is working correctly, waiting, or stuck because of an error. These messages make debugging easier and can also be added to the build log or design journal if something goes wrong.

## What's not decided
Right now, the logger has only one log() function. I have not decided whether to add different log levels such as Info, Warning, and Error. I will keep it simple for now and only add log levels later if they become necessary during debugging.