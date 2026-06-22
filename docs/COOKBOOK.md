# Cookbook

Small examples that match the documented API.

## Basic test

```c
MTEST(test_add) {
    MTEST_ASSERT_EQ(2 + 2, 4);
}
```

## Suite

```c
MTEST_SUITE(math) {
    MTEST_RUN(test_add);
}
```

## Skip

```c
MTEST(test_skip_example) {
    MTEST_SKIP("not supported here");
}
```

## Output callback

```c
static void sink(void *user, const char *data, size_t length) {
    (void)user;
    fwrite(data, 1, length, stdout);
}

int main(int argc, char **argv) {
    mtest_set_output(NULL, sink);
    MTEST_BEGIN(argc, argv);
    MTEST_SUITE_RUN(math);
    return MTEST_END();
}
```

## Color disable

```c
int main(int argc, char **argv) {
    MTEST_BEGIN(argc, argv);
    mtest_set_color(0);
    MTEST_SUITE_RUN(math);
    return MTEST_END();
}
```

## Float disable build

Compile with `MTEST_ENABLE_FLOAT=0` and avoid float-only macros.

## CLI disable build

Compile with `MTEST_ENABLE_CLI=0`, `MTEST_ENABLE_ENV=0`, and `MTEST_ENABLE_EXIT=0`.

## Multi-TU

```c
/* shared.h */
MTEST_SUITE_DECLARE(shared);

/* shared.c */
MTEST_SUITE_DEFINE(shared) {
    MTEST_RUN(test_add);
}
```
