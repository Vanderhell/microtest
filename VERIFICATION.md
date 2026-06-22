# Verification

Executed on Windows PowerShell in `C:\Users\vande\Desktop\github\microtest\microtest`.

## Results

- `git status --short` -> `M tests/test_all.c` and `?? build-reduce/` before commit.
- `git log --oneline --decorate -10` -> latest commit `d94ae39 docs: update verification evidence`.
- `git tag --list` -> no tags present.
- `git diff --check` -> no patch errors.
- MSVC clean build retry:
  - `cmake -S . -B C:\tmp\microtest-msvc-fixed`
  - `cmake --build C:\tmp\microtest-msvc-fixed --parallel 1`
  - `ctest --test-dir C:\tmp\microtest-msvc-fixed -C Debug --output-on-failure`
  - Result: passed 9/9.
- ClangCL default build:
  - `cmake -S . -B C:\tmp\microtest-clangcl -G "Visual Studio 17 2022" -T ClangCL -A x64`
  - `cmake --build C:\tmp\microtest-clangcl --parallel 1`
  - `ctest --test-dir C:\tmp\microtest-clangcl -C Debug --output-on-failure`
  - Result: passed 9/9.
- Float-disabled ClangCL build:
  - `cmake -S . -B C:\tmp\microtest-float-off -G "Visual Studio 17 2022" -T ClangCL -A x64 -DMICROTEST_ENABLE_FLOAT=OFF`
  - `cmake --build C:\tmp\microtest-float-off --parallel 1`
  - `ctest --test-dir C:\tmp\microtest-float-off -C Debug --output-on-failure`
  - Result: passed 9/9.
- Color-disabled ClangCL build:
  - `cmake -S . -B C:\tmp\microtest-color-off -G "Visual Studio 17 2022" -T ClangCL -A x64 -DMICROTEST_ENABLE_COLOR=OFF`
  - `cmake --build C:\tmp\microtest-color-off --parallel 1`
  - `ctest --test-dir C:\tmp\microtest-color-off -C Debug --output-on-failure`
  - Result: passed 9/9.
- CLI-disabled minimal ClangCL build:
  - `cmake -S . -B C:\tmp\microtest-cli-off -G "Visual Studio 17 2022" -T ClangCL -A x64 -DMICROTEST_ENABLE_CLI=OFF -DMICROTEST_ENABLE_ENV=OFF -DMICROTEST_ENABLE_EXIT=OFF`
  - `cmake --build C:\tmp\microtest-cli-off --parallel 1`
  - `ctest --test-dir C:\tmp\microtest-cli-off -C Debug --output-on-failure`
  - Result: passed 5/5.
- Install/consumer smoke test:
  - Included in the ClangCL and MSVC `ctest` runs via `install_consumer`.
  - Result: passed.
- GCC single-file build:
  - `gcc -std=c99 -Wall -Wextra -Werror -I tests tests/test_all.c -o C:\tmp\microtest-gcc.exe`
  - Result: failed because `gcc` is not on `PATH` on this host.
- LLVM sanitizer attempt:
  - `cmake -S . -B C:\tmp\microtest-sanitize -G Ninja -DCMAKE_C_COMPILER=clang -DCMAKE_C_FLAGS="-fsanitize=address,undefined" -DCMAKE_EXE_LINKER_FLAGS="-fsanitize=address,undefined" -DCMAKE_BUILD_TYPE=Debug`
  - Result: failed at link time with missing `oldnames.lib` and `msvcrtd.lib`.
- ClangCL sanitizer attempt with Debug runtime:
  - `cmake -S . -B C:\tmp\microtest-asan-clangcl -G "Visual Studio 17 2022" -T ClangCL -A x64 -DCMAKE_C_FLAGS="/fsanitize=address"`
  - Result: failed with `invalid argument '/MDd' not allowed with '-fsanitize=address'`.
- ClangCL sanitizer attempt with non-debug runtime:
  - `cmake -S . -B C:\tmp\microtest-asan-release -G "Visual Studio 17 2022" -T ClangCL -A x64 -DCMAKE_BUILD_TYPE=Release -DCMAKE_MSVC_RUNTIME_LIBRARY=MultiThreadedDLL -DCMAKE_C_FLAGS="/fsanitize=address"`
  - Result: failed at link time with undefined `__asan_init` and `__asan_version_mismatch_check_v8`.
- MSVC minimal probes:
  - `cmake -S C:\tmp\msvc-probe-stdio -B C:\tmp\msvc-probe-stdio-build`
  - `cmake --build C:\tmp\msvc-probe-stdio-build --parallel 1`
  - `cmake -S C:\tmp\msvc-probe-mtest -B C:\tmp\msvc-probe-mtest-build`
  - `cmake --build C:\tmp\msvc-probe-mtest-build --parallel 1`
  - Result: passed.

## Notes

- `v1.1.0` is not present in the local tag list.
