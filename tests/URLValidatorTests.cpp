#include <gtest/gtest.h>
#include "URLValidator.h"

// 1. A valid http URL should be accepted.
TEST(URLValidatorTest, ValidHttpURL) {
    URLValidator validator;
    EXPECT_TRUE(validator.isValid("http://example.com"));
    EXPECT_TRUE(validator.isValid("http://example.com/path/to/page?query=1"));
}

// 2. A valid https URL should be accepted.
TEST(URLValidatorTest, ValidHttpsURL) {
    URLValidator validator;
    EXPECT_TRUE(validator.isValid("https://example.com"));
    EXPECT_TRUE(validator.isValid("https://sub.domain.com/index.html"));
}

// 3. Scheme check must be case-insensitive.
TEST(URLValidatorTest, SchemeCaseInsensitivity) {
    URLValidator validator;
    EXPECT_TRUE(validator.isValid("HTTPS://example.com"));
    EXPECT_TRUE(validator.isValid("Http://example.com"));
}

// 4. Empty strings should be rejected.
TEST(URLValidatorTest, EmptyURLRejected) {
    URLValidator validator;
    EXPECT_FALSE(validator.isValid(""));
}

// 5. JavaScript protocol links must be rejected.
TEST(URLValidatorTest, JavascriptLinksRejected) {
    URLValidator validator;
    EXPECT_FALSE(validator.isValid("javascript:alert(1)"));
    EXPECT_FALSE(validator.isValid("JAVASCRIPT:void(0)"));
}

// 6. Mailto protocol links must be rejected.
TEST(URLValidatorTest, MailtoLinksRejected) {
    URLValidator validator;
    EXPECT_FALSE(validator.isValid("mailto:info@example.com"));
    EXPECT_FALSE(validator.isValid("MAILTO:press@unacademy.com"));
}

// 7. Bare fragment links (hash-only) must be rejected.
TEST(URLValidatorTest, BareFragmentsRejected) {
    URLValidator validator;
    EXPECT_FALSE(validator.isValid("#section"));
    EXPECT_FALSE(validator.isValid("#top"));
}

// 8. Missing protocol scheme entirely should be rejected.
TEST(URLValidatorTest, MissingProtocolRejected) {
    URLValidator validator;
    EXPECT_FALSE(validator.isValid("www.unacademy.com"));
    EXPECT_FALSE(validator.isValid("example.com/about"));
}

// 9. Other unsupported protocols (e.g. ftp, file, data) should be rejected.
TEST(URLValidatorTest, UnsupportedProtocolsRejected) {
    URLValidator validator;
    EXPECT_FALSE(validator.isValid("ftp://ftp.example.com"));
    EXPECT_FALSE(validator.isValid("file:///C:/localfile.txt"));
    EXPECT_FALSE(validator.isValid("data:text/html,<html>"));
}

// 10. Check behaviour on short malformed strings.
TEST(URLValidatorTest, ShortInvalidStringsRejected) {
    URLValidator validator;
    EXPECT_FALSE(validator.isValid("h"));
    EXPECT_FALSE(validator.isValid("http"));
    EXPECT_FALSE(validator.isValid("https"));
}
