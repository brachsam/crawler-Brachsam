cmake_minimum_required(VERSION 3.15)

project(WebCrawler)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# ==========================
# Import Project-01 Library
add_subdirectory(external/ds_library-Brachsam)

# ==========================
# Include Header Files
target_include_directories(Collections
    PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)

# ==========================
# WebCrawler Executable
add_executable(WebCrawler
    src/main.cpp
    src/URLNormalizer.cpp
)

target_link_libraries(WebCrawler
    PRIVATE
    Collections
)