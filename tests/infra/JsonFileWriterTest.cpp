// JsonFileWriter 단위 테스트
// - JsonFileReader Green이 선행 조건이며, Fixture에서 reader로 round-trip 검증한다.
// - 정상 쓰기, 잘못된 경로, Unicode round-trip 케이스를 검증한다.

#include <gtest/gtest.h>
#include <filesystem>
#include <string>

#include "../../src/infra/JsonFileWriter.h"
#include "../../src/infra/JsonFileReader.h"

namespace fs = std::filesystem;

class JsonFileWriterTest : public ::testing::Test {
protected:
    void SetUp() override {
        tempPath_ = fs::temp_directory_path() / "oss_test_JsonFileWriter.json";
    }

    void TearDown() override {
        std::error_code ec;
        fs::remove(tempPath_, ec);
    }

    fs::path tempPath_;
    oss::infra::JsonFileWriter writer_;
    oss::infra::JsonFileReader reader_; // round-trip 검증용
};

// 1. 유효한 경로에 내용을 쓰면 true를 반환하고 파일이 생성된다.
TEST_F(JsonFileWriterTest, WriteCreatesFileWithContent) {
    const std::string content = R"({"id":1,"name":"test"})";

    const bool result = writer_.write(tempPath_.string(), content);

    EXPECT_TRUE(result) << "쓰기 성공인데 false가 반환됐다";
    EXPECT_TRUE(fs::exists(tempPath_)) << "파일이 생성되지 않았다";

    // reader로 round-trip 검증
    const auto readBack = reader_.read(tempPath_.string());
    ASSERT_TRUE(readBack.has_value());
    EXPECT_EQ(*readBack, content);
}

// 2. 접근 불가한 경로(루트 드라이브의 잘못된 서브경로)에 쓰면 false를 반환한다.
TEST_F(JsonFileWriterTest, WriteReturnsFalseWhenPathIsInvalid) {
    // Windows에서 실제로 쓸 수 없는 경로를 사용한다.
    // 널 문자가 포함된 경로는 파일 시스템 API에서 거부된다.
    const std::string invalidPath = "Z:\\__oss_nonexistent_drive__\\sub\\file.json";

    const bool result = writer_.write(invalidPath, "content");

    EXPECT_FALSE(result) << "잘못된 경로인데 true가 반환됐다";
}

// 3. 한국어가 포함된 JSON 문자열을 쓰고 읽었을 때 내용이 보존된다 (Unicode round-trip).
TEST_F(JsonFileWriterTest, WriteAndReadRoundTrip_PreservesUnicodeContent) {
    // 소스 파일은 /utf-8 컴파일 옵션으로 처리되므로 일반 문자열 리터럴로 UTF-8 한국어를 사용한다.
    // C++20에서는 u8"..." 리터럴이 const char8_t* 타입이므로 std::string에 직접 대입 불가.
    const std::string utf8Json = "{\"name\":\"\xed\x99\x8d\xea\xb8\xb8\xeb\x8f\x99\",\"city\":\"\xec\x84\x9c\xec\x9a\xb8\"}";

    const bool writeResult = writer_.write(tempPath_.string(), utf8Json);
    ASSERT_TRUE(writeResult) << "Unicode JSON 쓰기가 실패했다";

    const auto readBack = reader_.read(tempPath_.string());
    ASSERT_TRUE(readBack.has_value()) << "Unicode JSON 읽기가 실패했다";
    EXPECT_EQ(*readBack, utf8Json) << "Unicode round-trip 후 내용이 달라졌다";
}
