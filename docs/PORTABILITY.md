# Portability

Documented portability scope:

- C99.
- Hosted C toolchains.
- Single-header integration.
- ANSI color optional.
- CLI optional.
- Float support optional.

Documented limits:

- No claim of thread safety.
- No claim of freestanding support.
- No claim of ISR or RTOS support.
- No claim of production hardening.
- No dynamic allocation.

Verified toolchain coverage on this host:

- MSVC `cl.exe`
- ClangCL
- MinGW GCC on the known WinGet path

Sanitizer execution was not verified on this host.
