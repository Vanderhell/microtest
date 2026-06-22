#define MTEST_IMPLEMENTATION
#include "mtest.h"

typedef struct output_capture {
    char   data[2048];
    size_t length;
} output_capture_t;

static void capture_write(void *user, const char *data, size_t length) {
    output_capture_t *capture = (output_capture_t *)user;
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

MTEST(sample_pass) {
    MTEST_ASSERT_TRUE(1);
}

MTEST_SUITE(sample) {
    MTEST_RUN(sample_pass);
}

static int find_substr_order(const char *text,
                             const char *a,
                             const char *b,
                             const char *c,
                             const char *d) {
    const char *pa = strstr(text, a);
    const char *pb = strstr(text, b);
    const char *pc = strstr(text, c);
    const char *pd = strstr(text, d);

    if (pa == NULL || pb == NULL || pc == NULL || pd == NULL) {
        return 0;
    }
    return pa < pb && pb < pc && pc < pd;
}

int main(void) {
    output_capture_t capture;
    char *argv[] = { "output_callback", "--no-color", "-v" };
    mtest_begin_result_t begin_result;
    const char *text;

    memset(&capture, 0, sizeof(capture));
    mtest_set_output(&capture, capture_write);
    begin_result = MTEST_BEGIN(3, argv);
    if (begin_result != MTEST_BEGIN_OK) {
        return 1;
    }

    MTEST_SUITE_RUN(sample);

    if (MTEST_END() != 0) {
        return 1;
    }

    text = capture.data;
    if (capture.length == 0) {
        return 1;
    }
    if (strchr(text, '\033') != NULL) {
        return 1;
    }
    if (strstr(text, "sample_pass") == NULL) {
        return 1;
    }
    if (strstr(text, "PASS") == NULL) {
        return 1;
    }
    if (strstr(text, "asserts:") == NULL) {
        return 1;
    }
    if (!find_substr_order(text, "=== output_callback ===", "[sample]", "sample_pass", "Results:")) {
        return 1;
    }

    return 0;
}
