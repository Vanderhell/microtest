# Embedded

microtest is documented as embedded-oriented, not as a freestanding or ISR-safe framework.

Documented embedded-friendly properties:

- Single-header integration.
- No dynamic allocation.
- Optional runtime color control.
- Optional CLI can be disabled.
- Output can be redirected with a callback.

Embedded profile example:

```c
MTEST_BEGIN(argc, argv);
mtest_set_color(0);
MTEST_SUITE_RUN(board_checks);
return MTEST_END();
```

For an embedded profile build, the verified configuration is:

- `MTEST_ENABLE_COLOR=0`
- `MTEST_ENABLE_FLOAT=0`
- `MTEST_ENABLE_CLI=0`
- `MTEST_ENABLE_ENV=0`
- `MTEST_ENABLE_EXIT=0`
