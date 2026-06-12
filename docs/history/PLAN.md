# 주문 생산 관리 시스템 — 구현 계획

**목표:** 반도체 시료 주문·생산·재고를 관리하는 콘솔 MVC 애플리케이션 구축  
**아키텍처:** C++20 단일 프로젝트, MVC 레이어 분리, JSON 영속화 (Phase 2~)  
**브랜치 전략:** `master` ← 각 Phase PR 병합

---

## 페이즈 로드맵

| 페이즈 | 브랜치 | 범위 |
|--------|--------|------|
| **Phase 1** | `phase1/console-mvc` | MVC 셸 + 인메모리 도메인 모델, 6개 메뉴 전부 동작 |
| **Phase 2** | `phase2/json-persistence` | JSON 파일 로드/저장 — 재시작 후 데이터 유지 |
| **Phase 3** | `phase3/validation-ux` | 입력 유효성 검사, 한글 오류 메시지, 인덱스 기반 CRUD 선택 |
| **Phase 4** | `phase4/realtime-production` | 생산라인 실시간 진행률 표시 및 자동 완료 전환 |
| **Phase 5** | `phase5/ux-polish` | 테이블 정렬, 페이징, 헤더 갱신 |
| **Phase 6** | `phase6/release` | 시드 데이터 제거, Release 빌드 검증 |

---

## PRD 기능 커버리지

| 기능 | Phase |
|------|-------|
| 메인 메뉴 (0~6) + 시스템 현황 헤더 | 1 |
| 시료 등록·목록 조회·이름 검색 | 1 |
| 주문 접수 → RESERVED | 1 |
| 주문 승인 → CONFIRMED (재고 충분) / PRODUCING (부족) | 1 |
| 주문 거절 → REJECTED | 1 |
| 모니터링: 상태별 주문 수 + 시료별 재고 상태 | 1 |
| 생산라인 조회: 현재 처리 중 + 대기 큐 (정적 스냅샷) | 1 |
| 생산 완료 자동 전환: PRODUCING → CONFIRMED + 재고 보충 (메인 루프 tick) | 1 |
| 출고 처리: CONFIRMED → RELEASE + 재고 차감 | 1 |
| JSON 영속화 (samples / orders / production_queue) | 2 |
| 입력 유효성 검사 + 한글 오류 메시지 + 인덱스 기반 CRUD 선택 | 3 |
| 생산라인 실시간 진행률 표시 (진행 %, 완료 예정 시각) | 4 |
| 생산 완료 시 메인 화면 즉시 알림 및 자동 CONFIRMED 전환 표시 | 4 |
| 리스트 페이징 · 테이블 포맷 | 5 |

---

## 세부 계획

- [Phase 1 — MVC 셸 + 인메모리 도메인](Phase1.md)
- [Phase 2 — JSON 영속화](Phase2.md)
- [Phase 3 — 유효성 검사 + UX 개선 (인덱스 기반 선택)](Phase3.md)
- Phase 4~6 — 각 Phase 완료 후 작성
