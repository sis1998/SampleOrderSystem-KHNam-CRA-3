// JsonParser 단위 테스트
// - 유효한 JSON 파싱, 잘못된 JSON 입력, 중첩 JSON, 빈 입력 케이스를 검증한다.
// - JsonParser는 예외를 외부로 노출하지 않고 nullopt로 처리해야 한다.

#include <gtest/gtest.h>
#include <string>

#include "../../src/infra/JsonParser.h"

// JsonParser는 stateless이므로 TEST_F 대신 TEST를 사용한다.
// 모든 케이스에서 동일한 parser 인스턴스를 통해 검증한다.
namespace {
    oss::infra::JsonParser parser;
}

// 1. 유효한 JSON 객체 문자열을 파싱하면 optional 값이 반환된다.
TEST(JsonParserTest, ParsesValidJsonObject) {
    const std::string input = R"({"key":"value"})";

    const auto result = parser.parse(input);

    ASSERT_TRUE(result.has_value()) << "유효한 JSON인데 nullopt가 반환됐다";
    EXPECT_EQ(result->at("key").get<std::string>(), "value");
}

// 2. 잘못된 JSON 문자열을 파싱하면 nullopt를 반환한다.
TEST(JsonParserTest, ReturnsNulloptForInvalidJson) {
    const std::string input = "{ broken json";

    const auto result = parser.parse(input);

    EXPECT_FALSE(result.has_value()) << "잘못된 JSON인데 값이 반환됐다";
}

// 3. 중첩 JSON 객체를 파싱하면 내부 필드에 접근할 수 있다.
TEST(JsonParserTest, ParsesNestedJson) {
    const std::string input = R"({"outer":{"inner":"hello","count":42}})";

    const auto result = parser.parse(input);

    ASSERT_TRUE(result.has_value()) << "중첩 JSON 파싱이 실패했다";
    EXPECT_EQ(result->at("outer").at("inner").get<std::string>(), "hello");
    EXPECT_EQ(result->at("outer").at("count").get<int>(), 42);
}

// 4. 빈 문자열을 입력하면 nullopt를 반환한다.
TEST(JsonParserTest, ReturnsNulloptForEmptyString) {
    const auto result = parser.parse("");

    EXPECT_FALSE(result.has_value()) << "빈 문자열인데 값이 반환됐다";
}

// 5. 공백만 있는 문자열을 입력하면 nullopt를 반환한다.
TEST(JsonParserTest, ReturnsNulloptForWhitespaceOnlyString) {
    const auto result = parser.parse("   ");

    EXPECT_FALSE(result.has_value()) << "공백 전용 문자열인데 값이 반환됐다";
}
