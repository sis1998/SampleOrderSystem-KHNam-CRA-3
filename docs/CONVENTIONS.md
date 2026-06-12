# 코드 컨벤션

## 커밋 메시지

### 형식

```
[prefix] scope: 설명
```

- **prefix**: 변경 유형 (아래 표 참조)
- **scope**: 변경 대상을 한 단어로 표기 (예: `ui`, `order`, `inventory`, `production`, `persistence`, `test`)
- **설명**: 변경 내용을 간결하게 서술

### Prefix 목록

| Prefix | 의미 |
|--------|------|
| `[feat]` | 새 기능 추가 |
| `[feat en]` | 기존 기능 개선 (enhancement) |
| `[feat dis]` | 기능 비활성화 또는 제거 |
| `[refactor]` | 동작 변경 없는 코드 구조 개선 |
| `[revert]` | 이전 커밋 되돌리기 |
| `[fix]` | 버그 수정 |
| `[test]` | 테스트 추가 또는 수정 |
| `[docs]` | 문서 추가 또는 수정 |

### 예시

```
[feat] ui: add color on console
[feat en] order: improve reserved list display
[feat dis] production: disable auto-refresh on idle
[fix] inventory: correct stock deduction on release
[refactor] persistence: extract json serialization to repository
[test] order: add unit tests for state transition
[docs] constraints: add json library spec
[revert] ui: revert color change due to encoding issue
```

### 규칙

- scope는 소문자 한 단어
- 설명은 소문자로 시작, 마침표 없음
- 제목은 72자 이내
