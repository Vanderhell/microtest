#define MTEST_IMPLEMENTATION
#include "mtest.h"

MTEST(consumer_pass) {
    MTEST_ASSERT_TRUE(1);
}

MTEST_SUITE(consumer) {
    MTEST_RUN(consumer_pass);
}

int main(void) {
    char *argv[] = { "consumer", "--no-color" };
    mtest_begin_result_t begin_result = MTEST_BEGIN(2, argv);

    if (begin_result != MTEST_BEGIN_OK) {
        return 1;
    }

    MTEST_SUITE_RUN(consumer);
    return MTEST_END();
}
