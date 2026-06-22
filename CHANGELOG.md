# Changelog

## [1.1.0] - 2026-06-22

### Added
- Type-correct signed and unsigned integer assertions.
- String, memory, pointer, and float safety checks.
- CLI exit-code semantics for help, filtering, list mode, verbose mode, and no-color output.
- Output callbacks and runtime color control.
- Multi-translation-unit suite support.
- Independent negative self-test runners.
- CMake package export, install, and consumer verification.
- CI coverage for the CMake build path.

### Changed
- Removed timing and maximum-name configuration switches.
- Updated documentation and release baseline to match the current package layout.

## [1.0.0] - 2026-03-21

### Added
- 16 assertion macros for integers, strings, memory, floats, pointers, and booleans.
- Test suites with headers.
- Setup/teardown fixtures.
- MTEST_SKIP with reason.
- CLI: --filter, -v, -x, -l, -h.
- Color output (ANSI, optional).
- Assert counter in summary.
- 19 self-tests with 51 assertions.
