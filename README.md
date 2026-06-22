# microtest

Single-header C99 test framework for hosted and embedded-oriented C projects.

What it does:

- Assertions for integers, pointers, strings, memory, and optional floats.
- Suites for grouping tests, including multi-translation-unit suites.
- Fixtures with explicit setup and teardown.
- Runtime CLI filtering, list mode, verbose mode, stop-on-fail, and no-color.
- Output callbacks for custom sinks.
- CMake package export, install, and consumer verification.

What it does not do:

- No dynamic allocation.
- No third-party dependencies.
- No claim of thread safety.
- No ASan/UBSan verification on this host.
- No claim of freestanding, ISR, RTOS, or production readiness.

## Quick Start

```c
#define MTEST_IMPLEMENTATION
#include "mtest.h"

MTEST(test_addition) {
    MTEST_ASSERT_EQ(2 + 2, 4);
}

MTEST_SUITE(math) {
    MTEST_RUN(test_addition);
}

int main(int argc, char **argv) {
    MTEST_BEGIN(argc, argv);
    MTEST_SUITE_RUN(math);
    return MTEST_END();
}
```

## Basic Examples

Assertions:

```c
MTEST(test_strings) {
    MTEST_ASSERT_STR_EQ("alpha", "alpha");
    MTEST_ASSERT_MEM_EQ("abc", "abc", 3);
}
```

Skip:

```c
MTEST(test_skip_example) {
    MTEST_SKIP("not supported here");
}
```

Fixture:

```c
static void setup(void) { }
static void teardown(void) { }

MTEST(test_with_fixture) {
    MTEST_ASSERT_TRUE(1);
}

MTEST_SUITE(example) {
    MTEST_RUN_F(test_with_fixture, setup, teardown);
}
```

Multi-TU:

```c
/* in one file */
MTEST_SUITE_DEFINE(shared) { MTEST_RUN(test_one); }

/* in another file */
MTEST_SUITE_DECLARE(shared);
```

## CLI

Supported options:

- `--filter=NAME`
- `-v`, `--verbose`
- `-x`, `--stop-on-fail`
- `-l`, `--list`
- `-h`, `--help`
- `--no-color`

Examples:

```bash
./test_all --filter=ring
./test_all -l
./test_all -x
./test_all --no-color
```

`--help` prints usage and returns success without running suites.
An empty `--filter=` is invalid.
An unknown option fails with a nonzero exit code.

## Configuration

| Macro | Default | Meaning |
|---|---:|---|
| `MTEST_ENABLE_COLOR` | `1` | ANSI colors at runtime |
| `MTEST_ENABLE_CLI` | `1` | CLI parsing and help |
| `MTEST_ENABLE_ENV` | `1` | `NO_COLOR` support |
| `MTEST_ENABLE_EXIT` | `1` | Hosted exit codes from `MTEST_END()` |
| `MTEST_ENABLE_FLOAT` | `1` | Float assertions and math helpers |

## Layout

- `include/mtest.h` - single public header
- `tests/` - self-tests, CLI checks, install consumer, and multi-TU checks
- `cmake/` - package config template
- `docs/` - reference notes for API and usage

## More

- [API](docs/API.md)
- [Assertions](docs/ASSERTIONS.md)
- [CLI](docs/CLI.md)
- [Fixtures](docs/FIXTURES.md)
- [Testing](docs/TESTING.md)
- [Release](docs/RELEASE.md)
