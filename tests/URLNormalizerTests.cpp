#include <gtest/gtest.h>
#include "URLNormalizer.h"

// 1. Fully-formed absolute URL should remain unchanged.
TEST(URLNormalizerTest, AbsoluteURLUnchanged) {
    URLNormalizer normalizer;
    std::string base = "https://example.com/path";
    EXPECT_EQ(normalizer.normalize("https://google.com/search", base), "https://google.com/search");
}

// 2. Fragment markers (#) and anything after should be stripped.
TEST(URLNormalizerTest, RemoveFragments) {
    URLNormalizer normalizer;
    std::string base = "https://example.com/home";
    EXPECT_EQ(normalizer.normalize("https://example.com/home#section-1", base), "https://example.com/home");
    EXPECT_EQ(normalizer.normalize("/about#team", base), "https://example.com/about");
}

// 3. Scheme and host portions must be lowercased, leaving the path case-sensitive.
TEST(URLNormalizerTest, LowercaseSchemeAndHost) {
    URLNormalizer normalizer;
    std::string base = "HTTPS://Example.COM/Path/Page.html";
    EXPECT_EQ(normalizer.normalize("HTTPS://Example.COM/Path/Page.html", base), "https://example.com/Path/Page.html");
}

// 4. Resolve plain relative links sitting next to current directory.
TEST(URLNormalizerTest, ResolvePlainRelativeURL) {
    URLNormalizer normalizer;
    std::string base = "https://example.com/about/team.html";
    EXPECT_EQ(normalizer.normalize("contact.html", base), "https://example.com/about/contact.html");
}

// 5. Resolve relative path with leading slash against root.
TEST(URLNormalizerTest, ResolveRootRelativeURL) {
    URLNormalizer normalizer;
    std::string base = "https://example.com/about/team.html";
    EXPECT_EQ(normalizer.normalize("/services/index.html", base), "https://example.com/services/index.html");
}

// 6. Resolve double-dot directory traversal (../) correctly.
TEST(URLNormalizerTest, ResolveParentDirectoryRelativeURL) {
    URLNormalizer normalizer;
    std::string base = "https://example.com/about/team/index.html";
    EXPECT_EQ(normalizer.normalize("../contact.html", base), "https://example.com/about/contact.html");
}

// 7. Redundant trailing slash should be removed from non-root paths.
TEST(URLNormalizerTest, RemoveRedundantTrailingSlash) {
    URLNormalizer normalizer;
    std::string base = "https://example.com";
    EXPECT_EQ(normalizer.normalize("https://example.com/about/", base), "https://example.com/about");
}

// 8. Root trailing slash should NOT be removed.
TEST(URLNormalizerTest, PreservesRootTrailingSlash) {
    URLNormalizer normalizer;
    std::string base = "https://example.com/";
    EXPECT_EQ(normalizer.normalize("https://example.com/", base), "https://example.com/");
}

// 9. Remove empty query marker (?) if it occurs at the end of the URL.
TEST(URLNormalizerTest, RemoveEmptyQueryString) {
    URLNormalizer normalizer;
    std::string base = "https://example.com/page";
    EXPECT_EQ(normalizer.normalize("https://example.com/page?", base), "https://example.com/page");
}

// 10. Preserves non-empty query parameters.
TEST(URLNormalizerTest, PreservesValidQueryParameters) {
    URLNormalizer normalizer;
    std::string base = "https://example.com/page";
    EXPECT_EQ(normalizer.normalize("https://example.com/page?ref=home&id=10", base), "https://example.com/page?ref=home&id=10");
}
