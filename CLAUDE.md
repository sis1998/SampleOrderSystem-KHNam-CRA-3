## Project Overview

주문 생산 관리 시스템 (Order Production Management System) — 콘솔 기반 통합 솔루션으로, 주문 내역·재고 현황·생산 상황을 실시간으로 관리한다. 모든 데이터는 JSON 파일로 영속화된다.

요구사항 및 제약사항은 한글로 작성되며 아래 문서를 우선 참조한다.

- [기능 명세 (PRD)](docs/PRD.md)
- [제약사항 (CONSTRAINTS)](docs/CONSTRAINTS.md)

구현 참조 PoC 레포지토리:

- [ConsoleMVC](https://github.com/sis1998/ConsoleMVC-KHNam-CRA-3) — MVC 스켈레톤
- [DataPersistence](https://github.com/sis1998/DataPersistence-KHNam-CRA-3) — nlohmann/json CRUD
- [DataMonitor](https://github.com/sis1998/DataMonitor-KHNam-CRA-3) — 콘솔 실시간 모니터링
- [DummyDataGenerator](https://github.com/sis1998/DummyDataGenerator-KHNam-CRA-3) — 더미 데이터 생성

## Build

**환경:** Visual Studio 2022, MSVC v145, C++20, x64

```
# MSBuild (CLI)
msbuild SampleOrderSystem-KHNam-CRA-3.slnx /p:Configuration=Debug /p:Platform=x64
msbuild SampleOrderSystem-KHNam-CRA-3.slnx /p:Configuration=Release /p:Platform=x64
```

VS에서는 `Ctrl+Shift+B` 또는 **빌드 > 솔루션 빌드**.

## Testing

테스트 프레임워크: **Google Mock / Google Test** (NuGet `gmock 1.11.0`).  
헤더 경로: `packages/gmock.1.11.0/lib/native/include/`

```
# 전체 테스트 실행
vstest.console.exe x64\Debug\SampleOrderSystem-KHNam-CRA-3.exe

# 특정 테스트만 실행 (Google Test 필터)
vstest.console.exe x64\Debug\... --TestCaseFilter:"FullyQualifiedName~OrderService"
```

## Architecture

| 관심사 | 설명 |
|--------|------|
| **Order** | 주문 접수·조회·취소 |
| **Inventory** | 원자재·완제품 재고 추적 |
| **Production** | 생산 지시·진행 상태 |
| **Persistence** | JSON 직렬화/역직렬화 (파일 I/O) |
| **Realtime** | 이벤트/상태 변경을 즉시 반영하는 루프 또는 콜백 구조 |

- 비즈니스 로직과 I/O(JSON, 콘솔)는 분리해 테스트 가능하게 설계한다.
- 도메인 요구사항·제약사항 상세는 `docs/` 아래 문서를 참조한다.

## Key Constraints

- 데이터 영속화는 JSON만 사용 (외부 DB 없음).
- 실시간 동작: 상태 변경은 지연 없이 반영되어야 한다.
- 콘솔 I/O가 유일한 UI.
