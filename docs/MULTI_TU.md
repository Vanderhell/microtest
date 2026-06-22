# Multi-Translation-Unit

The suite macros support splitting tests across source files.

## Pattern

```c
/* main.c */
MTEST_SUITE_DECLARE(math);
MTEST_SUITE_DECLARE(strings);

int main(int argc, char **argv) {
    MTEST_BEGIN(argc, argv);
    MTEST_SUITE_RUN(math);
    MTEST_SUITE_RUN(strings);
    return MTEST_END();
}
```

```c
/* test_math.c */
MTEST_SUITE_DEFINE(math) {
    MTEST_RUN(test_addition);
}
```

```c
/* test_strings.c */
MTEST_SUITE_DEFINE(strings) {
    MTEST_RUN(test_string_eq);
}
```

The verified multi-TU test target is `test_multi_tu`.
