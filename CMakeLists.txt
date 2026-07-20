cmake_minimum_required(VERSION 3.15)

project(WebCrawler)

# ==========================
# C++ Standard
# ==========================
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# ==========================
# Toolchain / MSYS2 Prefix Path Configuration
# ==========================
# Force CMake to search for libraries and packages inside MSYS2 UCRT64 prefix.
list(APPEND CMAKE_PREFIX_PATH "C:/msys64/ucrt64")

# ==========================
# Export Compile Commands
# ==========================
# Enable generation of compile_commands.json for language servers/IDEs.
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# ==========================
# Copy Third-Party Dependencies / Vendor Headers
# ==========================
# Copying of vendor/json.hpp removed. nlohmann_json is now imported as a package.

# ==========================
# Import Project-01 Library
# ==========================
# Include the Collections library from Project-01.
add_subdirectory(external/ds_library-Brachsam)

# ==========================
# Find Third-Party Packages
# ==========================
# Find SQLite3 for persistent page storage.
find_package(SQLite3 REQUIRED)

# Find CURL for network/HTTP fetches.
find_package(CURL REQUIRED)

# Find nlohmann_json.
find_package(nlohmann_json REQUIRED)

# ==========================
# Source Files
# ==========================
# List all source files in the project to compile.
set(SOURCES
    src/main.cpp
    src/frontier.cpp
    src/URLNormalizer.cpp
    src/URLValidator.cpp
    src/robotsHandler.cpp
    src/pagestorage.cpp
    src/configmanager.cpp
    src/logger.cpp
    src/seenstore.cpp
    src/pagefetcher.cpp
    src/htmlParser.cpp
    src/kmp.cpp
    src/crawler.cpp
    src/crawlSchedular.cpp
)

# Indexer source files
set(INDEXER_SOURCES
    src/Indexer/Normalizer.cpp
    src/Indexer/Tokenizer.cpp
    src/Indexer/StopWordFilter.cpp
    src/Indexer/TextExtractor.cpp
    src/Indexer/TermFrequencyCounter.cpp
    src/Indexer/InvertedIndexBuilder.cpp
    src/Indexer/DocumentReader.cpp
    src/Indexer/IndexStorage.cpp
    src/Indexer/Indexer.cpp
    src/Indexer/indexer_main.cpp
    src/pagestorage.cpp
    src/logger.cpp
    src/configmanager.cpp
)

# ==========================
# Executable
# ==========================
add_executable(WebCrawler ${SOURCES})

# ==========================
# Include Directories
# ==========================
target_include_directories(WebCrawler
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/include
)

# ==========================
# Link Libraries
# ==========================
# Link the executable with required third-party and custom libraries.
target_link_libraries(WebCrawler
    PRIVATE
        Collections
        SQLite3::SQLite3
        CURL::libcurl
        nlohmann_json::nlohmann_json
)

# ==========================
# Indexer Executable
# ==========================
add_executable(Indexer ${INDEXER_SOURCES})

target_include_directories(Indexer
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/include
)

target_link_libraries(Indexer
    PRIVATE
        Collections
        SQLite3::SQLite3
)

# ==========================
# Unit Tests Target
# ==========================
enable_testing()

set(TEST_SOURCES
    tests/FrontierTests.cpp
    tests/SeenStoreTests.cpp
    tests/URLNormalizerTests.cpp
    tests/URLValidatorTests.cpp
    tests/HTMLParserTests.cpp
    tests/PageStorageTests.cpp
    tests/IndexerTests.cpp
    src/frontier.cpp
    src/URLNormalizer.cpp
    src/URLValidator.cpp
    src/robotsHandler.cpp
    src/pagestorage.cpp
    src/configmanager.cpp
    src/logger.cpp
    src/seenstore.cpp
    src/pagefetcher.cpp
    src/htmlParser.cpp
    src/kmp.cpp
    src/crawler.cpp
    src/crawlSchedular.cpp
    src/Indexer/Normalizer.cpp
    src/Indexer/Tokenizer.cpp
    src/Indexer/StopWordFilter.cpp
    src/Indexer/TextExtractor.cpp
    src/Indexer/TermFrequencyCounter.cpp
    src/Indexer/InvertedIndexBuilder.cpp
    src/Indexer/DocumentReader.cpp
    src/Indexer/IndexStorage.cpp
    src/Indexer/Indexer.cpp
)

add_executable(CrawlerTests ${TEST_SOURCES})
target_include_directories(CrawlerTests
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/include
)
target_link_libraries(CrawlerTests
    PRIVATE
        Collections
        SQLite3::SQLite3
        CURL::libcurl
        nlohmann_json::nlohmann_json
        gtest
        gtest_main
)

include(GoogleTest)
gtest_discover_tests(CrawlerTests)