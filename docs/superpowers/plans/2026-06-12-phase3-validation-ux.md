# Phase 3 — Validation + UX (인덱스 기반 선택) Implementation Plan

> **For agentic workers:** REQUIRED SUB-SKILL: Use superpowers:subagent-driven-development (recommended) or superpowers:executing-plans to implement this plan task-by-task. Steps use checkbox (`- [ ]`) syntax for tracking.

**Goal:** 모든 사용자 입력에 유효성 검사를 추가하고, 시료 주문 화면에서 ID 직접 입력 대신 번호 선택 방식으로 UX를 개선한다.

**Architecture:** `InputParser` 헤더-온리 헬퍼로 `stoi`/`stod` 예외를 `std::optional` 반환으로 래핑해 컨트롤러 전반에서 재사용한다. `SampleView::renderIndexedList`를 추가해 번호 선택 UI를 렌더링하고, `OrderController::placeOrder`는 sampleId 직접 입력 대신 이 뷰를 통해 인덱스를 받는다. OrderView의 기존 목록 메서드는 `1.`, `2.` 번호 접두사를 표시하도록 수정해 번호 선택 UX를 일관되게 만든다.

**Tech Stack:** C++20, MSVC v145, Google Test 1.11.0, nlohmann/json (기존)

---

## 파일 구조

| 파일 | 작업 | 역할 |
|------|------|------|
| `src/infra/InputParser.h` | **신규** | `tryParseInt` / `tryParseDouble` — `std::optional` 반환 헬퍼 |
| `src/view/SampleView.h` | **수정** | `renderIndexedList` 시그니처 추가 |
| `src/view/SampleView.cpp` | **수정** | `renderIndexedList` 구현 |
| `src/view/OrderView.cpp` | **수정** | `renderReservedList`, `renderConfirmedList` — 번호 접두사 추가 |
| `src/controller/OrderController.h` | **수정** | `SampleView sampleView_` 멤버 추가 |
| `src/controller/OrderController.cpp` | **수정** | `placeOrder` 인덱스 선택 + 전체 유효성 검사 |
| `src/controller/SampleController.cpp` | **수정** | 시료 등록 유효성 검사 추가 |
| `test/InputParserTest.cpp` | **신규** | `InputParser` 단위 테스트 |
| `test/OrderControllerTest.cpp` | **수정** | `placeOrder` 인덱스 UX 반영 + 유효성 검사 테스트 추가 |
| `test/SampleControllerTest.cpp` | **수정** | 등록 유효성 검사 테스트 추가 |
| `SampleOrderSystem-KHNam-CRA-3.vcxproj` | **수정** | `InputParserTest.cpp` ClCompile 항목 추가 |

---

## Task 1 — InputParser 헬퍼

**Files:**
- Create: `src/infra/InputParser.h`
- Create: `test/InputParserTest.cpp`
- Modify: `SampleOrderSystem-KHNam-CRA-3.vcxproj` (ClCompile 추가)

---

- [ ] **Step 1: 실패하는 테스트 작성**

`test/InputParserTest.cpp` 를 새로 만든다:

```cpp
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
```

- [ ] **Step 2: vcxproj에 새 테스트 파일 등록**

`SampleOrderSystem-KHNam-CRA-3.vcxproj`에서 기존 `<ClCompile Include="test\JsonSerializerTest.cpp" />` 라인 아래에 다음을 추가한다:

```xml
<ClCompile Include="test\InputParserTest.cpp" />
```

- [ ] **Step 3: 빌드해서 컴파일 오류 확인**

```
msbuild SampleOrderSystem-KHNam-CRA-3.slnx /p:Configuration=Debug /p:Platform=x64
```

예상: `InputParser.h` 없음으로 인한 컴파일 오류.

- [ ] **Step 4: InputParser 헬퍼 구현**

`src/infra/InputParser.h` 를 새로 만든다:

```cpp
#pragma once
#include <optional>
#include <string>
#include <string_view>

inline std::optional<int> tryParseInt(std::string_view s) {
    try { return std::stoi(std::string(s)); } catch (...) { return std::nullopt; }
}

inline std::optional<double> tryParseDouble(std::string_view s) {
    try { return std::stod(std::string(s)); } catch (...) { return std::nullopt; }
}
```

- [ ] **Step 5: 빌드 및 테스트 통과 확인**

```
msbuild SampleOrderSystem-KHNam-CRA-3.slnx /p:Configuration=Debug /p:Platform=x64
vstest.console.exe x64\Debug\SampleOrderSystem-KHNam-CRA-3.exe --TestCaseFilter:"FullyQualifiedName~InputParserTest"
```

예상: 5개 테스트 PASS.

- [ ] **Step 6: 커밋**

```
git add src/infra/InputParser.h test/InputParserTest.cpp SampleOrderSystem-KHNam-CRA-3.vcxproj
git commit -m "[feat] infra: add InputParser tryParseInt/tryParseDouble helpers"
```

---

## Task 2 — View: 번호 접두사

**Files:**
- Modify: `src/view/SampleView.h`
- Modify: `src/view/SampleView.cpp`
- Modify: `src/view/OrderView.cpp`

---

- [ ] **Step 1: SampleView 시그니처 추가**

`src/view/SampleView.h`에서 `renderList` 선언 아래에 다음을 추가한다:

```cpp
void renderIndexedList(std::ostream& os, const std::vector<Sample>& samples);
```

결과 (파일 전체):
```cpp
#pragma once
#include <ostream>
#include <string_view>
#include <vector>
#include "../domain/Sample.h"

class SampleView {
public:
    void renderList(std::ostream& os, const std::vector<Sample>& samples);
    void renderIndexedList(std::ostream& os, const std::vector<Sample>& samples);
    void renderMessage(std::ostream& os, std::string_view message);
};
```

- [ ] **Step 2: 실패하는 테스트 작성**

`test/SampleControllerTest.cpp` 파일 끝에 추가한다:

```cpp
#include "../src/view/SampleView.h"

TEST(SampleViewTest, RenderIndexedList_ShowsNumberedEntries) {
    SampleView view;
    std::vector<Sample> samples = {
        {"S-001", "Silicon",   0.5, 0.92, 100},
        {"S-002", "Germanium", 1.0, 0.85,  50},
    };
    std::ostringstream out;
    view.renderIndexedList(out, samples);
    std::string s = out.str();
    EXPECT_NE(s.find(" 1."), std::string::npos);
    EXPECT_NE(s.find(" 2."), std::string::npos);
    EXPECT_NE(s.find("S-001"), std::string::npos);
    EXPECT_NE(s.find("S-002"), std::string::npos);
}
```

- [ ] **Step 3: 빌드 → 링크 오류(미구현) 확인**

```
msbuild SampleOrderSystem-KHNam-CRA-3.slnx /p:Configuration=Debug /p:Platform=x64
```

예상: `renderIndexedList` 미구현으로 링크 오류.

- [ ] **Step 4: SampleView::renderIndexedList 구현**

`src/view/SampleView.cpp` 끝에 추가한다:

```cpp
void SampleView::renderIndexedList(std::ostream& os, const std::vector<Sample>& samples) {
    os << "=== 시료 목록 ===\n";
    for (int i = 0; i < static_cast<int>(samples.size()); ++i) {
        const auto& s = samples[i];
        os << " " << (i + 1) << ". [" << s.sampleId << "] " << s.name
           << " | 재고: " << s.stock << "\n";
    }
}
```

- [ ] **Step 5: OrderView 번호 접두사 추가**

`src/view/OrderView.cpp`의 `renderReservedList`를 아래와 같이 교체한다:

```cpp
void OrderView::renderReservedList(std::ostream& os, const std::vector<Order>& orders) {
    os << "=== 승인 대기 주문 ===\n";
    for (int i = 0; i < static_cast<int>(orders.size()); ++i) {
        const auto& o = orders[i];
        os << " " << (i + 1) << ". [" << o.orderId << "] " << o.sampleId
           << " | 고객: " << o.customerName
           << " | 수량: " << o.quantity << "\n";
    }
}
```

`renderConfirmedList`도 동일하게 교체한다:

```cpp
void OrderView::renderConfirmedList(std::ostream& os, const std::vector<Order>& orders) {
    os << "=== 출고 대기 주문 ===\n";
    for (int i = 0; i < static_cast<int>(orders.size()); ++i) {
        const auto& o = orders[i];
        os << " " << (i + 1) << ". [" << o.orderId << "] " << o.sampleId
           << " | 고객: " << o.customerName
           << " | 수량: " << o.quantity << "\n";
    }
}
```

- [ ] **Step 6: 빌드 및 테스트 통과 확인**

```
msbuild SampleOrderSystem-KHNam-CRA-3.slnx /p:Configuration=Debug /p:Platform=x64
vstest.console.exe x64\Debug\SampleOrderSystem-KHNam-CRA-3.exe --TestCaseFilter:"FullyQualifiedName~SampleViewTest"
```

예상: 1개 테스트 PASS, 기존 테스트 전체 유지.

- [ ] **Step 7: 커밋**

```
git add src/view/SampleView.h src/view/SampleView.cpp src/view/OrderView.cpp test/SampleControllerTest.cpp
git commit -m "[feat en] ui: add numbered list rendering for sample/order selection"
```

---

## Task 3 — OrderController::placeOrder 인덱스 선택 + 유효성 검사

**Files:**
- Modify: `src/controller/OrderController.h`
- Modify: `src/controller/OrderController.cpp`
- Modify: `test/OrderControllerTest.cpp`

---

- [ ] **Step 1: 기존 PlaceOrder 테스트 업데이트**

`test/OrderControllerTest.cpp`에서 `PlaceOrder_ValidSample_CreatesReservedOrder`를 아래와 같이 교체한다 (sampleId 문자열 입력 → 인덱스 `"1"` 입력):

```cpp
TEST(OrderControllerTest, PlaceOrder_ValidSample_CreatesReservedOrder) {
    SampleModel sm; sm.add({"S-001", "Silicon", 0.5, 0.92, 100});
    OrderModel om; ProductionQueue q;
    OrderController ctrl(sm, om, q);
    std::istringstream in("1\n홍길동\n10\n"); // 인덱스 1 선택
    std::ostringstream out;
    ctrl.placeOrder(in, out);
    auto reserved = om.getByStatus(OrderStatus::RESERVED);
    ASSERT_EQ(reserved.size(), 1u);
    EXPECT_EQ(reserved[0].quantity, 10);
    EXPECT_EQ(reserved[0].sampleId, "S-001");
}
```

같은 파일 끝에 유효성 검사 테스트를 추가한다:

```cpp
TEST(OrderControllerTest, PlaceOrder_NoSamples_ShowsMessage) {
    SampleModel sm; OrderModel om; ProductionQueue q;
    OrderController ctrl(sm, om, q);
    std::istringstream in("");
    std::ostringstream out;
    ctrl.placeOrder(in, out);
    EXPECT_NE(out.str().find("등록된 시료가 없습니다"), std::string::npos);
    EXPECT_TRUE(om.getAll().empty());
}

TEST(OrderControllerTest, PlaceOrder_InvalidIndex_ShowsMessage) {
    SampleModel sm; sm.add({"S-001", "Silicon", 0.5, 0.92, 100});
    OrderModel om; ProductionQueue q;
    OrderController ctrl(sm, om, q);
    std::istringstream in("99\n");
    std::ostringstream out;
    ctrl.placeOrder(in, out);
    EXPECT_NE(out.str().find("잘못된"), std::string::npos);
    EXPECT_TRUE(om.getAll().empty());
}

TEST(OrderControllerTest, PlaceOrder_NonNumericIndex_ShowsMessage) {
    SampleModel sm; sm.add({"S-001", "Silicon", 0.5, 0.92, 100});
    OrderModel om; ProductionQueue q;
    OrderController ctrl(sm, om, q);
    std::istringstream in("abc\n");
    std::ostringstream out;
    ctrl.placeOrder(in, out);
    EXPECT_NE(out.str().find("잘못된"), std::string::npos);
    EXPECT_TRUE(om.getAll().empty());
}

TEST(OrderControllerTest, PlaceOrder_ZeroQty_ShowsMessage) {
    SampleModel sm; sm.add({"S-001", "Silicon", 0.5, 0.92, 100});
    OrderModel om; ProductionQueue q;
    OrderController ctrl(sm, om, q);
    std::istringstream in("1\n홍길동\n0\n");
    std::ostringstream out;
    ctrl.placeOrder(in, out);
    EXPECT_NE(out.str().find("1 이상"), std::string::npos);
    EXPECT_TRUE(om.getAll().empty());
}

TEST(OrderControllerTest, PlaceOrder_NonNumericQty_ShowsMessage) {
    SampleModel sm; sm.add({"S-001", "Silicon", 0.5, 0.92, 100});
    OrderModel om; ProductionQueue q;
    OrderController ctrl(sm, om, q);
    std::istringstream in("1\n홍길동\nabc\n");
    std::ostringstream out;
    ctrl.placeOrder(in, out);
    EXPECT_NE(out.str().find("1 이상"), std::string::npos);
    EXPECT_TRUE(om.getAll().empty());
}
```

- [ ] **Step 2: 빌드 → 테스트 실패 확인**

```
msbuild SampleOrderSystem-KHNam-CRA-3.slnx /p:Configuration=Debug /p:Platform=x64
vstest.console.exe x64\Debug\SampleOrderSystem-KHNam-CRA-3.exe --TestCaseFilter:"FullyQualifiedName~PlaceOrder"
```

예상: `PlaceOrder_ValidSample_CreatesReservedOrder`가 FAIL (입력 형식 불일치).

- [ ] **Step 3: OrderController.h에 SampleView 멤버 추가**

`src/controller/OrderController.h`에서 `#include "../view/OrderView.h"` 아래에 추가:

```cpp
#include "../view/SampleView.h"
```

`private:` 섹션의 `OrderView view_;` 아래에 추가:

```cpp
SampleView sampleView_;
```

결과 (파일 전체):
```cpp
#pragma once
#include <istream>
#include <ostream>
#include "../model/SampleModel.h"
#include "../model/OrderModel.h"
#include "../model/ProductionQueue.h"
#include "../view/OrderView.h"
#include "../view/SampleView.h"

class OrderController {
public:
    OrderController(SampleModel& sm, OrderModel& om, ProductionQueue& q);
    void placeOrder(std::istream& in, std::ostream& out);
    void approveReject(std::istream& in, std::ostream& out);
    void release(std::istream& in, std::ostream& out);
private:
    SampleModel& sampleModel_;
    OrderModel& orderModel_;
    ProductionQueue& queue_;
    OrderView view_;
    SampleView sampleView_;
};
```

- [ ] **Step 4: OrderController.cpp — placeOrder 재구현**

`src/controller/OrderController.cpp`의 `#include "OrderController.h"` 아래에 추가:

```cpp
#include "../infra/InputParser.h"
```

`placeOrder` 함수 전체를 아래와 같이 교체한다:

```cpp
void OrderController::placeOrder(std::istream& in, std::ostream& out) {
    auto samples = sampleModel_.getAll();
    if (samples.empty()) {
        view_.renderMessage(out, "등록된 시료가 없습니다.");
        return;
    }

    sampleView_.renderIndexedList(out, samples);
    out << "시료 번호 선택: ";
    std::string indexStr;
    std::getline(in, indexStr);
    auto idx = tryParseInt(indexStr);
    if (!idx || *idx < 1 || *idx > static_cast<int>(samples.size())) {
        view_.renderMessage(out, "잘못된 번호입니다.");
        return;
    }
    const std::string& sampleId = samples[*idx - 1].sampleId;

    out << "고객명: ";
    std::string customerName;
    std::getline(in, customerName);
    if (customerName.empty()) {
        view_.renderMessage(out, "고객명을 입력해 주세요.");
        return;
    }

    out << "수량: ";
    std::string qtyStr;
    std::getline(in, qtyStr);
    auto qty = tryParseInt(qtyStr);
    if (!qty || *qty < 1) {
        view_.renderMessage(out, "수량은 1 이상의 정수여야 합니다.");
        return;
    }

    std::string orderId = orderModel_.create(sampleId, customerName, *qty);
    view_.renderMessage(out, "주문 접수 완료. 주문 ID: " + orderId);
}
```

- [ ] **Step 5: 빌드 및 테스트 통과 확인**

```
msbuild SampleOrderSystem-KHNam-CRA-3.slnx /p:Configuration=Debug /p:Platform=x64
vstest.console.exe x64\Debug\SampleOrderSystem-KHNam-CRA-3.exe --TestCaseFilter:"FullyQualifiedName~PlaceOrder"
```

예상: 6개 테스트 전부 PASS.

- [ ] **Step 6: 전체 테스트 회귀 확인**

```
vstest.console.exe x64\Debug\SampleOrderSystem-KHNam-CRA-3.exe
```

예상: 기존 테스트 포함 전부 PASS (실패 0).

- [ ] **Step 7: 커밋**

```
git add src/controller/OrderController.h src/controller/OrderController.cpp test/OrderControllerTest.cpp
git commit -m "[feat en] order: replace sampleId prompt with indexed selection in placeOrder"
```

---

## Task 4 — SampleController 등록 유효성 검사

**Files:**
- Modify: `src/controller/SampleController.cpp`
- Modify: `test/SampleControllerTest.cpp`

---

- [ ] **Step 1: 실패하는 테스트 작성**

`test/SampleControllerTest.cpp` 끝에 추가한다:

```cpp
TEST(SampleControllerTest, Register_DuplicateId_ShowsError) {
    SampleModel model;
    model.add({"S-001", "Silicon", 0.5, 0.92, 100});
    SampleController controller(model);
    std::istringstream in("1\nS-001\nDuplicate\n0.5\n0.92\n50\n0\n");
    std::ostringstream out;
    controller.run(in, out);
    EXPECT_EQ(model.getAll().size(), 1u); // 추가 없음
    EXPECT_NE(out.str().find("이미 등록"), std::string::npos);
}

TEST(SampleControllerTest, Register_InvalidYield_TooHigh_ShowsError) {
    SampleModel model;
    SampleController controller(model);
    std::istringstream in("1\nS-002\nNew\n0.5\n1.5\n50\n0\n");
    std::ostringstream out;
    controller.run(in, out);
    EXPECT_TRUE(model.getAll().empty());
    EXPECT_NE(out.str().find("수율"), std::string::npos);
}

TEST(SampleControllerTest, Register_InvalidYield_Zero_ShowsError) {
    SampleModel model;
    SampleController controller(model);
    std::istringstream in("1\nS-002\nNew\n0.5\n0\n50\n0\n");
    std::ostringstream out;
    controller.run(in, out);
    EXPECT_TRUE(model.getAll().empty());
    EXPECT_NE(out.str().find("수율"), std::string::npos);
}

TEST(SampleControllerTest, Register_NegativeStock_ShowsError) {
    SampleModel model;
    SampleController controller(model);
    std::istringstream in("1\nS-002\nNew\n0.5\n0.9\n-1\n0\n");
    std::ostringstream out;
    controller.run(in, out);
    EXPECT_TRUE(model.getAll().empty());
    EXPECT_NE(out.str().find("재고"), std::string::npos);
}

TEST(SampleControllerTest, Register_NonNumericAvgTime_ShowsError) {
    SampleModel model;
    SampleController controller(model);
    std::istringstream in("1\nS-002\nNew\nabc\n0\n");
    std::ostringstream out;
    controller.run(in, out);
    EXPECT_TRUE(model.getAll().empty());
    EXPECT_NE(out.str().find("생산 시간"), std::string::npos);
}

TEST(SampleControllerTest, Register_ZeroAvgTime_ShowsError) {
    SampleModel model;
    SampleController controller(model);
    std::istringstream in("1\nS-002\nNew\n0\n0\n");
    std::ostringstream out;
    controller.run(in, out);
    EXPECT_TRUE(model.getAll().empty());
    EXPECT_NE(out.str().find("생산 시간"), std::string::npos);
}
```

- [ ] **Step 2: 빌드 → 테스트 실패 확인**

```
msbuild SampleOrderSystem-KHNam-CRA-3.slnx /p:Configuration=Debug /p:Platform=x64
vstest.console.exe x64\Debug\SampleOrderSystem-KHNam-CRA-3.exe --TestCaseFilter:"FullyQualifiedName~SampleControllerTest"
```

예상: 새 테스트 6개 FAIL.

- [ ] **Step 3: SampleController.cpp 유효성 검사 구현**

`src/controller/SampleController.cpp`의 `#include "SampleController.h"` 아래에 추가:

```cpp
#include "../infra/InputParser.h"
```

`choice == "1"` 블록 전체를 아래와 같이 교체한다:

```cpp
} else if (choice == "1") {
    Sample s;
    out << "시료 ID: ";
    std::getline(in, s.sampleId);
    if (s.sampleId.empty()) {
        view_.renderMessage(out, "시료 ID를 입력해 주세요.");
        continue;
    }
    out << "이름: ";
    std::getline(in, s.name);
    if (s.name.empty()) {
        view_.renderMessage(out, "시료 이름을 입력해 주세요.");
        continue;
    }
    out << "평균 생산 시간 (분): ";
    std::string tmp;
    std::getline(in, tmp);
    auto avgTime = tryParseDouble(tmp);
    if (!avgTime || *avgTime <= 0.0) {
        view_.renderMessage(out, "평균 생산 시간은 0보다 큰 수여야 합니다.");
        continue;
    }
    s.avgProductionTime = *avgTime;
    out << "수율 (0 초과 1 이하): ";
    std::getline(in, tmp);
    auto yield = tryParseDouble(tmp);
    if (!yield || *yield <= 0.0 || *yield > 1.0) {
        view_.renderMessage(out, "수율은 0 초과 1 이하의 값이어야 합니다.");
        continue;
    }
    s.yield = *yield;
    out << "재고: ";
    std::getline(in, tmp);
    auto stock = tryParseInt(tmp);
    if (!stock || *stock < 0) {
        view_.renderMessage(out, "재고는 0 이상의 정수여야 합니다.");
        continue;
    }
    s.stock = *stock;
    if (!model_.add(s)) {
        view_.renderMessage(out, "이미 등록된 시료 ID입니다.");
        continue;
    }
    view_.renderMessage(out, "시료가 등록되었습니다.");
```

- [ ] **Step 4: 빌드 및 테스트 통과 확인**

```
msbuild SampleOrderSystem-KHNam-CRA-3.slnx /p:Configuration=Debug /p:Platform=x64
vstest.console.exe x64\Debug\SampleOrderSystem-KHNam-CRA-3.exe --TestCaseFilter:"FullyQualifiedName~SampleControllerTest"
```

예상: 전체 PASS (기존 3개 + 신규 6개 = 9개).

- [ ] **Step 5: 커밋**

```
git add src/controller/SampleController.cpp test/SampleControllerTest.cpp
git commit -m "[feat en] sample: add registration input validation"
```

---

## Task 5 — OrderController approveReject/release 입력 유효성 검사

**Files:**
- Modify: `src/controller/OrderController.cpp`
- Modify: `test/OrderControllerTest.cpp`

---

- [ ] **Step 1: 실패하는 테스트 작성**

`test/OrderControllerTest.cpp` 끝에 추가한다:

```cpp
TEST(OrderControllerTest, ApproveReject_NonNumericChoice_ShowsMessage) {
    SampleModel sm; sm.add({"S-001", "Silicon", 0.5, 0.92, 100});
    OrderModel om; om.create("S-001", "홍길동", 10);
    ProductionQueue q;
    OrderController ctrl(sm, om, q);
    std::istringstream in("abc\n");
    std::ostringstream out;
    ctrl.approveReject(in, out);
    EXPECT_NE(out.str().find("잘못된"), std::string::npos);
    EXPECT_EQ(om.getByStatus(OrderStatus::RESERVED).size(), 1u); // 상태 변화 없음
}

TEST(OrderControllerTest, ApproveReject_OutOfRangeChoice_ShowsMessage) {
    SampleModel sm; sm.add({"S-001", "Silicon", 0.5, 0.92, 100});
    OrderModel om; om.create("S-001", "홍길동", 10);
    ProductionQueue q;
    OrderController ctrl(sm, om, q);
    std::istringstream in("99\n");
    std::ostringstream out;
    ctrl.approveReject(in, out);
    EXPECT_NE(out.str().find("잘못된"), std::string::npos);
    EXPECT_EQ(om.getByStatus(OrderStatus::RESERVED).size(), 1u);
}

TEST(OrderControllerTest, Release_NonNumericChoice_ShowsMessage) {
    SampleModel sm; sm.add({"S-001", "Silicon", 0.5, 0.92, 100});
    OrderModel om;
    std::string id = om.create("S-001", "홍길동", 30);
    om.transitionTo(id, OrderStatus::CONFIRMED);
    ProductionQueue q;
    OrderController ctrl(sm, om, q);
    std::istringstream in("abc\n");
    std::ostringstream out;
    ctrl.release(in, out);
    EXPECT_NE(out.str().find("잘못된"), std::string::npos);
    EXPECT_EQ(om.getByStatus(OrderStatus::CONFIRMED).size(), 1u); // 상태 변화 없음
}
```

- [ ] **Step 2: 빌드 → 테스트 실패 확인**

```
msbuild SampleOrderSystem-KHNam-CRA-3.slnx /p:Configuration=Debug /p:Platform=x64
vstest.console.exe x64\Debug\SampleOrderSystem-KHNam-CRA-3.exe --TestCaseFilter:"FullyQualifiedName~ApproveReject_NonNumeric|FullyQualifiedName~ApproveReject_OutOfRange|FullyQualifiedName~Release_NonNumeric"
```

예상: 3개 FAIL (stoi 예외 발생으로 크래시 또는 잘못된 메시지).

- [ ] **Step 3: approveReject 유효성 검사 적용**

`src/controller/OrderController.cpp`의 `approveReject` 함수에서 `stoi(choiceStr)` 라인을 `tryParseInt` 기반으로 교체한다:

기존:
```cpp
    int choice = std::stoi(choiceStr);

    if (choice < 1 || choice > static_cast<int>(reserved.size())) {
        view_.renderMessage(out, "잘못된 선택입니다.");
        return;
    }
```

교체 후:
```cpp
    auto choice = tryParseInt(choiceStr);
    if (!choice || *choice < 1 || *choice > static_cast<int>(reserved.size())) {
        view_.renderMessage(out, "잘못된 번호입니다.");
        return;
    }
```

같은 함수에서 `reserved[choice - 1]`을 `reserved[*choice - 1]`로 수정한다:

```cpp
    const Order& order = reserved[*choice - 1];
```

- [ ] **Step 4: release 유효성 검사 적용**

`release` 함수에서 동일하게 교체한다:

기존:
```cpp
    int choice = std::stoi(choiceStr);

    if (choice < 1 || choice > static_cast<int>(confirmed.size())) {
        view_.renderMessage(out, "잘못된 선택입니다.");
        return;
    }

    const Order& order = confirmed[choice - 1];
```

교체 후:
```cpp
    auto choice = tryParseInt(choiceStr);
    if (!choice || *choice < 1 || *choice > static_cast<int>(confirmed.size())) {
        view_.renderMessage(out, "잘못된 번호입니다.");
        return;
    }

    const Order& order = confirmed[*choice - 1];
```

- [ ] **Step 5: 빌드 및 전체 테스트 통과 확인**

```
msbuild SampleOrderSystem-KHNam-CRA-3.slnx /p:Configuration=Debug /p:Platform=x64
vstest.console.exe x64\Debug\SampleOrderSystem-KHNam-CRA-3.exe
```

예상: 전체 테스트 PASS (실패 0).

- [ ] **Step 6: 커밋**

```
git add src/controller/OrderController.cpp test/OrderControllerTest.cpp
git commit -m "[feat en] order: add input validation to approveReject and release"
```

---

## 완료 체크리스트

| 항목 | 확인 |
|------|------|
| `tryParseInt` / `tryParseDouble` 사용 — stoi/stod 직접 호출 없음 | |
| 시료 주문 시 번호 선택 (인덱스) 방식으로 동작 | |
| 승인/거절 목록에 `1.`, `2.` 번호 접두사 표시 | |
| 출고 목록에 `1.`, `2.` 번호 접두사 표시 | |
| 시료 등록: yield, avgTime, stock, 중복 ID 모두 검사 | |
| 모든 오류 메시지 한국어 | |
| 기존 테스트 전부 PASS (회귀 없음) | |
| 신규 테스트 추가: InputParser 5 + SampleView 1 + SampleController 6 + OrderController 5 = 17개 | |
