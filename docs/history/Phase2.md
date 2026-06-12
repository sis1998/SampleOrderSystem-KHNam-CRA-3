# Phase 2 — JSON 영속화

**브랜치:** `phase2/json-persistence` (master에서 분기)  
**목표:** 앱 재시작 후 데이터 유지 — samples / orders / production_queue 3개 파일

---

## 목차

1. [파일 구조](#파일-구조)
2. [Task 1 — nlohmann/json 추가](#task-1--nlohmannjson-추가)
3. [Task 2 — JsonSerializer](#task-2--jsonserializer)
4. [Task 3 — SampleRepository](#task-3--samplerepository)
5. [Task 4 — OrderRepository](#task-4--orderrepository)
6. [Task 5 — ProductionQueueRepository](#task-5--productionqueuerepository)
7. [Task 6 — AppController 연결](#task-6--appcontroller-연결)
8. [커밋 순서](#커밋-순서)

---

## 파일 구조

```
data/
  samples.json
  orders.json
  production_queue.json
src/
  infra/
    JsonSerializer.h/.cpp   # Sample·Order·QueueItem ↔ JSON 문자열 변환
  repository/
    SampleRepository.h/.cpp
    OrderRepository.h/.cpp
    ProductionQueueRepository.h/.cpp
```

---

## Task 목록

### Task 1 — nlohmann/json 추가

NuGet으로 `nlohmann.json` 패키지 설치, vcxproj에 include 경로 추가.

**검증:** `#include <nlohmann/json.hpp>` 포함 후 빌드 성공.

---

### Task 2 — JsonSerializer

`src/infra/JsonSerializer.h/.cpp` — Sample, Order, QueueItem, ProductionItem 각각에 대해 `to_json` / `from_json` 자유 함수 구현.

```cpp
nlohmann::json toJson(const Sample&);
Sample sampleFromJson(const nlohmann::json&);

nlohmann::json toJson(const Order&);
Order orderFromJson(const nlohmann::json&);

nlohmann::json toJson(const QueueItem&);
QueueItem queueItemFromJson(const nlohmann::json&);
```

OrderStatus ↔ 문자열 매핑: `"RESERVED"`, `"REJECTED"`, `"PRODUCING"`, `"CONFIRMED"`, `"RELEASE"`.  
`ProductionItem.startedAt`은 경과 분(`elapsed_min` double)으로 직렬화.

**검증 (단위 테스트):**
```cpp
Sample s{"S-001","Silicon",0.5,0.92,100};
EXPECT_EQ(sampleFromJson(toJson(s)).stock, 100);

Order o; o.orderId="ORD-1"; o.status=OrderStatus::CONFIRMED;
EXPECT_EQ(orderFromJson(toJson(o)).status, OrderStatus::CONFIRMED);
```

---

### Task 3 — SampleRepository

```cpp
class SampleRepository {
public:
    void load(SampleModel& model, const std::string& path);
    void save(const SampleModel& model, const std::string& path);
};
```

- `load`: 파일 없으면 아무것도 하지 않음 (첫 실행 허용)
- `save`: `data/samples.json` 덮어쓰기

**검증:**
```cpp
SampleRepository repo;
SampleModel m; m.add({"S-001","Silicon",0.5,0.92,100});
repo.save(m, "test_samples.json");
SampleModel m2; repo.load(m2, "test_samples.json");
EXPECT_EQ(m2.findById("S-001")->stock, 100);
```

---

### Task 4 — OrderRepository

```cpp
class OrderRepository {
public:
    void load(OrderModel& model, const std::string& path);
    void save(const OrderModel& model, const std::string& path);
};
```

`nextOrderId` 포함 직렬화.

**검증:** 주문 2건 저장 → 로드 → ID·상태·수량 일치.

---

### Task 5 — ProductionQueueRepository

```cpp
class ProductionQueueRepository {
public:
    void load(ProductionQueue& queue, const std::string& path);
    void save(const ProductionQueue& queue, const std::string& path);
};
```

`current` (진행 중 항목, 없으면 null), `queue` (대기 목록) 저장.  
로드 시 `startedAt`은 `steady_clock::now() - elapsed` 복원.

**검증:** QueueItem 1개 enqueue → save → load → `getWaiting().size() == 1`.

---

### Task 6 — AppController 연결

`AppController` 생성자에서 load, `dispatch()` 호출 후 save.

```cpp
// AppController.h 추가
SampleRepository sampleRepo_;
OrderRepository orderRepo_;
ProductionQueueRepository queueRepo_;

// 생성자
AppController() {
    sampleRepo_.load(sampleModel_, "data/samples.json");
    orderRepo_.load(orderModel_, "data/orders.json");
    queueRepo_.load(productionQueue_, "data/production_queue.json");
    if (sampleModel_.getAll().empty()) seedData(); // 첫 실행만 시드
}

// dispatch() 끝에
void saveAll() {
    sampleRepo_.save(sampleModel_, "data/samples.json");
    orderRepo_.save(orderModel_, "data/orders.json");
    queueRepo_.save(productionQueue_, "data/production_queue.json");
}
```

**검증 (수동):**
1. 앱 실행 → 시료 등록 → 종료
2. 재실행 → 시료 목록에 이전 데이터 표시 확인
3. 주문 접수 → 승인 → 종료 → 재실행 → 주문 상태 유지 확인

---

## 커밋 순서

```
[feat] phase2: add nlohmann/json via NuGet
[feat] infra: add JsonSerializer for Sample, Order, QueueItem
[test] infra: add JsonSerializer roundtrip tests
[feat] repository: add SampleRepository with load/save
[test] repository: add SampleRepository roundtrip test
[feat] repository: add OrderRepository with load/save
[test] repository: add OrderRepository roundtrip test
[feat] repository: add ProductionQueueRepository with load/save
[test] repository: add ProductionQueueRepository roundtrip test
[feat] app: wire repositories to AppController
```
