#include <gtest/gtest.h>
#include "../src/infra/InputParser.h"

TEST(InputParserTest, TryParseInt_ValidInt_ReturnsValue) {
    auto result = tryParseInt("42");
    ASSERT_TRUE(result.has_value());
    EXPECT_EQ(*result, 42);
}

TEST(InputParserTest, TryParseInt_NonNumeric_ReturnsNullopt) {
    EXPECT_FALSE(tryParseInt("abc").has_value());
}

TEST(InputParserTest, TryParseInt_Empty_ReturnsNullopt) {
    EXPECT_FALSE(tryParseInt("").has_value());
}

TEST(InputParserTest, TryParseDouble_ValidDouble_ReturnsValue) {
    auto result = tryParseDouble("0.92");
    ASSERT_TRUE(result.has_value());
    EXPECT_DOUBLE_EQ(*result, 0.92);
}

TEST(InputParserTest, TryParseDouble_NonNumeric_ReturnsNullopt) {
    EXPECT_FALSE(tryParseDouble("xyz").has_value());
}
