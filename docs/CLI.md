# CLI

CLI parsing is enabled when `MTEST_ENABLE_CLI` is `1`.

Supported options:

- `--filter=NAME`
- `-v`, `--verbose`
- `-x`, `--stop-on-fail`
- `-l`, `--list`
- `-h`, `--help`
- `--no-color`

## Semantics

- `--help` prints usage and exits successfully without running suites.
- `--filter=` with an empty value is invalid and returns a nonzero exit code.
- Unknown options are invalid and return a nonzero exit code.
- `-l` lists selected tests and does not run them.
- `-v` prints assertion counts for passing tests.
- `-x` stops after the first failure.
- `--no-color` disables ANSI color output for the run.
- `NO_COLOR` disables color when `MTEST_ENABLE_ENV` and `MTEST_ENABLE_COLOR` are enabled.

## Examples

```bash
./test_all --filter=math
./test_all -l
./test_all -v
./test_all -x
./test_all --no-color
```
