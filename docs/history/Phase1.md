# Phase 1 — 인프라 및 프로젝트 골격

작성일: 2026-06-12  
기준 문서: `docs/tmp_docs/PLAN_초안.md`, `docs/PRD.md`, `docs/CONSTRAINTS.md`

---

## 1. Phase 1 목표 요약

Phase 1은 이후 모든 Phase의 토대가 되는 빌드 환경과 레이어 골격을 확립하는 단계다. Visual Studio 2022 솔루션에서 메인 프로젝트(`SampleOrderSystem`)와 테스트 프로젝트(`SampleOrderSystemTests`)가 독립적으로 빌드·실행되고, NuGet `gmock 1.11.0` 기반의 테스트 실행 환경이 동작해야 한다. PoC 분석을 통해 확정된 `nlohmann/json v3.11.3` 단일 헤더(`vendor/json.hpp`)가 JSON 라이브러리로 통합되며, JSON 파일 읽기·쓰기를 담당하는 최소 유틸리티(`JsonFileReader`, `JsonFileWriter`, `JsonParser`)가 단일 의존점으로 고정된다. 이 단계에서 디렉터리 레이아웃, 네임스페이스 규칙, 네이밍 컨벤션, 콘솔 UTF-8 출력 설정이 모두 확정되어야 Phase 2 이후의 도메인 구현이 일관된 기반 위에서 진행될 수 있다.

---

## 2. 구현 항목 목록

> 각 항목은 TDD 사이클(Red → Green → Refactor) 순서로 기술한다.  
> 테스트 없음으로 표시된 항목은 플랫폼 설정 또는 구조 결정 사항으로 단위 테스트 대상이 아니다.

---

### 2-1. 테스트 프로젝트 설정 검증

**구현할 내용**

- 솔루션에 `SampleOrderSystemTests` 프로젝트를 추가한다.
- NuGet `gmock 1.11.0` 패키지를 테스트 프로젝트에 참조한다.
- 테스트 프로젝트가 오류 없이 빌드되고 테스트 러너가 실행됨을 확인한다.
- 별도 클래스 파일 없음 (설정 검증 목적의 인라인 테스트).
- 관련 파일: `tests/infra/InfraTest.cpp` (또는 별도 smoke test 파일)

**TDD 테스트 케이스 (Red 단계에서 작성할 것들)**

| # | 테스트 이름 | 검증 내용 |
|---|-------------|-----------|
| 1 | `TEST(Infra, GoogleTestIsLinked)` | `EXPECT_EQ(1, 1)` — Google Test 링크 및 실행 확인 |

**완료 기준**

- `SampleOrderSystemTests` 프로젝트가 빌드되고 테스트 1건이 Green으로 통과한다.

---

### 2-2. nlohmann/json 통합 검증

**구현할 내용**

- `vendor/json.hpp` (`nlohmann/json v3.11.3` 단일 헤더)를 프로젝트 루트 하위 `vendor/` 디렉터리에 배치한다.
- 테스트 프로젝트와 메인 프로젝트 모두의 `.vcxproj` `AdditionalIncludeDirectories`에 `$(ProjectDir)vendor`를 추가한다.
- 소스 파일에서 `#include "json.hpp"` 로 포함한다.
- PoC 근거: DataPersistence·DataMonitor·DummyDataGenerator 모두 동일한 방식으로 통합 완료.
- 관련 파일: `vendor/json.hpp`

**TDD 테스트 케이스 (Red 단계에서 작성할 것들)**

| # | 테스트 이름 | 검증 내용 |
|---|-------------|-----------|
| 1 | `TEST(Infra, NlohmannJsonIsIncludable)` | `nlohmann::json j = {{"key", "value"}}; EXPECT_EQ(j["key"], "value")` — 헤더 포함 및 기본 동작 확인 |
| 2 | `TEST(Infra, NlohmannJsonHandlesUnicodeString)` | 한국어 문자열(`"샘플"`)을 JSON 값으로 저장하고 읽었을 때 동일한 값이 유지됨 확인 (한국어 round-trip) |

**완료 기준**

- `vendor/json.hpp` 포함 후 빌드가 통과하고 테스트가 Green이다.

---

### 2-3. JsonFileReader — 파일 읽기 유틸리티

**구현할 내용**

- 클래스: `JsonFileReader`
- 네임스페이스: `oss::infra`
- 파일 위치: `src/infra/JsonFileReader.h`, `src/infra/JsonFileReader.cpp`
- 인터페이스: `std::optional<std::string> read(const std::string& path) const`
- 파일이 존재하면 전체 내용을 `std::string`으로 반환하고, 존재하지 않으면 `std::nullopt`를 반환한다.
- 빈 파일은 `std::nullopt`가 아닌 빈 문자열 `""`을 반환한다.
- 테스트 파일: `tests/infra/JsonFileReaderTest.cpp`

**TDD 테스트 케이스 (Red 단계에서 작성할 것들)**

| # | 테스트 이름 | 검증 내용 |
|---|-------------|-----------|
| 1 | `TEST_F(JsonFileReaderTest, ReturnsContentWhenFileExists)` | 존재하는 임시 파일 경로에서 문자열 반환, `std::nullopt` 아님 확인 |
| 2 | `TEST_F(JsonFileReaderTest, ReturnsNulloptWhenFileDoesNotExist)` | 존재하지 않는 경로에서 `std::nullopt` 반환 확인 |
| 3 | `TEST_F(JsonFileReaderTest, ReturnsExactFileContent)` | 작성한 내용과 읽은 내용이 정확히 일치함 확인 |
| 4 | `TEST_F(JsonFileReaderTest, ReturnsEmptyStringForEmptyFile)` | 빈 파일은 `std::nullopt`가 아닌 `""` 반환 확인 |

**Fixture 패턴**

- 테스트 클래스는 `TEST_F(JsonFileReaderTest, ...)` 형태를 사용한다.
- 임시 파일 경로는 `SetUp()`에서 `std::filesystem::temp_directory_path()`를 기반으로 생성하고, `TearDown()`에서 `std::filesystem::remove()`로 정리한다.

> **설계 노트**: Phase 3 시작 전 `IJsonFileReader` Extract Interface 리팩터링 예정. Repository 레이어에서 의존성 역전을 위해 추상 인터페이스로 분리한다.

**완료 기준**

- 4개 테스트 모두 Green.
- 존재하지 않는 경로에 대해 예외를 던지지 않고 `std::nullopt`를 반환한다.
- 빈 파일에 대해 `""` (빈 문자열)을 반환하고 `std::nullopt`를 반환하지 않는다.

---

### 2-4. JsonFileWriter — 파일 쓰기 유틸리티

> **선행 조건**: 이 절의 테스트는 2-3의 `JsonFileReader`가 Green임을 전제한다.

**구현할 내용**

- 클래스: `JsonFileWriter`
- 네임스페이스: `oss::infra`
- 파일 위치: `src/infra/JsonFileWriter.h`, `src/infra/JsonFileWriter.cpp`
- 인터페이스: `bool write(const std::string& path, const std::string& content) const`
- 지정 경로에 문자열을 전체 재작성(flush) 방식으로 저장한다. 성공 시 `true`, 실패 시 `false` 반환.
- PoC 근거: DataPersistence의 `flush()` 패턴 — 변경 시마다 파일 전체 재작성.
- 테스트 파일: `tests/infra/JsonFileWriterTest.cpp`

**TDD 테스트 케이스 (Red 단계에서 작성할 것들)**

| # | 테스트 이름 | 검증 내용 |
|---|-------------|-----------|
| 1 | `TEST_F(JsonFileWriterTest, WriteReturnsTrueOnSuccess)` | 유효한 경로에 쓰기 성공 시 `true` 반환 확인 |
| 2 | `TEST_F(JsonFileWriterTest, WrittenContentMatchesInput)` | 쓴 내용을 `JsonFileReader`로 다시 읽었을 때 원본 문자열과 일치 확인 |
| 3 | `TEST_F(JsonFileWriterTest, OverwritesExistingFile)` | 이미 존재하는 파일에 새 내용을 쓰면 이전 내용이 완전히 대체됨 확인 |
| 4 | `TEST_F(JsonFileWriterTest, WriteReturnsFalseWhenPathIsInvalid)` | 존재하지 않는 중간 디렉터리 경로 또는 접근 불가 경로에 쓸 때 `false` 반환 확인 |

**JsonFileWriter + JsonFileReader 연계 테스트**

| # | 테스트 이름 | 검증 내용 |
|---|-------------|-----------|
| 5 | `TEST_F(JsonFileWriterTest, WriteAndReadRoundTrip_PreservesUnicodeContent)` | 한국어 포함 JSON 문자열(`{"name": "홍길동"}`)을 쓰고 `JsonFileReader`로 읽었을 때 동일한 내용이 유지됨 확인 (Unicode round-trip) |

> **설계 노트**: Phase 3 시작 전 `IJsonFileWriter` Extract Interface 리팩터링 예정. Repository 레이어에서 의존성 역전을 위해 추상 인터페이스로 분리한다.

**완료 기준**

- 5개 테스트 모두 Green.
- 쓰기 후 파일 내용이 입력 문자열과 정확히 일치한다.
- 한국어 문자열이 round-trip 후 손실 없이 보존된다.

---

### 2-5. JsonParser — JSON 파싱 래퍼

**구현할 내용**

- 클래스: `JsonParser`
- 네임스페이스: `oss::infra`
- 파일 위치: `src/infra/JsonParser.h`, `src/infra/JsonParser.cpp`
- 인터페이스:
  - `std::optional<nlohmann::json> parse(const std::string& jsonString) const`
- `nlohmann::json`을 래핑하는 어댑터. 유효한 JSON이면 파싱 결과를 반환하고, 잘못된 JSON이면 `std::nullopt`를 반환한다 (예외를 외부로 노출하지 않음).
- 직렬화 방식: `item["field"]` 직접 접근 방식 사용 — `to_json`/`from_json` ADL 미사용. PoC 3개 공통 패턴.
- 테스트 파일: `tests/infra/JsonParserTest.cpp`

**TDD 테스트 케이스 (Red 단계에서 작성할 것들)**

| # | 테스트 이름 | 검증 내용 |
|---|-------------|-----------|
| 1 | `TEST(JsonParserTest, ParsesValidJsonString)` | 유효한 JSON 문자열에서 파싱 성공, 반환값이 `std::nullopt` 아님 확인 |
| 2 | `TEST(JsonParserTest, ReturnsNulloptForInvalidJson)` | 잘못된 JSON 문자열(`"{ broken"`)에서 `std::nullopt` 반환 확인 |
| 3 | `TEST(JsonParserTest, ParsedValueMatchesInput)` | `{"key": "value"}` 파싱 후 `result["key"] == "value"` 확인 (1번과 검증 범위 분리: 1번은 nullopt 여부만, 3번은 파싱된 값의 정확성 검증) |
| 4 | `TEST(JsonParserTest, ParsesNestedJson)` | 중첩 JSON 구조에서 내부 필드 접근 가능 확인 |
| 5 | `TEST(JsonParserTest, ReturnsNulloptForEmptyString)` | 빈 문자열 `""` 입력 시 `std::nullopt` 반환 확인 |
| 6 | `TEST(JsonParserTest, ReturnsNulloptForWhitespaceOnlyString)` | 공백 전용 문자열 `"   "` 입력 시 `std::nullopt` 반환 확인 |

**완료 기준**

- 6개 테스트 모두 Green.
- 잘못된 JSON에 대해 예외가 호출 측으로 전파되지 않는다.
- 빈 문자열과 공백 전용 문자열 모두 `std::nullopt`를 반환한다.

---

### 2-6. ConsoleSetup — UTF-8 콘솔 설정 유틸리티

**구현할 내용**

- 클래스: `ConsoleSetup`
- 네임스페이스: `oss::app`
- 파일 위치: `src/app/ConsoleSetup.h`, `src/app/ConsoleSetup.cpp`
- 인터페이스: `static void initialize()`
- `SetConsoleOutputCP(CP_UTF8)` + `SetConsoleCP(CP_UTF8)` 호출.
- `.vcxproj` 컴파일러 옵션에 `/utf-8` 추가.
- PoC 근거: 4개 PoC 모두 동일한 방식으로 한국어 콘솔 출력 처리 확인.
- 테스트 없음 (플랫폼 설정 사항 — 단위 테스트 불가).

**완료 기준**

- 한국어 문자열이 콘솔에 깨지지 않고 출력된다 (수동 확인).

---

### 2-7. 디렉터리 및 네임스페이스 골격 생성

**구현할 내용**

- 아래 레이아웃의 디렉터리와 빈 헤더 파일을 배치한다.
- 각 레이어 디렉터리에 대응하는 네임스페이스를 헤더 파일에 선언한다.
- 테스트 없음 (구조 결정 사항).

**네임스페이스 매핑**

| 디렉터리 | 네임스페이스 |
|----------|-------------|
| `src/model/` | `oss::model` |
| `src/repository/` | `oss::repository` |
| `src/controller/` | `oss::controller` |
| `src/view/` | `oss::view` |
| `src/infra/` | `oss::infra` |
| `src/app/` | `oss::app` |

**완료 기준**

- 디렉터리 레이아웃이 아래 "3. 디렉터리 레이아웃"과 일치한다.
- 각 레이어 헤더가 올바른 네임스페이스를 선언한다.

---

### 네이밍 규칙 (전 Phase 공통 확정)

| 대상 | 규칙 | 예시 |
|------|------|------|
| 클래스 / 구조체 | PascalCase | `JsonFileReader`, `SampleRepository` |
| 파일명 | 클래스명과 1:1 PascalCase | `JsonFileReader.h`, `SampleRepository.cpp` |
| 멤버 변수 | snake_case + 후위 밑줄 | `filePath_`, `records_`, `nextId_` |
| 메서드 / 함수 | camelCase | `findById()`, `saveAll()`, `renderList()` |
| JSON 키 | lowerCamelCase (PRD 스키마 기준) | `"sampleId"`, `"orderId"`, `"createdAt"` |
| 열거형 값 | SCREAMING_SNAKE_CASE | `OrderStatus::RESERVED`, `OrderStatus::CONFIRMED` |
| 로컬 변수 | camelCase | `currentItem`, `statusMsg` |

---

## 3. 디렉터리 레이아웃

Phase 1 완료 후 예상 파일 구조:

```
SampleOrderSystem/               ← 메인 프로젝트 루트
├── src/
│   ├── infra/
│   │   ├── JsonFileReader.h
│   │   ├── JsonFileReader.cpp
│   │   ├── JsonFileWriter.h
│   │   ├── JsonFileWriter.cpp
│   │   ├── JsonParser.h
│   │   └── JsonParser.cpp
│   ├── app/
│   │   ├── ConsoleSetup.h
│   │   └── ConsoleSetup.cpp
│   ├── model/               ← 빈 디렉터리 (Phase 2에서 채워짐)
│   ├── repository/          ← 빈 디렉터리 (Phase 3에서 채워짐)
│   ├── controller/          ← 빈 디렉터리 (Phase 4에서 채워짐)
│   └── view/                ← 빈 디렉터리 (Phase 5에서 채워짐)
├── tests/
│   ├── infra/
│   │   ├── JsonFileReaderTest.cpp
│   │   ├── JsonFileWriterTest.cpp
│   │   └── JsonParserTest.cpp
│   ├── model/               ← 빈 디렉터리 (Phase 2에서 채워짐)
│   ├── repository/          ← 빈 디렉터리 (Phase 3에서 채워짐)
│   ├── controller/          ← 빈 디렉터리 (Phase 4에서 채워짐)
│   └── view/                ← 빈 디렉터리 (Phase 5에서 채워짐)
├── vendor/
│   └── json.hpp             ← nlohmann/json v3.11.3 단일 헤더
└── data/
    ├── samples.json          ← PRD 스키마 형식의 초기 파일 (빈 배열)
    ├── orders.json           ← PRD 스키마 형식의 초기 파일 (nextOrderId: 1)
    └── production_queue.json ← PRD 스키마 형식의 초기 파일 (current: null)
```

**data/ 초기 파일 내용 (PRD 5절 스키마 기준)**

```json
// data/samples.json
{
  "samples": []
}

// data/orders.json
{
  "nextOrderId": 1,
  "orders": []
}

// data/production_queue.json
{
  "current": null,
  "queue": []
}
```

---

## 4. 완료 기준 체크리스트

Phase 전체 완료 여부는 아래 체크리스트로 판단한다.

- [ ] `SampleOrderSystem` 프로젝트가 경고 0, 오류 0으로 빌드된다 (MSVC v145, C++20).
- [ ] `SampleOrderSystemTests` 프로젝트가 경고 0, 오류 0으로 빌드된다.
- [ ] 테스트 러너가 Phase 1의 테스트 전부(`InfraTest`, `JsonFileReaderTest`, `JsonFileWriterTest`, `JsonParserTest`) 총 17건을 Green으로 통과한다.
- [ ] `vendor/json.hpp` (nlohmann/json v3.11.3)가 프로젝트에 포함되어 빌드된다.
- [ ] `JsonFileReader`, `JsonFileWriter`, `JsonParser`가 독립적으로 테스트 가능한 클래스로 존재하며 `oss::infra` 네임스페이스 안에 선언된다.
- [ ] `ConsoleSetup::initialize()`가 구현되어 `main.cpp`에서 호출 가능한 상태다.
- [ ] `data/` 디렉터리 아래 3개 JSON 초기 파일이 PRD 스키마 형식으로 존재한다.
- [ ] 한국어 문자열이 콘솔에 깨지지 않고 출력된다 (수동 확인).
- [ ] 디렉터리 레이아웃이 "3. 디렉터리 레이아웃"과 정확히 일치한다.
- [ ] 각 레이어 헤더에 올바른 네임스페이스(`oss::infra`, `oss::app` 등)가 선언되어 있다.

---

## 5. 다음 Phase 진입 조건

Phase 2 시작 전 반드시 아래 조건을 모두 충족해야 한다.

1. **빌드 Green**: `SampleOrderSystem`과 `SampleOrderSystemTests` 모두 경고 0, 오류 0으로 빌드된다.
2. **테스트 전량 Green**: Phase 1의 모든 테스트 케이스(총 17건)가 실패 없이 통과한다.
3. **JSON 유틸리티 인터페이스 고정**: `JsonFileReader::read`, `JsonFileWriter::write`, `JsonParser::parse`의 시그니처가 확정되어 Phase 2의 도메인 객체 직렬화 구현에서 의존 가능한 상태다.
4. **vendor/json.hpp 배치 완료**: `#include "json.hpp"` 한 줄로 모든 레이어에서 nlohmann/json을 사용할 수 있다.
5. **data/ 초기 파일 존재**: 3개 JSON 파일이 PRD 스키마 형식으로 존재하여 Phase 3 Repository 구현 시 로드 테스트의 기준 파일로 사용 가능하다.
6. **네이밍 규칙 팀 합의 완료**: "2-7. 네이밍 규칙" 표의 규칙이 팀 내 합의되어 Phase 2 클래스 설계에 즉시 적용 가능한 상태다.
