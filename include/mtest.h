/*
 * microtest â€” Single-header test framework for embedded C libraries.
 *
 * Replaces the ad-hoc test macros scattered across the micro* ecosystem.
 * Provides: assertions, test registration, suites, setup/teardown, filtering,
 * color output, XML report, and summary stats.
 *
 * C99 Â· Single header Â· Zero dependencies Â· Zero allocations Â· Portable
 *
 * SPDX-License-Identifier: MIT
 * https://github.com/Vanderhell/microtest
 *
 * Usage:
 *   #define MTEST_IMPLEMENTATION    // in exactly ONE .c file
 *   #include "mtest.h"
 *
 *   MTEST(my_test) {
 *       MTEST_ASSERT_EQ(2 + 2, 4);
 *       MTEST_ASSERT_STR_EQ("hello", "hello");
 *   }
 *
 *   MTEST_SUITE(math) {
 *       MTEST_RUN(my_test);
 *   }
 *
 *   int main(int argc, char **argv) {
 *       MTEST_BEGIN(argc, argv);
 *       MTEST_SUITE_RUN(math);
 *       return MTEST_END();
 *   }
 */

#ifndef MTEST_H
#define MTEST_H

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdint.h>
#include <inttypes.h>
#include <math.h>

typedef enum mtest_outcome {
    MTEST_OUTCOME_NOT_RUN = 0,
    MTEST_OUTCOME_RUNNING,
    MTEST_OUTCOME_PASSED,
    MTEST_OUTCOME_FAILED,
    MTEST_OUTCOME_SKIPPED,
    MTEST_OUTCOME_SETUP_FAILED
} mtest_outcome_t;

typedef enum mtest_phase {
    MTEST_PHASE_IDLE = 0,
    MTEST_PHASE_SETUP,
    MTEST_PHASE_BODY,
    MTEST_PHASE_TEARDOWN
} mtest_phase_t;

/* â”€â”€ Configuration â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€ */

/** Enable ANSI color output. */
#ifndef MTEST_ENABLE_COLOR
#define MTEST_ENABLE_COLOR 1
#endif

/** Enable timing (requires clock()). */
#ifndef MTEST_ENABLE_TIMING
#define MTEST_ENABLE_TIMING 1
#endif

/** Maximum test name length for filtering. */
#ifndef MTEST_MAX_NAME
#define MTEST_MAX_NAME 64
#endif

/* â”€â”€ Color codes â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€ */

#if MTEST_ENABLE_COLOR
#define MTEST_CLR_RESET  "\033[0m"
#define MTEST_CLR_GREEN  "\033[32m"
#define MTEST_CLR_RED    "\033[31m"
#define MTEST_CLR_YELLOW "\033[33m"
#define MTEST_CLR_CYAN   "\033[36m"
#define MTEST_CLR_BOLD   "\033[1m"
#define MTEST_CLR_DIM    "\033[2m"
#else
#define MTEST_CLR_RESET  ""
#define MTEST_CLR_GREEN  ""
#define MTEST_CLR_RED    ""
#define MTEST_CLR_YELLOW ""
#define MTEST_CLR_CYAN   ""
#define MTEST_CLR_BOLD   ""
#define MTEST_CLR_DIM    ""
#endif

/* â”€â”€ Global state â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€ */

typedef struct {
    int             tests_discovered;
    int             tests_selected;
    int             tests_run;
    int             tests_passed;
    int             tests_failed;
    int             tests_skipped;
    int             tests_filtered_out;
    int             suites_entered;
    int             suites_run;     /* compatibility alias for suites_entered */
    int             asserts_total;
    int             asserts_failed;

    /* Current test state */
    const char     *current_test;
    const char     *current_suite;
    const char     *current_skip_reason;
    mtest_outcome_t current_outcome;
    mtest_phase_t   current_phase;
    int             current_failed;     /* assertions failed in current test */

    /* Filtering */
    const char *filter;             /* --filter=NAME substring match     */
    int         verbose;            /* -v: show passing asserts          */
    int         stop_on_fail;       /* -x: stop after first failure      */
    int         list_only;          /* -l: list tests, don't run         */

#if MTEST_ENABLE_TIMING
    double      total_time_ms;
    double      suite_time_ms;
#endif
} mtest_state_t;

/* Extern declaration â€” defined in MTEST_IMPLEMENTATION block */
#ifndef MTEST_IMPLEMENTATION
extern mtest_state_t mtest_g;
#endif

/* â”€â”€ Test definition macros â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€ */

/** Define a test function. */
#define MTEST(name)  static void name(void)

/** Define a suite function. */
#define MTEST_SUITE(name)  static void mtest_suite_##name(void)

/** Run a suite. */
#define MTEST_SUITE_RUN(name)  do {                                       \
    mtest_g.current_suite = #name;                                         \
    mtest_g.suites_entered++;                                              \
    mtest_g.suites_run++;                                                  \
    printf("\n" MTEST_CLR_BOLD "[%s]" MTEST_CLR_RESET "\n", #name);        \
    mtest_suite_##name();                                                  \
    mtest_g.current_suite = NULL;                                          \
} while (0)

/** Run a test within a suite. */
#define MTEST_RUN(name)  do {                                             \
    mtest_g.tests_discovered++;                                            \
    if (mtest_g.stop_on_fail && mtest_g.tests_failed > 0) {                \
        mtest_g.tests_filtered_out++;                                      \
        break;                                                             \
    }                                                                      \
    if (mtest_g.filter != NULL &&                                          \
        strstr(#name, mtest_g.filter) == NULL) {                           \
        mtest_g.tests_filtered_out++;                                      \
        break;                                                             \
    }                                                                      \
    if (mtest_g.list_only) {                                               \
        printf("  %s\n", #name);                                           \
        mtest_g.tests_filtered_out++;                                      \
        break;                                                             \
    }                                                                      \
    mtest_g.tests_selected++;                                              \
    mtest_g.tests_run++;                                                   \
    mtest_g.current_test = #name;                                          \
    mtest_g.current_failed = 0;                                            \
    mtest_g.current_skip_reason = NULL;                                    \
    mtest_g.current_outcome = MTEST_OUTCOME_RUNNING;                       \
    mtest_g.current_phase = MTEST_PHASE_BODY;                              \
    printf("  %-55s ", #name);                                             \
    fflush(stdout);                                                        \
    name();                                                                \
    if (mtest_g.current_outcome == MTEST_OUTCOME_RUNNING) {                \
        mtest_g.current_outcome = MTEST_OUTCOME_PASSED;                    \
    }                                                                      \
    if (mtest_g.current_outcome == MTEST_OUTCOME_PASSED) {                 \
        mtest_g.tests_passed++;                                            \
        printf(MTEST_CLR_GREEN "PASS" MTEST_CLR_RESET "\n");              \
    }                                                                      \
    mtest_g.current_test = NULL;                                           \
    mtest_g.current_skip_reason = NULL;                                    \
    mtest_g.current_outcome = MTEST_OUTCOME_NOT_RUN;                       \
    mtest_g.current_phase = MTEST_PHASE_IDLE;                              \
    mtest_g.current_failed = 0;                                            \
} while (0)

/* â”€â”€ Setup / Teardown â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€ */

/** Run a test with setup and teardown functions.
 * Teardown always runs after setup, even when setup fails or skips, so partial
 * fixture initialization can be cleaned up deterministically.
 */
#define MTEST_RUN_F(name, setup_fn, teardown_fn)  do {                    \
    mtest_g.tests_discovered++;                                            \
    if (mtest_g.stop_on_fail && mtest_g.tests_failed > 0) {                \
        mtest_g.tests_filtered_out++;                                      \
        break;                                                             \
    }                                                                      \
    if (mtest_g.filter != NULL &&                                          \
        strstr(#name, mtest_g.filter) == NULL) {                           \
        mtest_g.tests_filtered_out++;                                      \
        break;                                                             \
    }                                                                      \
    if (mtest_g.list_only) {                                               \
        printf("  %s\n", #name);                                           \
        mtest_g.tests_filtered_out++;                                      \
        break;                                                             \
    }                                                                      \
    mtest_g.tests_selected++;                                              \
    mtest_g.tests_run++;                                                   \
    mtest_g.current_test = #name;                                          \
    mtest_g.current_failed = 0;                                            \
    mtest_g.current_skip_reason = NULL;                                    \
    mtest_g.current_outcome = MTEST_OUTCOME_RUNNING;                       \
    mtest_g.current_phase = MTEST_PHASE_SETUP;                             \
    mtest_outcome_t mtest_setup_outcome_ = MTEST_OUTCOME_RUNNING;          \
    int mtest_body_ran_ = 0;                                               \
    printf("  %-55s ", #name);                                             \
    fflush(stdout);                                                        \
    setup_fn();                                                            \
    mtest_setup_outcome_ = mtest_g.current_outcome;                         \
    if (mtest_g.current_outcome == MTEST_OUTCOME_RUNNING) {                \
        mtest_g.current_phase = MTEST_PHASE_BODY;                          \
        mtest_body_ran_ = 1;                                               \
        name();                                                            \
    }                                                                      \
    mtest_g.current_phase = MTEST_PHASE_TEARDOWN;                          \
    teardown_fn();                                                         \
    if (mtest_setup_outcome_ == MTEST_OUTCOME_FAILED &&                    \
        !mtest_body_ran_ &&                                                \
        mtest_g.current_outcome == MTEST_OUTCOME_FAILED &&                 \
        mtest_g.current_failed == 1) {                                     \
        mtest_g.current_outcome = MTEST_OUTCOME_SETUP_FAILED;              \
    }                                                                      \
    if (mtest_g.current_outcome == MTEST_OUTCOME_RUNNING) {                \
        mtest_g.current_outcome = MTEST_OUTCOME_PASSED;                    \
    }                                                                      \
    if (mtest_g.current_outcome == MTEST_OUTCOME_PASSED) {                 \
        mtest_g.tests_passed++;                                            \
        printf(MTEST_CLR_GREEN "PASS" MTEST_CLR_RESET "\n");              \
    }                                                                      \
    mtest_g.current_test = NULL;                                           \
    mtest_g.current_skip_reason = NULL;                                    \
    mtest_g.current_outcome = MTEST_OUTCOME_NOT_RUN;                       \
    mtest_g.current_phase = MTEST_PHASE_IDLE;                              \
    mtest_g.current_failed = 0;                                            \
} while (0)

/* â”€â”€ Assertion macros â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€ */

#define MTEST_FAIL_(fmt, ...) do {                                        \
    if (mtest_g.current_failed == 0) {                                     \
        printf(MTEST_CLR_RED "FAIL" MTEST_CLR_RESET "\n");                \
        mtest_g.tests_failed++;                                            \
    }                                                                      \
    mtest_g.current_outcome = MTEST_OUTCOME_FAILED;                        \
    mtest_g.current_failed++;                                              \
    mtest_g.asserts_failed++;                                              \
    printf("    " MTEST_CLR_RED "%s:%d: " MTEST_CLR_RESET fmt "\n",       \
           __FILE__, __LINE__, __VA_ARGS__);                               \
    return;                                                                \
} while (0)

/* Legacy generic integer assertions compare via intmax_t. Unsigned values
 * above INTMAX_MAX require the typed *_U64 assertions below.
 */

/** Assert that two integers are equal. */
#define MTEST_ASSERT_EQ(expected, actual) do {                            \
    mtest_g.asserts_total++;                                               \
    intmax_t mtest_e_ = (intmax_t)(expected);                              \
    intmax_t mtest_a_ = (intmax_t)(actual);                                \
    if (mtest_e_ != mtest_a_) {                                            \
        MTEST_FAIL_("expected %" PRIdMAX ", got %" PRIdMAX,             \
                    mtest_e_, mtest_a_);                                   \
    }                                                                      \
} while (0)

/** Assert that two integers are NOT equal. */
#define MTEST_ASSERT_NE(a, b) do {                                        \
    mtest_g.asserts_total++;                                               \
    intmax_t mtest_a_ = (intmax_t)(a);                                     \
    intmax_t mtest_b_ = (intmax_t)(b);                                     \
    if (mtest_a_ == mtest_b_) {                                            \
        MTEST_FAIL_("expected != %" PRIdMAX ", got %" PRIdMAX,         \
                    mtest_a_, mtest_b_);                                   \
    }                                                                      \
} while (0)

/** Assert a > b. */
#define MTEST_ASSERT_GT(a, b) do {                                        \
    mtest_g.asserts_total++;                                               \
    intmax_t mtest_a_ = (intmax_t)(a);                                     \
    intmax_t mtest_b_ = (intmax_t)(b);                                     \
    if (!(mtest_a_ > mtest_b_)) {                                          \
        MTEST_FAIL_("%" PRIdMAX " not > %" PRIdMAX,                     \
                    mtest_a_, mtest_b_);                                   \
    }                                                                      \
} while (0)

/** Assert a >= b. */
#define MTEST_ASSERT_GE(a, b) do {                                        \
    mtest_g.asserts_total++;                                               \
    intmax_t mtest_a_ = (intmax_t)(a);                                     \
    intmax_t mtest_b_ = (intmax_t)(b);                                     \
    if (!(mtest_a_ >= mtest_b_)) {                                         \
        MTEST_FAIL_("%" PRIdMAX " not >= %" PRIdMAX,                   \
                    mtest_a_, mtest_b_);                                   \
    }                                                                      \
} while (0)

/** Assert a < b. */
#define MTEST_ASSERT_LT(a, b) do {                                        \
    mtest_g.asserts_total++;                                               \
    intmax_t mtest_a_ = (intmax_t)(a);                                     \
    intmax_t mtest_b_ = (intmax_t)(b);                                     \
    if (!(mtest_a_ < mtest_b_)) {                                          \
        MTEST_FAIL_("%" PRIdMAX " not < %" PRIdMAX,                     \
                    mtest_a_, mtest_b_);                                   \
    }                                                                      \
} while (0)

/** Assert a <= b. */
#define MTEST_ASSERT_LE(a, b) do {                                        \
    mtest_g.asserts_total++;                                               \
    intmax_t mtest_a_ = (intmax_t)(a);                                     \
    intmax_t mtest_b_ = (intmax_t)(b);                                     \
    if (!(mtest_a_ <= mtest_b_)) {                                         \
        MTEST_FAIL_("%" PRIdMAX " not <= %" PRIdMAX,                   \
                    mtest_a_, mtest_b_);                                   \
    }                                                                      \
} while (0)

/** Assert two int32_t values are equal. */
#define MTEST_ASSERT_EQ_I32(expected, actual) do {                         \
    mtest_g.asserts_total++;                                               \
    int32_t mtest_e_ = (int32_t)(expected);                                \
    int32_t mtest_a_ = (int32_t)(actual);                                  \
    if (mtest_e_ != mtest_a_) {                                            \
        MTEST_FAIL_("expected %" PRId32 ", got %" PRId32,              \
                    mtest_e_, mtest_a_);                                   \
    }                                                                      \
} while (0)

#define MTEST_ASSERT_NE_I32(expected, actual) do {                         \
    mtest_g.asserts_total++;                                               \
    int32_t mtest_e_ = (int32_t)(expected);                                \
    int32_t mtest_a_ = (int32_t)(actual);                                  \
    if (mtest_e_ == mtest_a_) {                                            \
        MTEST_FAIL_("expected != %" PRId32 ", got %" PRId32,            \
                    mtest_e_, mtest_a_);                                   \
    }                                                                      \
} while (0)

#define MTEST_ASSERT_GT_I32(expected, actual) do {                         \
    mtest_g.asserts_total++;                                               \
    int32_t mtest_e_ = (int32_t)(expected);                                \
    int32_t mtest_a_ = (int32_t)(actual);                                  \
    if (!(mtest_e_ > mtest_a_)) {                                          \
        MTEST_FAIL_("%" PRId32 " not > %" PRId32,                       \
                    mtest_e_, mtest_a_);                                   \
    }                                                                      \
} while (0)

#define MTEST_ASSERT_GE_I32(expected, actual) do {                         \
    mtest_g.asserts_total++;                                               \
    int32_t mtest_e_ = (int32_t)(expected);                                \
    int32_t mtest_a_ = (int32_t)(actual);                                  \
    if (!(mtest_e_ >= mtest_a_)) {                                         \
        MTEST_FAIL_("%" PRId32 " not >= %" PRId32,                     \
                    mtest_e_, mtest_a_);                                   \
    }                                                                      \
} while (0)

#define MTEST_ASSERT_LT_I32(expected, actual) do {                         \
    mtest_g.asserts_total++;                                               \
    int32_t mtest_e_ = (int32_t)(expected);                                \
    int32_t mtest_a_ = (int32_t)(actual);                                  \
    if (!(mtest_e_ < mtest_a_)) {                                          \
        MTEST_FAIL_("%" PRId32 " not < %" PRId32,                       \
                    mtest_e_, mtest_a_);                                   \
    }                                                                      \
} while (0)

#define MTEST_ASSERT_LE_I32(expected, actual) do {                         \
    mtest_g.asserts_total++;                                               \
    int32_t mtest_e_ = (int32_t)(expected);                                \
    int32_t mtest_a_ = (int32_t)(actual);                                  \
    if (!(mtest_e_ <= mtest_a_)) {                                         \
        MTEST_FAIL_("%" PRId32 " not <= %" PRId32,                     \
                    mtest_e_, mtest_a_);                                   \
    }                                                                      \
} while (0)

#define MTEST_ASSERT_EQ_U32(expected, actual) do {                         \
    mtest_g.asserts_total++;                                               \
    uint32_t mtest_e_ = (uint32_t)(expected);                              \
    uint32_t mtest_a_ = (uint32_t)(actual);                                \
    if (mtest_e_ != mtest_a_) {                                            \
        MTEST_FAIL_("expected %" PRIu32 ", got %" PRIu32,              \
                    mtest_e_, mtest_a_);                                   \
    }                                                                      \
} while (0)

#define MTEST_ASSERT_NE_U32(expected, actual) do {                         \
    mtest_g.asserts_total++;                                               \
    uint32_t mtest_e_ = (uint32_t)(expected);                              \
    uint32_t mtest_a_ = (uint32_t)(actual);                                \
    if (mtest_e_ == mtest_a_) {                                            \
        MTEST_FAIL_("expected != %" PRIu32 ", got %" PRIu32,            \
                    mtest_e_, mtest_a_);                                   \
    }                                                                      \
} while (0)

#define MTEST_ASSERT_GT_U32(expected, actual) do {                         \
    mtest_g.asserts_total++;                                               \
    uint32_t mtest_e_ = (uint32_t)(expected);                              \
    uint32_t mtest_a_ = (uint32_t)(actual);                                \
    if (!(mtest_e_ > mtest_a_)) {                                          \
        MTEST_FAIL_("%" PRIu32 " not > %" PRIu32,                       \
                    mtest_e_, mtest_a_);                                   \
    }                                                                      \
} while (0)

#define MTEST_ASSERT_GE_U32(expected, actual) do {                         \
    mtest_g.asserts_total++;                                               \
    uint32_t mtest_e_ = (uint32_t)(expected);                              \
    uint32_t mtest_a_ = (uint32_t)(actual);                                \
    if (!(mtest_e_ >= mtest_a_)) {                                         \
        MTEST_FAIL_("%" PRIu32 " not >= %" PRIu32,                     \
                    mtest_e_, mtest_a_);                                   \
    }                                                                      \
} while (0)

#define MTEST_ASSERT_LT_U32(expected, actual) do {                         \
    mtest_g.asserts_total++;                                               \
    uint32_t mtest_e_ = (uint32_t)(expected);                              \
    uint32_t mtest_a_ = (uint32_t)(actual);                                \
    if (!(mtest_e_ < mtest_a_)) {                                          \
        MTEST_FAIL_("%" PRIu32 " not < %" PRIu32,                       \
                    mtest_e_, mtest_a_);                                   \
    }                                                                      \
} while (0)

#define MTEST_ASSERT_LE_U32(expected, actual) do {                         \
    mtest_g.asserts_total++;                                               \
    uint32_t mtest_e_ = (uint32_t)(expected);                              \
    uint32_t mtest_a_ = (uint32_t)(actual);                                \
    if (!(mtest_e_ <= mtest_a_)) {                                         \
        MTEST_FAIL_("%" PRIu32 " not <= %" PRIu32,                     \
                    mtest_e_, mtest_a_);                                   \
    }                                                                      \
} while (0)

#define MTEST_ASSERT_EQ_I64(expected, actual) do {                         \
    mtest_g.asserts_total++;                                               \
    int64_t mtest_e_ = (int64_t)(expected);                                \
    int64_t mtest_a_ = (int64_t)(actual);                                  \
    if (mtest_e_ != mtest_a_) {                                            \
        MTEST_FAIL_("expected %" PRId64 ", got %" PRId64,              \
                    mtest_e_, mtest_a_);                                   \
    }                                                                      \
} while (0)

#define MTEST_ASSERT_NE_I64(expected, actual) do {                         \
    mtest_g.asserts_total++;                                               \
    int64_t mtest_e_ = (int64_t)(expected);                                \
    int64_t mtest_a_ = (int64_t)(actual);                                  \
    if (mtest_e_ == mtest_a_) {                                            \
        MTEST_FAIL_("expected != %" PRId64 ", got %" PRId64,            \
                    mtest_e_, mtest_a_);                                   \
    }                                                                      \
} while (0)

#define MTEST_ASSERT_GT_I64(expected, actual) do {                         \
    mtest_g.asserts_total++;                                               \
    int64_t mtest_e_ = (int64_t)(expected);                                \
    int64_t mtest_a_ = (int64_t)(actual);                                  \
    if (!(mtest_e_ > mtest_a_)) {                                          \
        MTEST_FAIL_("%" PRId64 " not > %" PRId64,                       \
                    mtest_e_, mtest_a_);                                   \
    }                                                                      \
} while (0)

#define MTEST_ASSERT_GE_I64(expected, actual) do {                         \
    mtest_g.asserts_total++;                                               \
    int64_t mtest_e_ = (int64_t)(expected);                                \
    int64_t mtest_a_ = (int64_t)(actual);                                  \
    if (!(mtest_e_ >= mtest_a_)) {                                         \
        MTEST_FAIL_("%" PRId64 " not >= %" PRId64,                     \
                    mtest_e_, mtest_a_);                                   \
    }                                                                      \
} while (0)

#define MTEST_ASSERT_LT_I64(expected, actual) do {                         \
    mtest_g.asserts_total++;                                               \
    int64_t mtest_e_ = (int64_t)(expected);                                \
    int64_t mtest_a_ = (int64_t)(actual);                                  \
    if (!(mtest_e_ < mtest_a_)) {                                          \
        MTEST_FAIL_("%" PRId64 " not < %" PRId64,                       \
                    mtest_e_, mtest_a_);                                   \
    }                                                                      \
} while (0)

#define MTEST_ASSERT_LE_I64(expected, actual) do {                         \
    mtest_g.asserts_total++;                                               \
    int64_t mtest_e_ = (int64_t)(expected);                                \
    int64_t mtest_a_ = (int64_t)(actual);                                  \
    if (!(mtest_e_ <= mtest_a_)) {                                         \
        MTEST_FAIL_("%" PRId64 " not <= %" PRId64,                     \
                    mtest_e_, mtest_a_);                                   \
    }                                                                      \
} while (0)

#define MTEST_ASSERT_EQ_U64(expected, actual) do {                         \
    mtest_g.asserts_total++;                                               \
    uint64_t mtest_e_ = (uint64_t)(expected);                              \
    uint64_t mtest_a_ = (uint64_t)(actual);                                \
    if (mtest_e_ != mtest_a_) {                                            \
        MTEST_FAIL_("expected %" PRIu64 ", got %" PRIu64,              \
                    mtest_e_, mtest_a_);                                   \
    }                                                                      \
} while (0)

#define MTEST_ASSERT_NE_U64(expected, actual) do {                         \
    mtest_g.asserts_total++;                                               \
    uint64_t mtest_e_ = (uint64_t)(expected);                              \
    uint64_t mtest_a_ = (uint64_t)(actual);                                \
    if (mtest_e_ == mtest_a_) {                                            \
        MTEST_FAIL_("expected != %" PRIu64 ", got %" PRIu64,            \
                    mtest_e_, mtest_a_);                                   \
    }                                                                      \
} while (0)

#define MTEST_ASSERT_GT_U64(expected, actual) do {                         \
    mtest_g.asserts_total++;                                               \
    uint64_t mtest_e_ = (uint64_t)(expected);                              \
    uint64_t mtest_a_ = (uint64_t)(actual);                                \
    if (!(mtest_e_ > mtest_a_)) {                                          \
        MTEST_FAIL_("%" PRIu64 " not > %" PRIu64,                       \
                    mtest_e_, mtest_a_);                                   \
    }                                                                      \
} while (0)

#define MTEST_ASSERT_GE_U64(expected, actual) do {                         \
    mtest_g.asserts_total++;                                               \
    uint64_t mtest_e_ = (uint64_t)(expected);                              \
    uint64_t mtest_a_ = (uint64_t)(actual);                                \
    if (!(mtest_e_ >= mtest_a_)) {                                         \
        MTEST_FAIL_("%" PRIu64 " not >= %" PRIu64,                     \
                    mtest_e_, mtest_a_);                                   \
    }                                                                      \
} while (0)

#define MTEST_ASSERT_LT_U64(expected, actual) do {                         \
    mtest_g.asserts_total++;                                               \
    uint64_t mtest_e_ = (uint64_t)(expected);                              \
    uint64_t mtest_a_ = (uint64_t)(actual);                                \
    if (!(mtest_e_ < mtest_a_)) {                                          \
        MTEST_FAIL_("%" PRIu64 " not < %" PRIu64,                       \
                    mtest_e_, mtest_a_);                                   \
    }                                                                      \
} while (0)

#define MTEST_ASSERT_LE_U64(expected, actual) do {                         \
    mtest_g.asserts_total++;                                               \
    uint64_t mtest_e_ = (uint64_t)(expected);                              \
    uint64_t mtest_a_ = (uint64_t)(actual);                                \
    if (!(mtest_e_ <= mtest_a_)) {                                         \
        MTEST_FAIL_("%" PRIu64 " not <= %" PRIu64,                     \
                    mtest_e_, mtest_a_);                                   \
    }                                                                      \
} while (0)

#define MTEST_ASSERT_EQ_SIZE(expected, actual) do {                        \
    mtest_g.asserts_total++;                                               \
    size_t mtest_e_ = (size_t)(expected);                                  \
    size_t mtest_a_ = (size_t)(actual);                                    \
    if (mtest_e_ != mtest_a_) {                                            \
        MTEST_FAIL_("expected %zu, got %zu", mtest_e_, mtest_a_);        \
    }                                                                      \
} while (0)

#define MTEST_ASSERT_NE_SIZE(expected, actual) do {                        \
    mtest_g.asserts_total++;                                               \
    size_t mtest_e_ = (size_t)(expected);                                  \
    size_t mtest_a_ = (size_t)(actual);                                    \
    if (mtest_e_ == mtest_a_) {                                            \
        MTEST_FAIL_("expected != %zu, got %zu", mtest_e_, mtest_a_);    \
    }                                                                      \
} while (0)

#define MTEST_ASSERT_GT_SIZE(expected, actual) do {                        \
    mtest_g.asserts_total++;                                               \
    size_t mtest_e_ = (size_t)(expected);                                  \
    size_t mtest_a_ = (size_t)(actual);                                    \
    if (!(mtest_e_ > mtest_a_)) {                                          \
        MTEST_FAIL_("%zu not > %zu", mtest_e_, mtest_a_);                \
    }                                                                      \
} while (0)

#define MTEST_ASSERT_GE_SIZE(expected, actual) do {                        \
    mtest_g.asserts_total++;                                               \
    size_t mtest_e_ = (size_t)(expected);                                  \
    size_t mtest_a_ = (size_t)(actual);                                    \
    if (!(mtest_e_ >= mtest_a_)) {                                         \
        MTEST_FAIL_("%zu not >= %zu", mtest_e_, mtest_a_);              \
    }                                                                      \
} while (0)

#define MTEST_ASSERT_LT_SIZE(expected, actual) do {                        \
    mtest_g.asserts_total++;                                               \
    size_t mtest_e_ = (size_t)(expected);                                  \
    size_t mtest_a_ = (size_t)(actual);                                    \
    if (!(mtest_e_ < mtest_a_)) {                                          \
        MTEST_FAIL_("%zu not < %zu", mtest_e_, mtest_a_);                \
    }                                                                      \
} while (0)

#define MTEST_ASSERT_LE_SIZE(expected, actual) do {                        \
    mtest_g.asserts_total++;                                               \
    size_t mtest_e_ = (size_t)(expected);                                  \
    size_t mtest_a_ = (size_t)(actual);                                    \
    if (!(mtest_e_ <= mtest_a_)) {                                         \
        MTEST_FAIL_("%zu not <= %zu", mtest_e_, mtest_a_);              \
    }                                                                      \
} while (0)

/** Assert expression is true. */
#define MTEST_ASSERT_TRUE(expr) do {                                      \
    mtest_g.asserts_total++;                                               \
    int mtest_expr_ = (expr);                                              \
    if (!mtest_expr_) {                                                    \
        MTEST_FAIL_("expected true: %s", #expr);                          \
    }                                                                      \
} while (0)

/** Assert expression is false. */
#define MTEST_ASSERT_FALSE(expr) do {                                     \
    mtest_g.asserts_total++;                                               \
    int mtest_expr_ = (expr);                                              \
    if (mtest_expr_) {                                                     \
        MTEST_FAIL_("expected false: %s", #expr);                         \
    }                                                                      \
} while (0)

/** Assert pointer is NULL. */
#define MTEST_ASSERT_NULL(ptr) do {                                       \
    mtest_g.asserts_total++;                                               \
    void *mtest_ptr_ = (void *)(ptr);                                      \
    if (mtest_ptr_ != NULL) {                                              \
        MTEST_FAIL_("expected NULL, got %p", mtest_ptr_);                 \
    }                                                                      \
} while (0)

/** Assert pointer is not NULL. */
#define MTEST_ASSERT_NOT_NULL(ptr) do {                                   \
    mtest_g.asserts_total++;                                               \
    void *mtest_ptr_ = (void *)(ptr);                                      \
    if (mtest_ptr_ == NULL) {                                              \
        MTEST_FAIL_("expected non-NULL: %s", #ptr);                       \
    }                                                                      \
} while (0)

/** Assert two strings are equal. */
#define MTEST_ASSERT_STR_EQ(expected, actual) do {                        \
    mtest_g.asserts_total++;                                               \
    const char *mtest_e_ = (expected);                                     \
    const char *mtest_a_ = (actual);                                       \
    if (mtest_e_ == NULL || mtest_a_ == NULL) {                            \
        if (mtest_e_ != mtest_a_) {                                        \
            MTEST_FAIL_("string: expected \"%s\", got \"%s\"",             \
                        mtest_e_ ? mtest_e_ : "(null)",                    \
                        mtest_a_ ? mtest_a_ : "(null)");                   \
        }                                                                  \
    } else if (strcmp(mtest_e_, mtest_a_) != 0) {                          \
        MTEST_FAIL_("expected \"%s\", got \"%s\"", mtest_e_, mtest_a_);   \
    }                                                                      \
} while (0)

/** Assert string contains substring. */
#define MTEST_ASSERT_STR_CONTAINS(haystack, needle) do {                  \
    mtest_g.asserts_total++;                                               \
    const char *mtest_h_ = (haystack);                                     \
    const char *mtest_n_ = (needle);                                       \
    if (mtest_h_ == NULL || mtest_n_ == NULL ||                            \
        strstr(mtest_h_, mtest_n_) == NULL) {                              \
        MTEST_FAIL_("\"%s\" not found in \"%s\"",                    \
                    mtest_n_ ? mtest_n_ : "(null)",                      \
                    mtest_h_ ? mtest_h_ : "(null)");                      \
    }                                                                      \
} while (0)

/** Assert two memory regions are equal. */
#define MTEST_ASSERT_MEM_EQ(expected, actual, len) do {                   \
    mtest_g.asserts_total++;                                               \
    const void *mtest_e_ = (expected);                                     \
    const void *mtest_a_ = (actual);                                       \
    size_t mtest_len_ = (size_t)(len);                                     \
    if (mtest_e_ == NULL || mtest_a_ == NULL) {                            \
        if (mtest_e_ != mtest_a_) {                                        \
            MTEST_FAIL_("memory mismatch (%zu bytes): expected %p, got %p",\
                        mtest_len_, mtest_e_, mtest_a_);                  \
        }                                                                  \
    } else if (memcmp(mtest_e_, mtest_a_, mtest_len_) != 0) {              \
        MTEST_FAIL_("memory mismatch (%zu bytes)", mtest_len_);           \
    }                                                                      \
} while (0)

/** Assert two floats are approximately equal. */
#define MTEST_ASSERT_FLOAT_EQ(expected, actual) do {                      \
    mtest_g.asserts_total++;                                               \
    float mtest_e_ = (float)(expected);                                    \
    float mtest_a_ = (float)(actual);                                      \
    if (fabsf(mtest_e_ - mtest_a_) > 0.001f) {                            \
        MTEST_FAIL_("expected %f, got %f", (double)mtest_e_,              \
                     (double)mtest_a_);                                    \
    }                                                                      \
} while (0)

/** Assert two doubles are approximately equal with custom epsilon. */
#define MTEST_ASSERT_NEAR(expected, actual, epsilon) do {                  \
    mtest_g.asserts_total++;                                               \
    double mtest_e_ = (double)(expected);                                  \
    double mtest_a_ = (double)(actual);                                    \
    double mtest_eps_ = (double)(epsilon);                                 \
    if (fabs(mtest_e_ - mtest_a_) > mtest_eps_) {                         \
        MTEST_FAIL_("expected %f Â± %f, got %f",                           \
                     mtest_e_, mtest_eps_, mtest_a_);                      \
    }                                                                      \
} while (0)

/** Unconditional fail. */
#define MTEST_FAIL(msg) MTEST_FAIL_("%s", (msg))

/** Skip current test. */
#define MTEST_SKIP(reason) do {                                           \
    if (mtest_g.current_phase == MTEST_PHASE_TEARDOWN) {                   \
        mtest_g.current_outcome = MTEST_OUTCOME_FAILED;                    \
        if (mtest_g.current_failed == 0) {                                 \
            mtest_g.tests_failed++;                                        \
        }                                                                  \
        mtest_g.current_failed++;                                          \
        printf(MTEST_CLR_RED "FAIL" MTEST_CLR_RESET "\n");                \
        printf("    " MTEST_CLR_RED "%s:%d: teardown cannot skip: %s"     \
               MTEST_CLR_RESET "\n", __FILE__, __LINE__, (reason));       \
        return;                                                            \
    }                                                                      \
    mtest_g.current_outcome = MTEST_OUTCOME_SKIPPED;                       \
    mtest_g.current_skip_reason = (reason);                                \
    printf(MTEST_CLR_YELLOW "SKIP" MTEST_CLR_RESET " (%s)\n", (reason)); \
    mtest_g.tests_skipped++;                                               \
    return;                                                                \
} while (0)

/* â”€â”€ Begin / End â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€ */

/**
 * Parse CLI args and print header.
 * Supports: --filter=NAME, -v (verbose), -x (stop on fail), -l (list).
 */
#define MTEST_BEGIN(argc, argv) do {                                      \
    memset(&mtest_g, 0, sizeof(mtest_g));                                  \
    for (int i_ = 1; i_ < (argc); i_++) {                                 \
        if (strncmp((argv)[i_], "--filter=", 9) == 0) {                    \
            mtest_g.filter = (argv)[i_] + 9;                               \
        } else if (strcmp((argv)[i_], "-v") == 0) {                        \
            mtest_g.verbose = 1;                                           \
        } else if (strcmp((argv)[i_], "-x") == 0) {                        \
            mtest_g.stop_on_fail = 1;                                      \
        } else if (strcmp((argv)[i_], "-l") == 0) {                        \
            mtest_g.list_only = 1;                                         \
        } else if (strcmp((argv)[i_], "-h") == 0 ||                        \
                   strcmp((argv)[i_], "--help") == 0) {                     \
            printf("Usage: %s [options]\n"                                 \
                   "  --filter=NAME  Run only tests matching NAME\n"       \
                   "  -v             Verbose output\n"                      \
                   "  -x             Stop on first failure\n"              \
                   "  -l             List tests without running\n",        \
                   (argv)[0]);                                              \
            exit(0);                                                        \
        }                                                                  \
    }                                                                      \
    printf("\n" MTEST_CLR_BOLD "=== %s ===" MTEST_CLR_RESET "\n",          \
           (argv)[0]);                                                     \
    if (mtest_g.filter) {                                                  \
        printf(MTEST_CLR_DIM "  filter: %s" MTEST_CLR_RESET "\n",         \
               mtest_g.filter);                                            \
    }                                                                      \
} while (0)

/** Print summary and return exit code. */
#define MTEST_END()  mtest_end_()

/* â”€â”€ Implementation (define in exactly one .c file) â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€ */

#ifdef MTEST_IMPLEMENTATION

mtest_state_t mtest_g;

static int mtest_end_(void) {
    printf("\n" MTEST_CLR_BOLD "=== Results: ");

    if (mtest_g.tests_failed == 0) {
        printf(MTEST_CLR_GREEN "%d/%d passed",
               mtest_g.tests_passed, mtest_g.tests_run);
    } else {
        printf(MTEST_CLR_RED "%d/%d passed, %d FAILED",
               mtest_g.tests_passed, mtest_g.tests_run,
               mtest_g.tests_failed);
    }

    if (mtest_g.tests_skipped > 0) {
        printf(MTEST_CLR_YELLOW ", %d skipped", mtest_g.tests_skipped);
    }

    if (mtest_g.tests_filtered_out > 0) {
        printf(MTEST_CLR_CYAN ", %d filtered", mtest_g.tests_filtered_out);
    }

    printf(MTEST_CLR_RESET MTEST_CLR_BOLD " ===" MTEST_CLR_RESET);
    printf(MTEST_CLR_DIM " (discovered=%d selected=%d run=%d suites=%d asserts=%d failed_asserts=%d)"
           MTEST_CLR_RESET,
           mtest_g.tests_discovered,
           mtest_g.tests_selected,
           mtest_g.tests_run,
           mtest_g.suites_entered,
           mtest_g.asserts_total,
           mtest_g.asserts_failed);
    printf("\n\n");

    return mtest_g.tests_failed > 0 ? 1 : 0;
}

#endif /* MTEST_IMPLEMENTATION */

#endif /* MTEST_H */

