/*
 * microtest self-test — the test framework tests itself.
 *
 * Build: gcc -std=c99 -Wall -Wextra -Wpedantic -Werror -I../include test_all.c -o test_all -lm
 */

#define MTEST_IMPLEMENTATION
#include "mtest.h"

/* ═══════════════════════════════════════════════════════════════════════════
 * Tests: Integer assertions
 * ═══════════════════════════════════════════════════════════════════════════ */

MTEST(test_assert_eq) {
    MTEST_ASSERT_EQ(42, 42);
    MTEST_ASSERT_EQ(0, 0);
    MTEST_ASSERT_EQ(-1, -1);
    MTEST_ASSERT_EQ(0xFFFF, 65535);
}

MTEST(test_assert_ne) {
    MTEST_ASSERT_NE(1, 2);
    MTEST_ASSERT_NE(-1, 1);
    MTEST_ASSERT_NE(0, 1);
}

MTEST(test_assert_gt) {
    MTEST_ASSERT_GT(5, 3);
    MTEST_ASSERT_GT(0, -1);
    MTEST_ASSERT_GT(1000, 999);
}

MTEST(test_assert_ge) {
    MTEST_ASSERT_GE(5, 5);
    MTEST_ASSERT_GE(6, 5);
    MTEST_ASSERT_GE(0, 0);
}

MTEST(test_assert_lt) {
    MTEST_ASSERT_LT(3, 5);
    MTEST_ASSERT_LT(-1, 0);
    MTEST_ASSERT_LT(999, 1000);
}

/* ═══════════════════════════════════════════════════════════════════════════
 * Tests: Boolean assertions
 * ═══════════════════════════════════════════════════════════════════════════ */

MTEST(test_assert_true) {
    MTEST_ASSERT_TRUE(1);
    MTEST_ASSERT_TRUE(42);
    MTEST_ASSERT_TRUE(1 == 1);
    MTEST_ASSERT_TRUE(3 > 2);
}

MTEST(test_assert_false) {
    MTEST_ASSERT_FALSE(0);
    MTEST_ASSERT_FALSE(1 == 2);
    MTEST_ASSERT_FALSE(3 < 2);
}

/* ═══════════════════════════════════════════════════════════════════════════
 * Tests: Pointer assertions
 * ═══════════════════════════════════════════════════════════════════════════ */

MTEST(test_assert_null) {
    void *p = NULL;
    MTEST_ASSERT_NULL(p);
}

MTEST(test_assert_not_null) {
    int x = 42;
    MTEST_ASSERT_NOT_NULL(&x);
}

/* ═══════════════════════════════════════════════════════════════════════════
 * Tests: String assertions
 * ═══════════════════════════════════════════════════════════════════════════ */

MTEST(test_assert_str_eq) {
    MTEST_ASSERT_STR_EQ("hello", "hello");
    MTEST_ASSERT_STR_EQ("", "");
}

MTEST(test_assert_str_eq_null) {
    /* NULL == NULL should pass */
    MTEST_ASSERT_STR_EQ(NULL, NULL);
}

MTEST(test_assert_str_contains) {
    MTEST_ASSERT_STR_CONTAINS("hello world", "world");
    MTEST_ASSERT_STR_CONTAINS("abcdef", "cde");
    MTEST_ASSERT_STR_CONTAINS("exact", "exact");
}

/* ═══════════════════════════════════════════════════════════════════════════
 * Tests: Memory assertions
 * ═══════════════════════════════════════════════════════════════════════════ */

MTEST(test_assert_mem_eq) {
    uint8_t a[] = { 0xDE, 0xAD, 0xBE, 0xEF };
    uint8_t b[] = { 0xDE, 0xAD, 0xBE, 0xEF };
    MTEST_ASSERT_MEM_EQ(a, b, 4);
}

/* ═══════════════════════════════════════════════════════════════════════════
 * Tests: Float assertions
 * ═══════════════════════════════════════════════════════════════════════════ */

MTEST(test_assert_float_eq) {
    MTEST_ASSERT_FLOAT_EQ(3.14f, 3.14f);
    MTEST_ASSERT_FLOAT_EQ(0.0f, 0.0f);
    MTEST_ASSERT_FLOAT_EQ(-273.15f, -273.15f);
}

MTEST(test_assert_near) {
    MTEST_ASSERT_NEAR(3.14159, 3.14160, 0.001);
    MTEST_ASSERT_NEAR(100.0, 100.05, 0.1);
}

/* ═══════════════════════════════════════════════════════════════════════════
 * Tests: Setup / Teardown
 * ═══════════════════════════════════════════════════════════════════════════ */

static int setup_called = 0;
static int teardown_called = 0;
static int fixture_value = 0;

static void my_setup(void) {
    setup_called++;
    fixture_value = 42;
}

static void my_teardown(void) {
    teardown_called++;
    fixture_value = 0;
}

MTEST(test_with_fixture) {
    MTEST_ASSERT_EQ(42, fixture_value);
    MTEST_ASSERT_TRUE(setup_called > 0);
}

/* ═══════════════════════════════════════════════════════════════════════════
 * Tests: Skip
 * ═══════════════════════════════════════════════════════════════════════════ */

MTEST(test_skip_example) {
#ifdef _WIN32
    MTEST_SKIP("not supported on Windows");
#endif
    MTEST_ASSERT_TRUE(1);  /* runs on non-Windows */
}

/* ═══════════════════════════════════════════════════════════════════════════
 * Tests: Large values
 * ═══════════════════════════════════════════════════════════════════════════ */

MTEST(test_large_values) {
    MTEST_ASSERT_EQ(0xFFFFFFFF, 0xFFFFFFFF);
    MTEST_ASSERT_EQ(-2147483647, -2147483647);
    MTEST_ASSERT_GT(0x80000000UL, 0x7FFFFFFFUL);
}

/* ═══════════════════════════════════════════════════════════════════════════
 * Tests: Multiple asserts in one test
 * ═══════════════════════════════════════════════════════════════════════════ */

MTEST(test_multiple_asserts) {
    MTEST_ASSERT_EQ(1, 1);
    MTEST_ASSERT_TRUE(1);
    MTEST_ASSERT_FALSE(0);
    MTEST_ASSERT_NE(1, 2);
    MTEST_ASSERT_GT(2, 1);
    MTEST_ASSERT_GE(2, 2);
    MTEST_ASSERT_LT(1, 2);
    MTEST_ASSERT_STR_EQ("ok", "ok");
    MTEST_ASSERT_FLOAT_EQ(1.0f, 1.0f);
}

/* ═══════════════════════════════════════════════════════════════════════════
 * Suites
 * ═══════════════════════════════════════════════════════════════════════════ */

MTEST_SUITE(integers) {
    MTEST_RUN(test_assert_eq);
    MTEST_RUN(test_assert_ne);
    MTEST_RUN(test_assert_gt);
    MTEST_RUN(test_assert_ge);
    MTEST_RUN(test_assert_lt);
}

MTEST_SUITE(booleans) {
    MTEST_RUN(test_assert_true);
    MTEST_RUN(test_assert_false);
}

MTEST_SUITE(pointers) {
    MTEST_RUN(test_assert_null);
    MTEST_RUN(test_assert_not_null);
}

MTEST_SUITE(strings) {
    MTEST_RUN(test_assert_str_eq);
    MTEST_RUN(test_assert_str_eq_null);
    MTEST_RUN(test_assert_str_contains);
}

MTEST_SUITE(memory) {
    MTEST_RUN(test_assert_mem_eq);
}

MTEST_SUITE(floats) {
    MTEST_RUN(test_assert_float_eq);
    MTEST_RUN(test_assert_near);
}

MTEST_SUITE(fixtures) {
    MTEST_RUN_F(test_with_fixture, my_setup, my_teardown);
}

MTEST_SUITE(misc) {
    MTEST_RUN(test_skip_example);
    MTEST_RUN(test_large_values);
    MTEST_RUN(test_multiple_asserts);
}

/* ═══════════════════════════════════════════════════════════════════════════
 * Main
 * ═══════════════════════════════════════════════════════════════════════════ */

int main(int argc, char **argv) {
    MTEST_BEGIN(argc, argv);

    MTEST_SUITE_RUN(integers);
    MTEST_SUITE_RUN(booleans);
    MTEST_SUITE_RUN(pointers);
    MTEST_SUITE_RUN(strings);
    MTEST_SUITE_RUN(memory);
    MTEST_SUITE_RUN(floats);
    MTEST_SUITE_RUN(fixtures);
    MTEST_SUITE_RUN(misc);

    /* Verify setup/teardown was called (only when not filtering) */
    if (mtest_g.filter == NULL && !mtest_g.list_only) {
        if (setup_called != 1 || teardown_called != 1) {
            printf("  FIXTURE ERROR: setup=%d teardown=%d\n",
                   setup_called, teardown_called);
            mtest_g.tests_failed++;
        }
    }

    return MTEST_END();
}
