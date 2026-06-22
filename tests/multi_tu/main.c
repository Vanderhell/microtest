#define MTEST_IMPLEMENTATION
#include "mtest.h"

MTEST_SUITE_DECLARE(math);
MTEST_SUITE_DECLARE(strings);

int main(int argc, char **argv) {
    mtest_begin_result_t begin_result = MTEST_BEGIN(argc, argv);
    if (begin_result != MTEST_BEGIN_OK) {
        return begin_result == MTEST_BEGIN_HELP_REQUESTED ? 0 : 1;
    }

    MTEST_SUITE_RUN(math);
    MTEST_SUITE_RUN(strings);
    return MTEST_END();
}
