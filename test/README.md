# 테스트 환경 안내

## 테스트 구성 개요

이 프로젝트는 **Google Test / Google Mock** 프레임워크를 사용하여 단위 테스트를 작성합니다.

| 항목 | 내용 |
|------|------|
| 프레임워크 | Google Test / Google Mock |
| 버전 | gmock 1.11.0 |
| 패키지 관리 | NuGet (`packages/gmock.1.11.0/`) |
| 헤더 경로 | `packages/gmock.1.11.0/lib/native/include/` (gmock.targets가 자동 추가) |
| 소스 컴파일 | `gtest-all.cc`, `gmock-all.cc` — gmock.targets가 자동 처리 (직접 추가 불필요) |

---

## 빌드 구성: Test|x64

테스트를 실행하려면 반드시 **Test|x64** 구성으로 빌드해야 합니다.

### `#ifdef TESTING` 분기 설명

`main.cpp`는 전처리기 매크로 `TESTING` 정의 여부에 따라 진입점을 분리합니다.

```cpp
#ifdef TESTING
// Test|x64 구성: Google Test 진입점
int main(int argc, char** argv) {
    ::testing::InitGoogleMock(&argc, argv);
    return RUN_ALL_TESTS();
}
#else
// Debug|x64, Release|x64 구성: 앱 진입점
int main() {
    // ... 애플리케이션 코드
}
#endif
```

| 빌드 구성 | `TESTING` 정의 | 실행 대상 |
|-----------|---------------|-----------|
| `Debug\|x64` | 미정의 | 앱 진입점 (`main()`) |
| `Release\|x64` | 미정의 | 앱 진입점 (`main()`) |
| `Test\|x64` | **정의됨** | Google Test 진입점 (`RUN_ALL_TESTS()`) |

테스트 파일(`test/*.cpp`)도 Debug/Release 구성에서는 `ExcludedFromBuild=true`로 제외됩니다.

---

## 빌드 명령

### 테스트 빌드 (Test|x64)

```bat
"C:\Program Files\Microsoft Visual Studio\18\Community\MSBuild\Current\Bin\MSBuild.exe" ^
    SampleOrderSystem-KHNam-CRA-3.slnx ^
    /p:Configuration=Test ^
    /p:Platform=x64 ^
    /v:minimal
```

### 앱 빌드 (Debug|x64)

```bat
"C:\Program Files\Microsoft Visual Studio\18\Community\MSBuild\Current\Bin\MSBuild.exe" ^
    SampleOrderSystem-KHNam-CRA-3.slnx ^
    /p:Configuration=Debug ^
    /p:Platform=x64 ^
    /v:minimal
```

---

## 테스트 실행 방법

1. 위 빌드 명령으로 **Test|x64** 구성 빌드
2. 빌드 성공 후 생성된 실행 파일 실행:

```bat
x64\Test\SampleOrderSystem-KHNam-CRA-3.exe
```

실행 시 Google Test가 등록된 모든 테스트를 자동으로 탐색하고 결과를 출력합니다.

---

## 새 테스트 파일 추가 방법

### 1단계: 파일 생성

`test/` 폴더 하위에 새 테스트 파일을 추가합니다.

```
test/
├── smoke_test.cpp        (기존)
└── your_new_test.cpp     (신규)
```

### 2단계: vcxproj에 ClCompile 항목 등록

`SampleOrderSystem-KHNam-CRA-3.vcxproj`에 아래 형식으로 추가합니다.
Debug/Release 구성에서는 반드시 `ExcludedFromBuild=true`를 지정해야 합니다.

```xml
<ClCompile Include="test\your_new_test.cpp">
  <ExcludedFromBuild Condition="'$(Configuration)|$(Platform)'=='Debug|x64'">true</ExcludedFromBuild>
  <ExcludedFromBuild Condition="'$(Configuration)|$(Platform)'=='Release|x64'">true</ExcludedFromBuild>
</ClCompile>
```

### 3단계: 테스트 코드 작성 예시

```cpp
#include <gtest/gtest.h>
#include <gmock/gmock.h>

// 테스트 스위트명_상태_기대동작 패턴 사용
TEST(YourClass, MethodName_WhenCondition_ExpectedBehavior) {
    // Arrange
    // ...

    // Act
    // ...

    // Assert
    EXPECT_EQ(expected, actual);
}
```

---

## 주의사항

- 소스 파일에 한국어 주석 작성 시 C4819 경고 발생 (코드페이지 949 문제) — ASCII 주석 사용을 권장합니다.
- `gtest-all.cc` / `gmock-all.cc`를 직접 vcxproj에 추가하면 중복 컴파일 오류가 발생합니다. gmock.targets가 이미 처리하므로 추가하지 마십시오.
- `.slnx`에 새 구성을 추가할 경우 `<BuildType Name="..." />`을 반드시 함께 추가해야 MSB4126 오류를 방지할 수 있습니다.
