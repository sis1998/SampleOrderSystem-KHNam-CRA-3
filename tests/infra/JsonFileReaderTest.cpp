// JsonFileReader 단위 테스트
// - 정상 파일 읽기, 존재하지 않는 파일, 빈 파일 케이스를 검증한다.
// - SetUp()에서 temp_directory_path() 기반 임시 파일을 생성하고
//   TearDown()에서 정리한다.

#include <gtest/gtest.h>
#include <filesystem>
#include <fstream>
#include <string>

#include "../../src/infra/JsonFileReader.h"

namespace fs = std::filesystem;

class JsonFileReaderTest : public ::testing::Test {
protected:
    void SetUp() override {
        // temp 디렉터리 아래에 고유한 임시 파일 경로를 결정한다.
        // (실제 파일 생성은 각 테스트에서 필요에 따라 수행한다.)
        tempPath_ = fs::temp_directory_path() / "oss_test_JsonFileReader.json";
    }

    void TearDown() override {
        // 테스트 후 임시 파일이 남아있으면 제거한다.
        std::error_code ec;
        fs::remove(tempPath_, ec);
    }

    // 헬퍼: 임시 파일에 지정 내용을 기록한다.
    void writeTempFile(const std::string& content) {
        std::ofstream out(tempPath_, std::ios::out | std::ios::trunc | std::ios::binary);
        out << content;
    }

    fs::path tempPath_;
    oss::infra::JsonFileReader reader_;
};

// 1. 파일이 존재하고 내용이 있을 때 해당 내용을 반환한다.
TEST_F(JsonFileReaderTest, ReturnsFileContentsWhenFileExists) {
    const std::string expected = R"({"key":"value"})";
    writeTempFile(expected);

    const auto result = reader_.read(tempPath_.string());

    ASSERT_TRUE(result.has_value()) << "파일이 존재하는데 nullopt가 반환됐다";
    EXPECT_EQ(*result, expected);
}

// 2. 파일이 존재하지 않을 때 nullopt를 반환한다.
TEST_F(JsonFileReaderTest, ReturnsNulloptWhenFileDoesNotExist) {
    // 임시 파일을 생성하지 않고 존재하지 않는 경로로 읽기를 시도한다.
    const fs::path nonExistent = fs::temp_directory_path() / "oss_test_does_not_exist_xyz.json";
    std::error_code ec;
    fs::remove(nonExistent, ec); // 혹시 남아있을 경우 제거

    const auto result = reader_.read(nonExistent.string());

    EXPECT_FALSE(result.has_value()) << "존재하지 않는 파일인데 값이 반환됐다";
}

// 3. 파일이 존재하지만 내용이 비어 있을 때 "" (빈 문자열)을 반환한다 (nullopt 아님).
TEST_F(JsonFileReaderTest, ReturnsEmptyStringForEmptyFile) {
    writeTempFile(""); // 빈 파일 생성

    const auto result = reader_.read(tempPath_.string());

    ASSERT_TRUE(result.has_value()) << "빈 파일인데 nullopt가 반환됐다";
    EXPECT_EQ(*result, "") << "빈 파일은 빈 문자열을 반환해야 한다";
}
