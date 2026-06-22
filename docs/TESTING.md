# Testing

Verified test targets:

- `microtest_selftest`
- `microtest_selftest_float_off`
- `microtest_embedded_profile`
- `multi_tu`
- `install_consumer`
- `output_callback`
- `output_callback_nocolor`
- `stop_on_fail`
- `cli_behavior`

Verified build configurations:

- MSVC
- ClangCL
- Float disabled
- Color disabled
- CLI/env/exit disabled
- MinGW GCC single-file build

Example local command sequence:

```powershell
cmake -S . -B C:\tmp\microtest-clangcl -G "Visual Studio 17 2022" -T ClangCL -A x64
cmake --build C:\tmp\microtest-clangcl --parallel 1
ctest --test-dir C:\tmp\microtest-clangcl -C Debug --output-on-failure
```
