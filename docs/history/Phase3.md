# Phase 3 — 유효성 검사 + UX 개선 (인덱스 기반 선택)

**브랜치:** `phase3/validation-ux` (master에서 분기)  
**목표:** 모든 사용자 입력에 유효성 검사를 추가하고, CRUD 진행 시 목록을 번호로 표시해 인덱스로 선택하도록 UX를 개선한다.

---

## 목차

1. [파일 구조](#파일-구조)
2. [Task 1 — InputParser 헬퍼](#task-1--inputparser-헬퍼)
3. [Task 2 — View 번호 접두사](#task-2--view-번호-접두사)
4. [Task 3 — OrderController::placeOrder 인덱스 선택](#task-3--ordercontrollerplaceorder-인덱스-선택)
5. [Task 4 — SampleController 등록 유효성 검사](#task-4--samplecontroller-등록-유효성-검사)
6. [Task 5 — OrderController 선택 입력 유효성 검사](#task-5--ordercontroller-선택-입력-유효성-검사)
7. [커밋 순서](#커밋-순서)

---

## 파일 구조

```
src/
  infra/
    InputParser.h            # tryParseInt / tryParseDouble — std::optional 반환
  view/
    SampleView.h/.cpp        # renderIndexedList 추가
    OrderView.cpp            # renderReservedList / renderConfirmedList 번호 접두사 추가
  controller/
    OrderController.h        # SampleView sampleView_ 멤버 추가
    OrderController.cpp      # placeOrder 인덱스 선택 + 전체 유효성 검사
    SampleController.cpp     # 등록 유효성 검사 추가
test/
  InputParserTest.cpp        # 신규
  SampleControllerTest.cpp   # 유효성 검사 케이스 추가
  OrderControllerTest.cpp    # 인덱스 UX 반영 + 유효성 검사 케이스 추가
```

---

## Task 목록

### Task 1 — InputParser 헬퍼

`src/infra/InputParser.h` (헤더-온리) — `stoi` / `stod` 예외를 `std::optional`로 래핑.

```cpp
inline std::optional<int> tryParseInt(std::string_view s) {
    try { return std::stoi(std::string(s)); } catch (...) { return std::nullopt; }
}

inline std::optional<double> tryParseDouble(std::string_view s) {
    try { return std::stod(std::string(s)); } catch (...) { return std::nullopt; }
}
```

이후 모든 컨트롤러에서 `stoi` / `stod` 직접 호출 없이 이 함수를 사용한다.  
vcxproj에 `test/InputParserTest.cpp` ClCompile 항목을 추가한다.

**검증 (단위 테스트):**
```cpp
EXPECT_EQ(*tryParseInt("42"), 42);
EXPECT_FALSE(tryParseInt("abc").has_value());
EXPECT_FALSE(tryParseInt("").has_value());
EXPECT_DOUBLE_EQ(*tryParseDouble("0.92"), 0.92);
EXPECT_FALSE(tryParseDouble("xyz").has_value());
```

---

### Task 2 — View 번호 접두사

**SampleView** — `renderIndexedList` 추가: 시료 선택이 필요한 화면에서 사용.

```cpp
// 출력 예시
=== 시료 목록 ===
 1. [S-001] 실리콘 웨이퍼-8인치 | 재고: 480
 2. [S-002] GaN-on-SiC 100mm    | 재고: 120
```

**OrderView** — `renderReservedList` / `renderConfirmedList`에 번호 접두사 추가:

```cpp
// 출력 예시 (승인 대기 주문)
=== 승인 대기 주문 ===
 1. [ORD-1] S-001 | 고객: LG이노텍 | 수량: 300
 2. [ORD-2] S-002 | 고객: SK하이닉스 | 수량: 50
```

**검증:** `renderIndexedList` 출력에 ` 1.`, ` 2.` 접두사 포함 여부 단위 테스트.

---

### Task 3 — OrderController::placeOrder 인덱스 선택

기존 `시료 ID:` 직접 입력 방식을 제거하고, 시료 목록을 `renderIndexedList`로 표시한 뒤 번호를 입력받는다.

```
[변경 전]
시료 ID: S-001
고객명: LG이노텍
수량: 300

[변경 후]
=== 시료 목록 ===
 1. [S-001] 실리콘 웨이퍼-8인치 | 재고: 480
 2. [S-002] GaN-on-SiC 100mm    | 재고: 120
시료 번호 선택: 1
고객명: LG이노텍
수량: 300
```

유효성 검사:
- 등록된 시료 없음 → `"등록된 시료가 없습니다."` 출력 후 반환
- 범위 밖 번호 또는 비정수 입력 → `"잘못된 번호입니다."` 출력 후 반환
- 고객명 빈 문자열 → `"고객명을 입력해 주세요."` 출력 후 반환
- 수량 1 미만 또는 비정수 → `"수량은 1 이상의 정수여야 합니다."` 출력 후 반환

`OrderController.h`에 `SampleView sampleView_` 멤버를 추가한다.

**검증:** 기존 `PlaceOrder_ValidSample_CreatesReservedOrder` 테스트 입력을 `"1\n홍길동\n10\n"`으로 수정. 유효성 검사 케이스(시료 없음, 범위 초과, 수량 0, 비정수) 각각 추가.

---

### Task 4 — SampleController 등록 유효성 검사

시료 등록(`choice == "1"`) 흐름에 단계별 검사를 추가한다.

| 입력 항목 | 검사 조건 | 오류 메시지 |
|-----------|-----------|-------------|
| 시료 ID | 빈 문자열 금지, 중복 ID 금지 | `"시료 ID를 입력해 주세요."` / `"이미 등록된 시료 ID입니다."` |
| 이름 | 빈 문자열 금지 | `"시료 이름을 입력해 주세요."` |
| 평균 생산 시간 | 0 초과 실수 | `"평균 생산 시간은 0보다 큰 수여야 합니다."` |
| 수율 | 0 초과 1 이하 실수 | `"수율은 0 초과 1 이하의 값이어야 합니다."` |
| 재고 | 0 이상 정수 | `"재고는 0 이상의 정수여야 합니다."` |

오류 발생 시 해당 루프 반복 처음으로 돌아간다 (`continue`).

**검증:** 중복 ID, yield > 1, yield = 0, 음수 재고, 비숫자 avgTime, avgTime = 0 각각 단위 테스트.

---

### Task 5 — OrderController 선택 입력 유효성 검사

`approveReject`와 `release`의 번호 선택 입력에 `tryParseInt` 적용.

```cpp
// 변경 전
int choice = std::stoi(choiceStr);
if (choice < 1 || choice > ...) { ... }

// 변경 후
auto choice = tryParseInt(choiceStr);
if (!choice || *choice < 1 || *choice > ...) {
    view_.renderMessage(out, "잘못된 번호입니다.");
    return;
}
```

**검증:** 비정수 입력(`"abc"`) 및 범위 초과(`"99"`) 각각 단위 테스트, 상태 변화 없음 확인.

---

## 커밋 순서

```
[feat] infra: add InputParser tryParseInt/tryParseDouble helpers
[test] infra: add InputParser unit tests
[feat en] ui: add numbered list rendering for sample/order selection
[test] ui: add renderIndexedList test
[feat en] order: replace sampleId prompt with indexed selection in placeOrder
[test] order: update and add placeOrder validation tests
[feat en] sample: add registration input validation
[test] sample: add registration validation tests
[feat en] order: add input validation to approveReject and release
[test] order: add approveReject/release invalid input tests
```
