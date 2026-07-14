#include <gtest/gtest.h>
#include "htmlParser.h"

// 1. Parser should return an empty array on empty HTML input.
TEST(HTMLParserTest, EmptyHtmlInput) {
    HTMLParser parser;
    auto links = parser.extractLinks("");
    EXPECT_EQ(links.size(), 0);
}

// 2. Extract standard link with double quotes.
TEST(HTMLParserTest, ExtractDoubleQuotedLink) {
    HTMLParser parser;
    std::string html = "<a href=\"https://example.com/page\">Link</a>";
    auto links = parser.extractLinks(html);
    ASSERT_EQ(links.size(), 1);
    EXPECT_EQ(links.get(0), "https://example.com/page");
}

// 3. Extract link with single quotes.
TEST(HTMLParserTest, ExtractSingleQuotedLink) {
    HTMLParser parser;
    std::string html = "<a href='https://example.com/page'>Link</a>";
    auto links = parser.extractLinks(html);
    ASSERT_EQ(links.size(), 1);
    EXPECT_EQ(links.get(0), "https://example.com/page");
}

// 4. Handle space variants around the equals sign in href attributes.
TEST(HTMLParserTest, HrefSpacingVariants) {
    HTMLParser parser;
    std::string html = "<a href  =   \"https://example.com/space\">Link</a>";
    auto links = parser.extractLinks(html);
    ASSERT_EQ(links.size(), 1);
    EXPECT_EQ(links.get(0), "https://example.com/space");
}

// 5. Extract multiple links from a document.
TEST(HTMLParserTest, ExtractMultipleLinks) {
    HTMLParser parser;
    std::string html = "<div><a href=\"/1\">One</a></div>"
                       "<span><a href=\"/2\">Two</a></span>";
    auto links = parser.extractLinks(html);
    ASSERT_EQ(links.size(), 2);
    EXPECT_EQ(links.get(0), "/1");
    EXPECT_EQ(links.get(1), "/2");
}

// 6. Anchor tag without href should not return a link.
TEST(HTMLParserTest, AnchorWithoutHref) {
    HTMLParser parser;
    std::string html = "<a name=\"top\">Top Section</a><a>Empty Link</a>";
    auto links = parser.extractLinks(html);
    EXPECT_EQ(links.size(), 0);
}

// 7. Extract link when there are other attributes inside the anchor tag.
TEST(HTMLParserTest, AnchorWithMultipleAttributes) {
    HTMLParser parser;
    std::string html = "<a class=\"btn\" id=\"main\" href=\"/home\" target=\"_blank\">Home</a>";
    auto links = parser.extractLinks(html);
    ASSERT_EQ(links.size(), 1);
    EXPECT_EQ(links.get(0), "/home");
}

// 8. Handle cases where anchor tag name is capitalized.
TEST(HTMLParserTest, CaseInsensitiveTagMatching) {
    HTMLParser parser;
    std::string html = "<A HREF=\"/capital\">Link</A>";
    auto links = parser.extractLinks(html);
    ASSERT_EQ(links.size(), 1);
    EXPECT_EQ(links.get(0), "/capital");
}

// 9. Handle case where attribute name is capitalized.
TEST(HTMLParserTest, CaseInsensitiveAttributeMatching) {
    HTMLParser parser;
    std::string html = "<a HREF=\"/capital-attr\">Link</a>";
    auto links = parser.extractLinks(html);
    ASSERT_EQ(links.size(), 1);
    EXPECT_EQ(links.get(0), "/capital-attr");
}

// 10. Robustness against malformed tags or early EOF.
TEST(HTMLParserTest, MalformedHTMLParsing) {
    HTMLParser parser;
    std::string html = "<a href=\"/unclosed";
    auto links = parser.extractLinks(html);
    // Unclosed quote makes it invalid, or extracted partially depending on bounds check
    // Here we assert that it does not crash
    EXPECT_NO_THROW(parser.extractLinks(html));
}
