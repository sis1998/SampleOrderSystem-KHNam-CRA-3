# Phase 1 — MVC 셸 + 인메모리 도메인

**목표:** JSON 없이 인메모리 MVC로 6개 메뉴 전부 동작  
**브랜치:** `phase1/console-mvc`

---

## 목차

- [파일 구조](#파일-구조)
- [Task 0 — 브랜치 생성](#task-0--브랜치-생성)
- [Task 1 — 도메인 구조체](#task-1--도메인-구조체)
- [Task 2 — SampleModel](#task-2--samplemodel)
- [Task 3 — OrderModel](#task-3--ordermodel)
- [Task 4 — ConsoleSetup](#task-4--consolesetup)
- [Task 5 — Views](#task-5--views)
- [Task 6 — SampleController](#task-6--samplecontroller)
- [Task 7 — OrderController](#task-7--ordercontroller)
- [Task 8 — ProductionQueue](#task-8--productionqueue)
- [Task 9 — MonitorController + ProductionController](#task-9--monitorcontroller--productioncontroller)
- [Task 10 — AppController + main.cpp](#task-10--appcontroller--maincpp)
- [Task 11 — 통합 스모크 테스트](#task-11--통합-스모크-테스트)

---

## 파일 구조

```
src/
  main.cpp
  infra/   ConsoleSetup.h/.cpp          UTF-8 코드페이지 설정
  domain/  Sample.h  Order.h  Production.h   순수 데이터 구조체
  model/   SampleModel  OrderModel  ProductionQueue
  view/    MainView  SampleView  OrderView  MonitorView  ProductionView
  controller/  AppController  SampleController  OrderController
               MonitorController  ProductionController
test/
  SampleModelTest.cpp
  OrderModelTest.cpp
  ProductionQueueTest.cpp
  MainViewTest.cpp
  SampleControllerTest.cpp
  OrderControllerTest.cpp
```

> 파일 추가할 때마다 vcxproj `<ClCompile>` / `<ClInclude>` 항목 추가 필수.

---

## Task 0 — 브랜치 생성

```bash
git checkout -b phase1/console-mvc
```

커밋: 없음

---

## Task 1 — 도메인 구조체

**생성:** `src/domain/Sample.h`, `Order.h`, `Production.h`

| 파일 | 내용 |
|------|------|
| `Sample.h` | `sampleId`, `name`, `avgProductionTime`, `yield`, `stock` |
| `Order.h` | `OrderStatus` enum (RESERVED/REJECTED/PRODUCING/CONFIRMED/RELEASE), `Order` 구조체 |
| `Production.h` | `ProductionItem` (현재 처리 중), `QueueItem` (대기) |

테스트: 없음 (순수 구조체)  
커밋: `[feat] domain: add Sample, Order, Production structs`

---

## Task 2 — SampleModel

**생성:** `src/model/SampleModel.h/.cpp`, `test/SampleModelTest.cpp`

테스트 케이스:
- `Add_StoresAndRetrievesById` — findById 반환 검증
- `Add_DuplicateId_ReturnsFalse` — 중복 sampleId 거부
- `FindByName_ReturnsSubstringMatch` — 부분 문자열, 대소문자 무관
- `DeductStock_ReducesStock` — 차감 후 stock 검증
- `DeductStock_Insufficient_ReturnsFalse` — 재고 부족 시 false
- `AddStock_IncreasesStock` — 생산 완료 시 보충

커밋: `[test] model: add SampleModel tests` → `[feat] model: implement SampleModel`

---

## Task 3 — OrderModel

**생성:** `src/model/OrderModel.h/.cpp`, `test/OrderModelTest.cpp`

테스트 케이스:
- `Create_FirstOrder_ReturnsORD1` — 주문번호 형식 검증
- `Create_SecondOrder_ReturnsORD2` — 순번 증가
- `GetByStatus_ReturnsOnlyMatchingStatus`
- `TransitionTo_ValidTransitions` — 허용 전이 5종 각각
- `TransitionTo_InvalidTransition_ReturnsFalse` — 허용 외 전이 거부
- `TerminalStates_CannotTransition` — REJECTED·RELEASE는 전이 불가

커밋: `[test] model: add OrderModel tests` → `[feat] model: implement OrderModel`

---

## Task 4 — ConsoleSetup

**생성:** `src/infra/ConsoleSetup.h/.cpp`

`ConsoleSetup::apply()` — `SetConsoleOutputCP(CP_UTF8)` + `SetConsoleCP(CP_UTF8)`

테스트: 없음 (Win32 API; Task 10 통합 테스트에서 확인)  
커밋: `[feat] infra: add ConsoleSetup for UTF-8 codepage`

---

## Task 5 — Views

**생성:** 5개 View 쌍 (`.h/.cpp`), `test/MainViewTest.cpp`

모든 View 메서드는 `std::ostream&` 주입 방식 — `std::ostringstream`으로 단위 테스트 가능.

| View | 주요 메서드 |
|------|-------------|
| `MainView` | `renderHeader(out, sampleCount, totalStock, orderCount, queueSize)`, `renderMenu(out)` |
| `SampleView` | `renderList(out, samples)`, `renderMessage(out, msg)` |
| `OrderView` | `renderReservedList`, `renderConfirmedList`, `renderMessage` |
| `MonitorView` | `renderOrderCounts(out, map<OrderStatus,int>)`, `renderStockStatus(out, samples, orders)` |
| `ProductionView` | `renderQueue(out, current, waiting)` |

테스트 케이스 (`MainViewTest`):
- `RenderMenu_ContainsAllSixItems` — 출력에 "1."~"6." 및 "0." 포함
- `RenderHeader_ContainsCounts` — 전달한 숫자가 출력에 포함

커밋: `[test] view: add MainView tests` → `[feat] view: add all views`

---

## Task 6 — SampleController

**생성:** `src/controller/SampleController.h/.cpp`, `test/SampleControllerTest.cpp`

`SampleController(SampleModel&)` — `run(istream&, ostream&)` 서브메뉴 루프  
서브메뉴: 1=등록, 2=목록, 3=검색, 0=뒤로

테스트 케이스:
- `Input0_ExitsImmediately` — "0\n" 입력 시 즉시 반환
- `Input1_AddsValidSample` — 유효 입력으로 model에 시료 추가
- `Input2_ListsAllSamples` — 출력에 모든 시료 포함

커밋: `[test] controller: add SampleController tests` → `[feat] controller: implement SampleController`

---

## Task 7 — OrderController

**생성:** `src/controller/OrderController.h/.cpp`, `test/OrderControllerTest.cpp`

`OrderController(SampleModel&, OrderModel&, ProductionQueue&)` — 메뉴 2·3·6 담당

| 메서드 | 동작 |
|--------|------|
| `placeOrder(in, out)` | sampleId·고객명·수량 입력 → RESERVED 생성 |
| `approveReject(in, out)` | RESERVED 목록 → 승인 시 재고 확인 → CONFIRMED 또는 PRODUCING+enqueue |
| `release(in, out)` | CONFIRMED 목록 → 재고 차감 → RELEASE |

승인 로직:
- 재고 ≥ 수량 → `deductStock` + `transitionTo(CONFIRMED)`
- 재고 < 수량 → `transitionTo(PRODUCING)` + `queue.enqueue(orderId, shortfall, ...)`

테스트 케이스:
- `PlaceOrder_ValidSample_CreatesReservedOrder`
- `Approve_StockSufficient_TransitionsToConfirmed`
- `Approve_StockInsufficient_TransitionsToProducing`
- `Reject_TransitionsToRejected`
- `Release_ConfirmedOrder_TransitionsToRelease`

커밋: `[test] controller: add OrderController tests` → `[feat] controller: implement OrderController`

---

## Task 8 — ProductionQueue

**생성:** `src/model/ProductionQueue.h/.cpp`, `test/ProductionQueueTest.cpp`

핵심 공식:  
`actualQty = ceil(shortfall / (yield × 0.9))`  
`estimatedMinutes = avgProductionTime × actualQty`

| 메서드 | 동작 |
|--------|------|
| `enqueue(orderId, sampleId, shortfall, avgTime, yield)` | actualQty 계산 후 대기열 추가 |
| `tick(SampleModel&, OrderModel&) → bool` | 대기 → 시작 / 완료 시 addStock + CONFIRMED 전환, true 반환 |
| `getCurrent()` | `optional<ProductionItem>` |
| `getWaiting()` | `deque<QueueItem>` |

테스트 케이스:
- `Enqueue_ComputesActualQtyWithSafetyMargin` — `ceil(shortfall/(yield*0.9))` 검증
- `Enqueue_MultipleOrders_PreservesFIFO`
- `Tick_WhenWaiting_StartsNextItem`
- `Tick_WhenElapsed_CompletesAndUpdatesModelState` — 매우 짧은 estimatedMinutes + 100ms sleep

커밋: `[test] model: add ProductionQueue tests` → `[feat] model: implement ProductionQueue`

---

## Task 9 — MonitorController + ProductionController

**생성:** 각 `.h/.cpp`

- `MonitorController::run(in, out)` — OrderModel·SampleModel에서 읽어 MonitorView 렌더링, REJECTED 제외
- `ProductionController::run(in, out)` — ProductionQueue에서 읽어 ProductionView 렌더링

테스트: 없음 (순수 렌더링)  
커밋: `[feat] controller: add MonitorController and ProductionController`

---

## Task 10 — AppController + main.cpp

**생성:** `src/controller/AppController.h/.cpp`, `src/main.cpp`

`AppController` 소유 관계:
- `SampleModel`, `OrderModel`, `ProductionQueue` (값으로 보유)
- 모든 서브 컨트롤러 (참조 전달)
- `MainView`

`run()` 루프:
```
while (true) {
    productionQueue_.tick(sampleModel_, orderModel_);  // 생산 완료 체크
    mainView_.renderHeader(cout, ...);
    mainView_.renderMenu(cout);
    getline(cin, input);
    dispatch(input);  // 1~6 → 각 서브컨트롤러, 0 → break
}
```

`main.cpp`:
```cpp
ConsoleSetup::apply();
AppController app;
app.run();
```

시드 데이터 (AppController ctor 안에):
```
S-001  실리콘 웨이퍼-8인치   avgTime=0.5  yield=0.92  stock=480
S-002  GaAs 기판-4인치       avgTime=1.2  yield=0.85  stock=120
S-003  SiC 웨이퍼-6인치      avgTime=2.0  yield=0.78  stock=60
```

vcxproj + slnx 모두 편집.  
커밋: `[feat] controller: add AppController and main entry point`

---

## Task 11 — 통합 스모크 테스트

빌드 후 직접 실행해서 아래 경로 검증:

- [ ] 한글 텍스트 깨짐 없음
- [ ] 시료 관리 → 목록: 시드 3개 표시
- [ ] 시료 주문 → 수량 충분 → 승인 → CONFIRMED 전환 확인
- [ ] 출고 처리 → RELEASE 전환 확인
- [ ] 시료 주문 → 재고 초과 수량 → 승인 → PRODUCING + 생산라인 표시
- [ ] 생산 완료 대기 → 메인 루프에서 자동 CONFIRMED 전환 확인
- [ ] 0 입력으로 정상 종료

커밋: `[feat] controller: add seed data, complete Phase 1`
