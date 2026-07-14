#include <gtest/gtest.h>
#include <cstdio>
#include "pagestorage.h"

// Helper class to clean up the test database file automatically.
class PageStorageTest : public ::testing::Test {
protected:
    const std::string dbPath = "data/test_pagestorage_suite.db";

    void SetUp() override {
        std::remove(dbPath.c_str());
    }

    void TearDown() override {
        std::remove(dbPath.c_str());
    }
};

// 1. A fresh database should have 0 pages.
TEST_F(PageStorageTest, IsEmptyOnCreation) {
    PageStorage storage(dbPath);
    EXPECT_EQ(storage.count(), 0);
}

// 2. Saving a single page increases count.
TEST_F(PageStorageTest, SaveSinglePage) {
    PageStorage storage(dbPath);
    Page page = {"https://example.com", 0, "<html>Hello</html>"};
    
    storage.savePage(page);
    EXPECT_EQ(storage.count(), 1);
}

// 3. Retrieve a saved page and verify all fields are intact.
TEST_F(PageStorageTest, RetrieveSavedPage) {
    PageStorage storage(dbPath);
    Page page = {"https://example.com/about", 2, "<html>About Us</html>"};
    
    storage.savePage(page);
    Page retrieved = storage.getPage("https://example.com/about");
    
    EXPECT_EQ(retrieved.url, "https://example.com/about");
    EXPECT_EQ(retrieved.depth, 2);
    EXPECT_EQ(retrieved.html, "<html>About Us</html>");
}

// 4. Verify exists() behaves correctly.
TEST_F(PageStorageTest, CheckPageExists) {
    PageStorage storage(dbPath);
    std::string url = "https://example.com/services";
    
    EXPECT_FALSE(storage.exists(url));
    storage.savePage({url, 1, "<html>Services</html>"});
    EXPECT_TRUE(storage.exists(url));
}

// 5. Duplicate writes on same URL should update existing row (ON CONFLICT DO UPDATE).
TEST_F(PageStorageTest, UpdatePageOnConflict) {
    PageStorage storage(dbPath);
    std::string url = "https://example.com";
    
    storage.savePage({url, 0, "Initial"});
    EXPECT_EQ(storage.count(), 1);
    
    // Write duplicate URL with different payload
    storage.savePage({url, 3, "Updated Payload"});
    EXPECT_EQ(storage.count(), 1); // Row count should still be 1
    
    Page retrieved = storage.getPage(url);
    EXPECT_EQ(retrieved.depth, 3);
    EXPECT_EQ(retrieved.html, "Updated Payload");
}

// 6. Retrieving a non-existent page must throw PageNotFoundException.
TEST_F(PageStorageTest, RetrieveMissingPageThrows) {
    PageStorage storage(dbPath);
    EXPECT_THROW(storage.getPage("https://example.com/notfound"), PageNotFoundException);
}

// 7. Store multiple different pages successfully.
TEST_F(PageStorageTest, SaveMultiplePages) {
    PageStorage storage(dbPath);
    storage.savePage({"url1", 1, "html1"});
    storage.savePage({"url2", 2, "html2"});
    storage.savePage({"url3", 3, "html3"});
    
    EXPECT_EQ(storage.count(), 3);
    EXPECT_TRUE(storage.exists("url1"));
    EXPECT_TRUE(storage.exists("url2"));
    EXPECT_TRUE(storage.exists("url3"));
}

// 8. Prepared statements should handle single/double quotes safely in HTML.
TEST_F(PageStorageTest, QuoteInjectionSafety) {
    PageStorage storage(dbPath);
    std::string trickyHtml = "<html><body attr=\"value\">It's a tricky 'html' payload!</body></html>";
    
    EXPECT_NO_THROW(storage.savePage({"url", 1, trickyHtml}));
    Page retrieved = storage.getPage("url");
    EXPECT_EQ(retrieved.html, trickyHtml);
}

// 9. Verify behaviour under empty HTML payloads.
TEST_F(PageStorageTest, SaveEmptyHTMLPayload) {
    PageStorage storage(dbPath);
    storage.savePage({"https://example.com/empty", 1, ""});
    
    Page retrieved = storage.getPage("https://example.com/empty");
    EXPECT_EQ(retrieved.html, "");
}

// 10. Perform stress test writing many pages.
TEST_F(PageStorageTest, StressWriteManyPages) {
    PageStorage storage(dbPath);
    for (int i = 0; i < 100; ++i) {
        storage.savePage({"url" + std::to_string(i), i, "html"});
    }
    EXPECT_EQ(storage.count(), 100);
}
