# Design Rationale

## 1. Header-only (stb style)
One file, MTEST_IMPLEMENTATION in one .c. No build system integration needed.

## 2. Macro-based assertions
C has no generics. Macros capture __FILE__, __LINE__ automatically and work with any type via casting.

## 3. Suite grouping
Not just flat test lists. Suites give structure and printed headers, matching how the micro* tests were already manually grouped with printf("[Section]").

## 4. CLI filtering at runtime
No need to recompile to run a subset. --filter=NAME does substring matching. Simple and effective.

## 5. Assert counter
Knowing "52 assertions passed" is more informative than just "19 tests passed." A test with 1 assert is not the same as a test with 10.

| Decision | Gains | Costs |
|----------|-------|-------|
| Header-only | Zero build config | MTEST_IMPLEMENTATION dance |
| Macro assertions | File/line capture | Slightly less type-safe |
| Suites | Structure | One more macro |
| CLI filtering | No recompile | argc/argv in main |
