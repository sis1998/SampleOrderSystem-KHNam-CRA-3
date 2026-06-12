# 반도체 시료 생산주문관리 시스템 — 기능 명세 (PRD)

## 1. 배경 및 목적

가상의 반도체 회사 **S-Semi**는 연구소·팹리스·대학 연구실에 반도체 시료(Sample)를 납품한다.  
주문량 급증으로 엑셀·메모장 기반 관리의 한계(주문 누락, 재고-공정 현황 불일치)가 발생하여  
콘솔 기반 통합 관리 시스템을 개발한다.

---

## 2. 역할

| 역할 | 책임 |
|------|------|
| **고객** | 시료를 이메일로 요청 |
| **주문 담당자** | 주문서 작성 및 접수 처리 |
| **생산 담당자** | 개발 시료 등록, 주문 승인/거절 |

흐름: 고객 시료 요청 → 주문 담당자 주문 접수(RESERVED) → 생산 담당자 승인/거절 → 재고 확인 후 자동 분기

---

## 3. 주문 상태 머신

```
RESERVED ──승인+재고충분──► CONFIRMED ──출고처리──► RELEASE
         ──승인+재고부족──► PRODUCING ──생산완료──► CONFIRMED
         ──거절──────────► REJECTED  (모니터링 제외)
```

| 상태 | 의미 |
|------|------|
| RESERVED | 주문 접수 |
| REJECTED | 주문 거절 (정상 흐름 외) |
| PRODUCING | 승인 완료, 재고 부족으로 생산 중 |
| CONFIRMED | 승인 완료, 출고 대기 |
| RELEASE | 출고 완료 |

허용 전이 규칙 및 재고 차감 시점 → [CONSTRAINTS.md](CONSTRAINTS.md) 참조

---

## 4. 기능 명세

### 4.1 메인 메뉴

시스템 현황(등록 시료 수, 총 재고, 전체 주문 수, 생산라인 대기 수)을 상단에 표시.

| 번호 | 메뉴 | 설명 |
|------|------|------|
| 1 | 시료 관리 | 시료 등록·목록 조회·검색 |
| 2 | 시료 주문 | 고객 주문 접수 (RESERVED 생성) |
| 3 | 주문 승인/거절 | RESERVED 목록에서 승인 또는 거절 |
| 4 | 모니터링 | 상태별 주문 수 및 시료별 재고 현황 |
| 5 | 생산라인 조회 | 현재 생산 중 및 대기 큐 확인 |
| 6 | 출고 처리 | CONFIRMED 주문 출고 실행 |
| 0 | 종료 | |

---

### 4.2 시료 관리

시료(Sample) 속성: `sampleId`, `name`, `avgProductionTime(min/ea)`, `yield(0~1)`, `stock`

기능: 등록 / 전체 목록 조회(재고 포함) / 이름·속성 검색

---

### 4.3 시료 주문

입력: 시료 ID, 고객명, 주문 수량  
출력: 주문번호(예: `ORD-1`), 상태 RESERVED

---

### 4.4 주문 승인/거절

RESERVED 목록 표시 후 번호 선택.

- **승인**: 재고 충분 시 즉시 CONFIRMED / 재고 부족 시 생산라인 큐 등록 후 PRODUCING (상세 규칙 → [CONSTRAINTS.md](CONSTRAINTS.md))
- **거절**: 즉시 REJECTED 전환

---

### 4.5 모니터링

**주문량 확인**: 상태별(RESERVED / CONFIRMED / PRODUCING / RELEASE) 건수  
**재고량 확인**: 시료별 재고 수량 + 재고 상태

| 재고 상태 | 조건 |
|-----------|------|
| 여유 | 주문 대비 재고 충분 |
| 부족 | 주문 대비 재고 수량 부족 |
| 고갈 | 재고 = 0 |

---

### 4.6 생산라인

- 단일 라인, FIFO 스케줄링, 메인 루프 polling으로 진행 상태 갱신
- 생산 완료 시 재고 보충 후 주문 상태 PRODUCING → CONFIRMED 자동 전환
- 표시: 현재 처리 중인 주문(번호, 시료, 진행률, 완료 예정 시각) + 대기 큐 목록

---

### 4.7 출고 처리

CONFIRMED 목록 표시 → 번호 선택 → 재고 차감 → 상태 RELEASE 전환

---

## 5. JSON 데이터 스키마

파일 경로 → [CONSTRAINTS.md](CONSTRAINTS.md) 참조

```json
// data/samples.json
{
  "samples": [
    { "sampleId": "S-001", "name": "실리콘 웨이퍼-8인치",
      "avgProductionTime": 0.5, "yield": 0.92, "stock": 480 }
  ]
}

// data/orders.json
{
  "nextOrderId": 2,
  "orders": [
    { "orderId": "ORD-1", "sampleId": "S-001",
      "customerName": "LG이노텍", "quantity": 300,
      "status": "RESERVED", "createdAt": "2026-06-12T09:32:00" }
  ]
}

// data/production_queue.json
{
  "current": {
    "orderId": "ORD-1", "actualQty": 206,
    "startedAt": "2026-06-12T09:35:00", "estimatedMinutes": 165
  },
  "queue": [
    { "orderId": "ORD-2", "shortfall": 150, "actualQty": 190 }
  ]
}
```
