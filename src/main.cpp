// main.cpp
//
// This is the starting point of the program.
// It creates the Crawler object and starts crawling.

#include <iostream>
#include <string>

#include "Crawler.h"

int main(int argc, char* argv[]) {

    // Usage:
    // WebCrawler <seed_url> [config_path]
    if (argc < 2) {
        std::cerr << "Usage: " << argv[0] << " <seed_url> [config_path]" << std::endl;
        std::cerr << "  seed_url    - the starting URL to crawl (required)" << std::endl;
        std::cerr << "  config_path - path to config.txt (default: \"config.txt\")" << std::endl;
        return 1;
    }

    // Read the seed URL and config file path.
    std::string seedUrl = argv[1];
    std::string configPath = (argc >= 3) ? argv[2] : "config.txt";

    try {
        // Create the crawler and start crawling.
        Crawler crawler(configPath, seedUrl);
        crawler.run();

        // Show the total number of pages crawled.
        std::cout << "Crawl complete. Pages crawled: "
                  << crawler.pagesCrawled() << std::endl;
    }
    catch (const ConfigException& e) {
        // Config file error.
        std::cerr << "Configuration error: " << e.what() << std::endl;
        return 1;
    }
    catch (const PageStorageException& e) {
        // Database error.
        std::cerr << "Database error: " << e.what() << std::endl;
        return 1;
    }
    catch (const CrawlerException& e) {
        // Crawler setup error.
        std::cerr << "Crawler setup error: " << e.what() << std::endl;
        return 1;
    }
    catch (const std::exception& e) {
        // Handle any other unexpected error.
        std::cerr << "Unexpected error: " << e.what() << std::endl;
        return 1;
    }

    return 0;
}