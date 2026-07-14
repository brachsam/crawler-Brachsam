#include <gtest/gtest.h>
#include "frontier.h"

// 1. A fresh frontier should be completely empty.
TEST(FrontierTest, IsEmptyOnCreation) {
    Frontier frontier;
    EXPECT_TRUE(frontier.isEmpty());
    EXPECT_EQ(frontier.size(), 0);
}

// 2. Enqueuing one item should update size and empty status.
TEST(FrontierTest, EnqueueSingleElement) {
    Frontier frontier;
    URLDepth item = {"https://example.com", 1};
    frontier.enqueue(item);
    EXPECT_FALSE(frontier.isEmpty());
    EXPECT_EQ(frontier.size(), 1);
}

// 3. Dequeuing the only item should return the item and leave the queue empty.
TEST(FrontierTest, DequeueSingleElement) {
    Frontier frontier;
    URLDepth item = {"https://example.com", 1};
    frontier.enqueue(item);

    URLDepth popped = frontier.dequeue();
    EXPECT_EQ(popped.url, "https://example.com");
    EXPECT_EQ(popped.depth, 1);
    EXPECT_TRUE(frontier.isEmpty());
    EXPECT_EQ(frontier.size(), 0);
}

// 4. Multiple enqueues should maintain FIFO order.
TEST(FrontierTest, FIFOOrderMultipleElements) {
    Frontier frontier;
    frontier.enqueue({"https://example.com/1", 1});
    frontier.enqueue({"https://example.com/2", 2});
    frontier.enqueue({"https://example.com/3", 3});

    EXPECT_EQ(frontier.dequeue().url, "https://example.com/1");
    EXPECT_EQ(frontier.dequeue().url, "https://example.com/2");
    EXPECT_EQ(frontier.dequeue().url, "https://example.com/3");
    EXPECT_TRUE(frontier.isEmpty());
}

// 5. Dequeuing from an empty queue must throw FrontierEmptyException.
TEST(FrontierTest, DequeueEmptyThrowsException) {
    Frontier frontier;
    EXPECT_THROW(frontier.dequeue(), FrontierEmptyException);
}

// 6. Exception message should contain relevant information.
TEST(FrontierTest, ExceptionMessageVerification) {
    Frontier frontier;
    try {
        frontier.dequeue();
        FAIL() << "Expected FrontierEmptyException was not thrown";
    } catch (const FrontierEmptyException& e) {
        EXPECT_TRUE(std::string(e.what()).length() > 0);
    }
}

// 7. Test mixing enqueues and dequeues.
TEST(FrontierTest, MixedEnqueueAndDequeue) {
    Frontier frontier;
    frontier.enqueue({"https://example.com/1", 1});
    frontier.enqueue({"https://example.com/2", 2});
    
    EXPECT_EQ(frontier.dequeue().url, "https://example.com/1");
    
    frontier.enqueue({"https://example.com/3", 3});
    
    EXPECT_EQ(frontier.dequeue().url, "https://example.com/2");
    EXPECT_EQ(frontier.dequeue().url, "https://example.com/3");
    EXPECT_TRUE(frontier.isEmpty());
}

// 8. Size tracking should remain accurate across operations.
TEST(FrontierTest, SizeTrackingAccurate) {
    Frontier frontier;
    EXPECT_EQ(frontier.size(), 0);
    
    for (int i = 0; i < 50; ++i) {
        frontier.enqueue({"url", i});
    }
    EXPECT_EQ(frontier.size(), 50);

    for (int i = 0; i < 20; ++i) {
        frontier.dequeue();
    }
    EXPECT_EQ(frontier.size(), 30);
}

// 9. Enqueuing duplicate values is allowed by the Frontier itself.
TEST(FrontierTest, DuplicatesAllowed) {
    Frontier frontier;
    frontier.enqueue({"https://example.com", 1});
    frontier.enqueue({"https://example.com", 1});
    
    EXPECT_EQ(frontier.size(), 2);
    EXPECT_EQ(frontier.dequeue().url, "https://example.com");
    EXPECT_EQ(frontier.dequeue().url, "https://example.com");
}

// 10. Verify depth values are preserved exactly.
TEST(FrontierTest, DepthPreservation) {
    Frontier frontier;
    frontier.enqueue({"url0", 0});
    frontier.enqueue({"url5", 5});
    frontier.enqueue({"url99", 99});

    EXPECT_EQ(frontier.dequeue().depth, 0);
    EXPECT_EQ(frontier.dequeue().depth, 5);
    EXPECT_EQ(frontier.dequeue().depth, 99);
}
