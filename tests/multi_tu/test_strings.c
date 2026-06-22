#include "mtest.h"

static void test_string_eq(void) {
    MTEST_ASSERT_STR_EQ("micro", "micro");
}

static void test_memory_eq(void) {
    const unsigned char left[] = { 1u, 2u, 3u, 4u };
    const unsigned char right[] = { 1u, 2u, 3u, 4u };
    MTEST_ASSERT_MEM_EQ(left, right, sizeof(left));
}

MTEST_SUITE_DEFINE(strings) {
    MTEST_RUN(test_string_eq);
    MTEST_RUN(test_memory_eq);
}
