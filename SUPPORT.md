# Support

Supported use:

- Hosted C99 applications.
- Embedded-oriented projects that can call `MTEST_BEGIN()` and `MTEST_END()`.
- Single-header integration with one `MTEST_IMPLEMENTATION` translation unit.

Unsupported claims:

- Thread safety.
- Freestanding or ISR usage.
- RTOS-specific guarantees.
- ASan/UBSan verification on this host.

For usage questions, start with:

- [README](README.md)
- [API](docs/API.md)
- [Testing](docs/TESTING.md)
