// nlohmann/json v3.11.3 통합 검증 테스트
// - vendor/json.hpp 헤더가 정상적으로 포함되어 기본 직렬화/역직렬화가 동작함을 확인한다.
// - 한국어 문자열의 Unicode round-trip을 검증한다.

#include <gtest/gtest.h>
#include <string>

#include "../../vendor/json.hpp"

using json = nlohmann::json;

// 1. parse() 후 dump(2)로 직렬화하면 원본 데이터를 포함한 포맷된 JSON이 생성된다.
TEST(NlohmannIntegrationTest, NlohmannJsonParsesAndSerializes) {
    const std::string input = R"({"key":"value","number":42})";

    const json parsed = json::parse(input);
    const std::string dumped = parsed.dump(2); // indent=2

    // 다시 파싱해서 값이 보존됐는지 검증한다.
    const json reparsed = json::parse(dumped);
    EXPECT_EQ(reparsed.at("key").get<std::string>(), "value");
    EXPECT_EQ(reparsed.at("number").get<int>(), 42);

    // dump(2) 결과에 줄바꿈이 포함되어야 한다 (포맷 적용 확인).
    EXPECT_NE(dumped.find('\n'), std::string::npos)
        << "dump(2) 결과에 줄바꿈이 없다 — 포맷이 적용되지 않았다";
}

// 2. 한국어 문자열을 JSON 값으로 저장하고 dump → parse round-trip 후 동일한 값이 유지된다.
TEST(NlohmannIntegrationTest, NlohmannJsonHandlesUnicodeString) {
    // UTF-8 인코딩 한국어 ("샘플 시스템")
    // C++20에서 u8"..." 리터럴은 const char8_t* 타입이므로 std::string에 직접 대입 불가.
    // /utf-8 컴파일 옵션 덕분에 일반 문자열 리터럴도 UTF-8로 처리된다.
    const std::string koreanValue = "\xec\x83\x98\xed\x94\x8c \xec\x8b\x9c\xec\x8a\xa4\xed\x85\x9c";

    json original;
    original["name"] = koreanValue;

    // dump 후 다시 parse
    const std::string serialized = original.dump();
    const json deserialized = json::parse(serialized);

    EXPECT_EQ(deserialized.at("name").get<std::string>(), koreanValue)
        << "한국어 문자열 round-trip 후 값이 달라졌다";
}
