# Verification

Executed on Windows PowerShell in `C:\Users\vande\Desktop\github\microtest\microtest`.

## Results

- `git status --short` -> only the unrelated `.gitignore` modification remained uncommitted.
- `git log --oneline --decorate -10` -> latest commit `a457ac9 build: add package export and CLI controls`.
- `git tag --list` -> no tags present.
- `git diff --check` -> no patch errors; warning only for existing `.gitignore` line endings.
- GCC single-file build:
  - `gcc.exe -std=c99 -Wall -Wextra -Wpedantic -Werror -Iinclude tests\test_all.c -o C:\tmp\microtest_gcc.exe -lm`
  - `C:\tmp\microtest_gcc.exe`
  - Result: passed.
- GCC sanitizer attempt:
  - `gcc.exe -std=c99 -Wall -Wextra -Wpedantic -Werror "-fsanitize=address,undefined" -Iinclude tests\test_all.c -o C:\tmp\microtest_san.exe -lm`
  - Result: failed to link `-lasan` and `-lubsan`; executable was not produced.
- ClangCL CMake/CTest default build:
  - `cmake -S . -B C:\tmp\microtest-clangcl-final -T ClangCL`
  - `cmake --build C:\tmp\microtest-clangcl-final --config Debug --parallel 1`
  - `ctest --test-dir C:\tmp\microtest-clangcl-final -C Debug --output-on-failure`
  - Result: passed 9/9.
- Float-disabled ClangCL build:
  - `cmake -S . -B C:\tmp\microtest-float-off -T ClangCL -DMICROTEST_ENABLE_FLOAT=OFF`
  - `cmake --build C:\tmp\microtest-float-off --config Debug --parallel 1`
  - `ctest --test-dir C:\tmp\microtest-float-off -C Debug --output-on-failure`
  - Result: passed 9/9.
- Color-disabled ClangCL build:
  - `cmake -S . -B C:\tmp\microtest-color-off -T ClangCL -DMICROTEST_ENABLE_COLOR=OFF`
  - `cmake --build C:\tmp\microtest-color-off --config Debug --parallel 1`
  - `ctest --test-dir C:\tmp\microtest-color-off -C Debug --output-on-failure`
  - Result: passed 9/9.
- CLI-disabled minimal ClangCL build:
  - `cmake -S . -B C:\tmp\microtest-cli-min -T ClangCL -DMICROTEST_ENABLE_CLI=OFF -DMICROTEST_ENABLE_ENV=OFF -DMICROTEST_ENABLE_EXIT=OFF`
  - `cmake --build C:\tmp\microtest-cli-min --config Debug --parallel 1`
  - `ctest --test-dir C:\tmp\microtest-cli-min -C Debug --output-on-failure`
  - Result: passed 5/5 after gating CLI-dependent tests.
- Install/consumer smoke test:
  - Included in the ClangCL CTest run via `install_consumer`.
  - Result: passed.
- MSVC clean build retry:
  - `cmake -S . -B C:\tmp\microtest-msvc-clean`
  - `cmake --build C:\tmp\microtest-msvc-clean --parallel 1`
  - Result: failed with `C1001` internal compiler error in `C:\Program Files (x86)\Windows Kits\10\Include\10.0.26100.0\ucrt\corecrt_stdio_config.h(90,5)`.

## Notes

- The unrelated `.gitignore` change remained uncommitted throughout.
- No annotated tag was created.
