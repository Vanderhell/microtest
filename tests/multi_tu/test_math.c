#include "mtest.h"

static void test_addition(void) {
    MTEST_ASSERT_EQ(2 + 2, 4);
}

static void test_unsigned_width(void) {
    MTEST_ASSERT_EQ_U64(UINT64_C(0x8000000000000000), UINT64_C(0x8000000000000000));
}

MTEST_SUITE_DEFINE(math) {
    MTEST_RUN(test_addition);
    MTEST_RUN(test_unsigned_width);
}
