# Design Rationale

## 1. Header-only
One file, `MTEST_IMPLEMENTATION` in exactly one `.c`. The same header works for embedded builds and packaged consumers.

## 2. Macro-based assertions
C has no generics. Macros capture `__FILE__` and `__LINE__` automatically and keep call sites terse.

## 3. Suite grouping
Suites give structure for related tests and support multi-translation-unit projects without a central registry file.

## 4. CLI filtering at runtime
Runtime filtering avoids recompilation when narrowing a test run. The CLI is explicit about help, list mode, verbose mode, stop-on-fail, and no-color behavior.

## 5. Output abstraction
Reporters can redirect output through a callback instead of relying on `printf`, which keeps the core header usable in embedded or host-side harnesses.

## 6. Assertion counters
Reporting passed assertions alongside test counts is more informative than a flat pass/fail summary.

| Decision | Gains | Costs |
|----------|-------|-------|
| Header-only | Zero build config | `MTEST_IMPLEMENTATION` dance |
| Macro assertions | File/line capture | Slightly less type-safe |
| Suites | Structure | One more macro |
| CLI filtering | No recompile | `argc`/`argv` in `main` |
| Output callbacks | Custom sinks | One more setup call |
