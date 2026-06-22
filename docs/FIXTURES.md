# Fixtures

Fixtures are explicit setup/teardown callbacks used with `MTEST_RUN_F`.

## Example

```c
static void setup(void) { }
static void teardown(void) { }

MTEST(test_with_fixture) {
    MTEST_ASSERT_TRUE(1);
}

MTEST_SUITE(example) {
    MTEST_RUN_F(test_with_fixture, setup, teardown);
}
```

## Behavior

- Teardown always runs after setup.
- Setup failure still allows teardown to run.
- Setup skip still allows teardown to run.
- Teardown skip is treated as a failure.
- Fixture state is tracked through the global runner state.
