#include <gtest/gtest.h>
#include "seenstore.h"

// 1. Fresh SeenStore should be empty.
TEST(SeenStoreTest, IsEmptyOnCreation) {
    SeenStore seenStore;
    EXPECT_EQ(seenStore.size(), 0);
}

// 2. Marking a URL makes it seen.
TEST(SeenStoreTest, MarkSingleSeen) {
    SeenStore seenStore;
    std::string url = "https://example.com";
    
    EXPECT_FALSE(seenStore.isSeen(url));
    seenStore.markSeen(url);
    EXPECT_TRUE(seenStore.isSeen(url));
    EXPECT_EQ(seenStore.size(), 1);
}

// 3. Marking duplicate URLs should not increase the size.
TEST(SeenStoreTest, DuplicateMarkingDoesNotGrowSize) {
    SeenStore seenStore;
    std::string url = "https://example.com";
    
    seenStore.markSeen(url);
    seenStore.markSeen(url);
    seenStore.markSeen(url);
    
    EXPECT_TRUE(seenStore.isSeen(url));
    EXPECT_EQ(seenStore.size(), 1);
}

// 4. Marking multiple different URLs should update size correctly.
TEST(SeenStoreTest, MultipleDifferentURLs) {
    SeenStore seenStore;
    seenStore.markSeen("https://example.com/1");
    seenStore.markSeen("https://example.com/2");
    seenStore.markSeen("https://example.com/3");
    
    EXPECT_TRUE(seenStore.isSeen("https://example.com/1"));
    EXPECT_TRUE(seenStore.isSeen("https://example.com/2"));
    EXPECT_TRUE(seenStore.isSeen("https://example.com/3"));
    EXPECT_EQ(seenStore.size(), 3);
}

// 5. Check lookup of unvisited URL returns false.
TEST(SeenStoreTest, UnvisitedReturnsFalse) {
    SeenStore seenStore;
    seenStore.markSeen("https://example.com/visited");
    
    EXPECT_FALSE(seenStore.isSeen("https://example.com/unvisited"));
}

// 6. Test case sensitivity on URL strings.
TEST(SeenStoreTest, CaseSensitivityChecked) {
    SeenStore seenStore;
    seenStore.markSeen("https://example.com/page");
    
    EXPECT_TRUE(seenStore.isSeen("https://example.com/page"));
    EXPECT_FALSE(seenStore.isSeen("https://example.com/Page"));
    EXPECT_FALSE(seenStore.isSeen("HTTPS://example.com/page"));
}

// 7. Testing empty string storage.
TEST(SeenStoreTest, EmptyStringAllowed) {
    SeenStore seenStore;
    EXPECT_FALSE(seenStore.isSeen(""));
    seenStore.markSeen("");
    EXPECT_TRUE(seenStore.isSeen(""));
    EXPECT_EQ(seenStore.size(), 1);
}

// 8. Testing extremely long URLs.
TEST(SeenStoreTest, LongURLAllowed) {
    SeenStore seenStore;
    std::string longUrl = "https://example.com/" + std::string(1000, 'a');
    
    EXPECT_FALSE(seenStore.isSeen(longUrl));
    seenStore.markSeen(longUrl);
    EXPECT_TRUE(seenStore.isSeen(longUrl));
    EXPECT_EQ(seenStore.size(), 1);
}

// 9. Testing URLs with special characters and queries.
TEST(SeenStoreTest, SpecialCharactersPreserved) {
    SeenStore seenStore;
    std::string url = "https://example.com/search?q=hello&lang=en#section";
    
    EXPECT_FALSE(seenStore.isSeen(url));
    seenStore.markSeen(url);
    EXPECT_TRUE(seenStore.isSeen(url));
}

// 10. Performance check under multiple marks.
TEST(SeenStoreTest, StressMultipleMarks) {
    SeenStore seenStore;
    for (int i = 0; i < 500; ++i) {
        seenStore.markSeen("https://example.com/" + std::to_string(i));
    }
    EXPECT_EQ(seenStore.size(), 500);
    EXPECT_TRUE(seenStore.isSeen("https://example.com/250"));
    EXPECT_FALSE(seenStore.isSeen("https://example.com/501"));
}
