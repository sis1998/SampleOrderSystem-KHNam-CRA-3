# 주문 생산 관리 시스템

반도체 시료 주문·생산·재고를 관리하는 콘솔 기반 MVC 애플리케이션.  
모든 데이터는 JSON 파일로 영속화된다.

---

## 빌드 환경

- Visual Studio 2022, MSVC v145, C++20, x64
- Google Test/Mock (NuGet `gmock 1.11.0`)
- nlohmann/json v3.11.3 (vendored)

## 빌드

```
msbuild SampleOrderSystem-KHNam-CRA-3.slnx /p:Configuration=Debug /p:Platform=x64
```

VS에서는 `Ctrl+Shift+B`.

## 테스트 실행

```
vstest.console.exe x64\Debug\SampleOrderSystem-KHNam-CRA-3.exe
```

또는 직접:

```
x64\Debug\SampleOrderSystem-KHNam-CRA-3.exe --gtest_color=no
```

## 실행

```
x64\Debug\SampleOrderSystem-KHNam-CRA-3.exe
```

앱 시작 시 `data/samples.json`을 읽어 시료 목록을 로드한다.  
`data/` 디렉토리가 없거나 파일이 없으면 조용히 빈 상태로 시작된다.

---

## 메뉴 구성

| 번호 | 기능 |
|------|------|
| 1 | 시료 관리 (목록 조회 / 등록 / 검색) |
| 2 | 주문 접수 |
| 3 | 주문 승인 / 거절 |
| 4 | 모니터링 (주문 현황 / 재고 현황) |
| 5 | 생산 라인 조회 |
| 6 | 출고 처리 |
| 0 | 종료 |

---

## 데이터 파일

| 파일 | 내용 |
|------|------|
| `data/samples.json` | 시료 목록 및 재고 |
| `data/orders.json` | 주문 내역 및 nextOrderId |
| `data/production_queue.json` | 생산 진행 상태 및 대기 큐 |

메뉴 입력 후 자동 저장된다. 파일이 손상되어 있으면 해당 데이터는 무시하고 빈 상태로 시작한다.

---

## 아키텍처

```
src/
  domain/       # 도메인 구조체 (Sample, Order, Production)
  model/        # 인메모리 상태 관리 (SampleModel, OrderModel, ProductionQueue)
  view/         # 콘솔 출력
  controller/   # 입력 처리 및 비즈니스 로직
  infra/        # JsonSerializer, ConsoleSetup
  repository/   # JSON 파일 load/save
data/
  samples.json
  orders.json
  production_queue.json
test/           # Google Test 단위 테스트
```

---

## 문서

- [기능 명세 (PRD)](docs/PRD.md)
- [제약사항 (CONSTRAINTS)](docs/CONSTRAINTS.md)
- [코드 컨벤션 (CONVENTIONS)](docs/CONVENTIONS.md)
- [구현 계획 (PLAN)](docs/history/PLAN.md)
