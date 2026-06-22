#define MTEST_IMPLEMENTATION
#include "mtest.h"

typedef struct capture_buffer {
    char   data[2048];
    size_t length;
} capture_buffer_t;

static void capture_write(void *user, const char *data, size_t length) {
    capture_buffer_t *capture = (capture_buffer_t *)user;
    size_t available;
    size_t copy_length;

    if (capture == NULL || data == NULL || length == 0) {
        return;
    }

    if (capture->length >= sizeof(capture->data) - 1) {
        return;
    }

    available = sizeof(capture->data) - 1 - capture->length;
    copy_length = length < available ? length : available;
    memcpy(capture->data + capture->length, data, copy_length);
    capture->length += copy_length;
    capture->data[capture->length] = '\0';
}

static int first_ran;
static int second_ran;

MTEST(first_failure) {
    first_ran = 1;
    MTEST_FAIL("first failure");
}

MTEST(second_should_not_run) {
    second_ran = 1;
}

MTEST_SUITE(stop_on_fail_suite) {
    MTEST_RUN(first_failure);
    MTEST_RUN(second_should_not_run);
}

int main(void) {
    capture_buffer_t capture;
    char *argv[] = { "stop_on_fail", "-x", "--no-color" };
    mtest_begin_result_t begin_result;

    memset(&capture, 0, sizeof(capture));
    mtest_set_output(&capture, capture_write);
    begin_result = MTEST_BEGIN(3, argv);
    if (begin_result != MTEST_BEGIN_OK) {
        return 1;
    }

    MTEST_SUITE_RUN(stop_on_fail_suite);

    if (MTEST_END() != 1) {
        return 1;
    }
    if (first_ran != 1 || second_ran != 0) {
        return 1;
    }
    if (mtest_g.tests_discovered != 2 ||
        mtest_g.tests_selected != 1 ||
        mtest_g.tests_run != 1 ||
        mtest_g.tests_passed != 0 ||
        mtest_g.tests_failed != 1 ||
        mtest_g.tests_skipped != 0 ||
        mtest_g.tests_filtered_out != 0 ||
        mtest_g.suites_entered != 1) {
        return 1;
    }
    if (strstr(capture.data, "second_should_not_run") != NULL) {
        return 1;
    }

    return 0;
}
