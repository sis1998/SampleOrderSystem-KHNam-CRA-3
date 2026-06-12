# 제약사항 (Constraints)

## 1. 기술 제약

| 항목 | 내용 |
|------|------|
| 언어 | C++20 |
| 빌드 환경 | Visual Studio 2022, MSVC v145 |
| UI | 콘솔(Console) 전용 — GUI 없음 |
| 데이터 영속성 | JSON 파일만 사용 — 외부 DB 없음 |
| JSON 라이브러리 | nlohmann/json v3.11.3 (단일 헤더, `vendor/json.hpp`) |
| 테스트 프레임워크 | Google Mock / Google Test (NuGet gmock 1.11.0) |
| 아키텍처 패턴 | MVC (Model / Controller / View 역할 분리) |
| 스레드 모델 | 단일 스레드 — 동시성 이슈 없음 |

---

## 2. 도메인 제약

### 2.1 시료(Sample)
- 시스템에 등록된 시료만 주문 가능
- 시료 ID는 시스템 내 고유해야 함
- 수율(yield)은 0 초과 1 이하의 실수

### 2.2 주문(Order)
- 주문 수량은 1 이상의 정수
- 주문번호 형식: `ORD-N` (N: 전체 누적 순번, 가변 자리수, zero-padding 없음)
- 상태 전이는 정의된 흐름만 허용 → [PRD.md](PRD.md) 상태 머신 참조
- REJECTED 주문은 모니터링 대상에서 제외

### 2.3 재고 차감 시점

| 경로 | 차감 시점 |
|------|-----------|
| 재고 충분 → CONFIRMED | 승인 즉시 차감 |
| 재고 부족 → PRODUCING → CONFIRMED → RELEASE | 출고(RELEASE 전환) 시 차감 |

- PRODUCING 상태에서는 재고를 차감하지 않음

### 2.4 생산라인
- 실생산량: `ceil(부족분 / (yield × 0.9))` — 수율 오차 안전 마진 적용
- 총 생산시간: `avgProductionTime × 실생산량` (단위: min)
- 생산 완료 시 실생산량 전체를 재고에 추가 (초과분 포함)

### 2.5 실시간 동작
- 메인 루프 polling: 매 입력 대기 전 경과 시간 체크 → 생산 완료 여부 판단 및 상태 자동 갱신

### 2.6 출고
- CONFIRMED 상태 주문에 한해 출고 가능
- 출고 시 주문 수량만큼 재고 차감

---

## 3. 데이터 제약

- JSON 파일은 프로젝트 루트 기준 `data/` 디렉터리 하위에 위치
- 파일 분리: `data/samples.json` / `data/orders.json` / `data/production_queue.json`
- 스키마 상세 → [PRD.md](PRD.md) JSON 데이터 스키마 참조
- JSON 파일 저장 포맷: 2칸 들여쓰기(`dump(2)`), UTF-8 인코딩
