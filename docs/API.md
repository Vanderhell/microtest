# API

`mtest.h` is the public API.

## Core macros

- `MTEST(name)` defines a test function.
- `MTEST_SUITE(name)` defines a suite.
- `MTEST_SUITE_DEFINE(name)` exports a suite for another translation unit.
- `MTEST_SUITE_DECLARE(name)` declares an external suite.
- `MTEST_SUITE_RUN(name)` runs a suite.
- `MTEST_RUN(name)` runs a test inside a suite.
- `MTEST_RUN_F(name, setup_fn, teardown_fn)` runs a test with fixtures.
- `MTEST_BEGIN(argc, argv)` parses CLI options and prints the header.
- `MTEST_END()` prints the summary and returns the exit code.

## Output and color

- `mtest_set_output(user, fn)` installs a callback sink.
- `mtest_set_color(enabled)` toggles color at runtime when color support is enabled.

## State

`mtest_g` is the global runner state.

It tracks:

- discovered, selected, run, passed, failed, skipped, and filtered tests
- entered suites
- total and failed assertions
- current test and suite names
- current phase and outcome
- CLI flags and output callback settings

The current design is single-process and global-state based. It is not documented as thread-safe.
