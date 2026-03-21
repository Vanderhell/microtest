# microtest

[![CI](https://github.com/Vanderhell/microtest/actions/workflows/ci.yml/badge.svg?branch=master)](https://github.com/Vanderhell/microtest/actions/workflows/ci.yml)
[![License: MIT](https://img.shields.io/badge/License-MIT-yellow.svg)](LICENSE)
[![C99](https://img.shields.io/badge/C-C99-blue.svg)](https://en.wikipedia.org/wiki/C99)
[![Header-only](https://img.shields.io/badge/style-header--only-success.svg)](include/mtest.h)

Single-header test framework for embedded C libraries.

C99 | Header-only | Zero dependencies | Zero allocations | Color output | CLI filtering

## Why microtest?

Many small C libraries end up with copy-pasted ad-hoc test macros. `microtest` extracts that pattern into a single reusable header with a consistent API.

## Features

- 16+ assertion macros for integers, strings, memory, floats, pointers, and booleans
- Test suites for logical grouping
- Setup/teardown fixtures via `MTEST_RUN_F`
- Skip support with reason (`MTEST_SKIP`)
- CLI options: `--filter`, `-v`, `-x`, `-l`, `-h`
- Colorized output (configurable)
- Per-run assertion counter in summary

## Quick start

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

## Build and run self-tests

```bash
cd tests
make clean
make
```

## CLI examples

```bash
./test_all
./test_all --filter=ring
./test_all -l
./test_all -x
./test_all -v
./test_all -h
```

## Configuration

| Macro | Default | Description |
|-------|---------|-------------|
| `MTEST_ENABLE_COLOR` | `1` | ANSI color output |
| `MTEST_ENABLE_TIMING` | `1` | Reserved timing switch |
| `MTEST_MAX_NAME` | `64` | Max test name length |

## Project structure

```text
include/mtest.h      # framework header
tests/test_all.c     # self-tests
tests/Makefile       # local test build/run
docs/DESIGN.md       # design rationale
CONTRIBUTING.md      # contribution guide
CHANGELOG.md         # release notes
LICENSE              # MIT license (Vanderhell)
```

## Roadmap

- JUnit/XML reporting mode
- More assertion variants for custom diagnostics
- Optional minimal benchmark helper for test runtime

## Contributing

Please read [CONTRIBUTING.md](CONTRIBUTING.md).

## Changelog

See [CHANGELOG.md](CHANGELOG.md).

## License

MIT License, Copyright (c) 2026 Vanderhell.
See [LICENSE](LICENSE).

