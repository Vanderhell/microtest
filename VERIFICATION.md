# Verification

Executed on Windows PowerShell in `C:\Users\vande\Desktop\github\microtest\microtest`.

## Results

- `git status --short` -> only the unrelated `.gitignore` modification remained uncommitted.
- `git log --oneline --decorate -10` -> latest commit `63e0544 docs: correct verification limits`.
- `git tag --list` -> no tags present.
- `git diff --check` -> no patch errors; warning only for existing `.gitignore` line endings.
- GCC single-file build:
  - `gcc.exe -std=c99 -Wall -Wextra -Wpedantic -Werror -Iinclude tests\test_all.c -o C:\tmp\microtest_gcc.exe -lm`
  - `C:\tmp\microtest_gcc.exe`
  - Result: passed.
- GCC sanitizer attempt:
  - `gcc.exe -std=c99 -Wall -Wextra -Wpedantic -Werror "-fsanitize=address,undefined" -Iinclude tests\test_all.c -o C:\tmp\microtest_san.exe -lm`
  - Result: failed to link `-lasan` and `-lubsan`; executable was not produced.
- Clang sanitizer attempt:
  - `clang.exe -std=c99 -Wall -Wextra -Wpedantic -Werror "-fsanitize=address,undefined" -Iinclude tests\test_all.c -o C:\tmp\microtest_clang_san.exe -lm`
  - Result: failed with `fatal error: 'stdio.h' file not found`.
- ClangCL sanitizer attempt:
  - `clang-cl.exe /nologo /std:c11 /W4 /WX "-fsanitize=address,undefined" /Iinclude tests\test_all.c /Fe:C:\tmp\microtest_clangcl_san.exe`
  - Result: failed with `unable to find a Visual Studio installation`.
- CMake sanitizer attempt:
  - `cmake -S . -B C:\tmp\microtest-asan -T ClangCL -DCMAKE_C_FLAGS_DEBUG=/fsanitize=address`
  - Result: failed in compiler detection with `invalid argument '/MDd' not allowed with '-fsanitize=address'`.
- CMake sanitizer attempt with non-debug runtime:
  - `cmake -S . -B C:\tmp\microtest-asan-clangcl -T ClangCL -DCMAKE_MSVC_RUNTIME_LIBRARY=MultiThreaded -DCMAKE_C_FLAGS_DEBUG=/fsanitize=address`
  - Result: failed at link time with unresolved `__asan_init` and `__asan_version_mismatch_check_v8`.
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
- MSVC minimal probes:
  - `cmake -S C:\tmp\msvc-probe-stdio -B C:\tmp\msvc-probe-stdio-build`
  - `cmake --build C:\tmp\msvc-probe-stdio-build --parallel 1`
  - `cmake -S C:\tmp\msvc-probe-mtest -B C:\tmp\msvc-probe-mtest-build`
  - `cmake --build C:\tmp\msvc-probe-mtest-build --parallel 1`
  - Result: both passed.

## Notes

- The unrelated `.gitignore` change remained uncommitted throughout.
- The local release tag `v1.1.0` does not exist.
