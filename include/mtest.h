/*
 * microtest â€” Single-header test framework for embedded C libraries.
 *
 * Replaces the ad-hoc test macros scattered across the micro* ecosystem.
 * Provides: assertions, test registration, suites, setup/teardown, filtering,
 * color output, output callbacks, and summary stats.
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
#include <stdarg.h>
#include <string.h>
#include <stdlib.h>
#include <stdint.h>
#include <inttypes.h>

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

typedef enum mtest_begin_result {
    MTEST_BEGIN_OK = 0,
    MTEST_BEGIN_HELP_REQUESTED,
    MTEST_BEGIN_INVALID_ARGUMENT
} mtest_begin_result_t;

typedef void (*mtest_write_fn)(void *user, const char *data, size_t length);

/* â”€â”€ Configuration â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€ */

/** Enable ANSI color output. */
#ifndef MTEST_ENABLE_COLOR
#define MTEST_ENABLE_COLOR 1
#endif

/** Enable command-line parsing. */
#ifndef MTEST_ENABLE_CLI
#define MTEST_ENABLE_CLI 1
#endif

/** Enable environment-variable integration. */
#ifndef MTEST_ENABLE_ENV
#define MTEST_ENABLE_ENV 1
#endif

/** Enable process exit helpers in hosted builds. */
#ifndef MTEST_ENABLE_EXIT
#define MTEST_ENABLE_EXIT 1
#endif

/** Enable floating-point assertions and math helpers. */
#ifndef MTEST_ENABLE_FLOAT
#define MTEST_ENABLE_FLOAT 1
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
    int         color_enabled;      /* runtime color toggle              */
    mtest_write_fn write_fn;        /* output callback                   */
    void       *write_user;         /* callback user data                */
    mtest_begin_result_t begin_result;
} mtest_state_t;

/* Extern declaration â€” defined in MTEST_IMPLEMENTATION block */
extern mtest_state_t mtest_g;

static inline void mtest_default_write_(void *user, const char *data, size_t length) {
    (void)user;
    if (data != NULL && length > 0) {
        fwrite(data, 1, length, stdout);
    }
}

static inline void mtest_write_raw_(const char *data, size_t length) {
    if (mtest_g.write_fn != NULL) {
        mtest_g.write_fn(mtest_g.write_user, data, length);
    } else {
        mtest_default_write_(NULL, data, length);
    }
}

static inline void mtest_write_cstr_(const char *text) {
    if (text != NULL) {
        mtest_write_raw_(text, strlen(text));
    }
}

static inline void mtest_write_fmt_(const char *fmt, ...) {
    char buffer[512];
    va_list args;
    int written;
    va_start(args, fmt);
    written = vsnprintf(buffer, sizeof(buffer), fmt, args);
    va_end(args);
    if (written < 0) {
        return;
    }
    if ((size_t)written >= sizeof(buffer)) {
        written = (int)(sizeof(buffer) - 1);
    }
    mtest_write_raw_(buffer, (size_t)written);
}

static inline const char *mtest_color_(const char *enabled, const char *disabled) {
#if MTEST_ENABLE_COLOR
    return mtest_g.color_enabled ? enabled : disabled;
#else
    (void)enabled;
    return disabled;
#endif
}

static inline void mtest_set_output(void *user, mtest_write_fn fn) {
    mtest_g.write_user = user;
    mtest_g.write_fn = fn;
}

static inline void mtest_set_color(int enabled) {
#if MTEST_ENABLE_COLOR
    mtest_g.color_enabled = enabled ? 1 : 0;
#else
    (void)enabled;
    mtest_g.color_enabled = 0;
#endif
}

static inline int mtest_env_present_(const char *name) {
#if defined(_MSC_VER)
    size_t required_size = 0;
    if (getenv_s(&required_size, NULL, 0, name) != 0) {
        return 0;
    }
    return required_size != 0;
#else
    return getenv(name) != NULL;
#endif
}

static inline void mtest_print_usage_(const char *program) {
    mtest_write_fmt_("Usage: %s [options]\n", program);
    mtest_write_cstr_("  --filter=NAME  Run only tests matching NAME\n");
    mtest_write_cstr_("  -v, --verbose  Verbose output\n");
    mtest_write_cstr_("  -x, --stop-on-fail Stop on first failure\n");
    mtest_write_cstr_("  -l, --list     List tests without running\n");
    mtest_write_cstr_("  -h, --help     Show this help text\n");
    mtest_write_cstr_("  --no-color     Disable ANSI color output\n");
}

static inline mtest_begin_result_t mtest_begin_(int argc, char **argv) {
    mtest_write_fn previous_write_fn = mtest_g.write_fn;
    void *previous_write_user = mtest_g.write_user;

    memset(&mtest_g, 0, sizeof(mtest_g));
    mtest_g.write_fn = previous_write_fn != NULL ? previous_write_fn
                                                 : mtest_default_write_;
    mtest_g.write_user = previous_write_user;
    mtest_g.color_enabled = MTEST_ENABLE_COLOR ? 1 : 0;
    mtest_g.begin_result = MTEST_BEGIN_OK;

#if MTEST_ENABLE_ENV && MTEST_ENABLE_COLOR
    if (mtest_env_present_("NO_COLOR")) {
        mtest_g.color_enabled = 0;
    }
#endif

#if MTEST_ENABLE_CLI
    int i;
    for (i = 1; i < argc; ++i) {
        const char *arg = argv[i];
        if (strncmp(arg, "--filter=", 9) == 0) {
            const char *value = arg + 9;
            if (*value == '\0') {
                mtest_print_usage_(argv[0]);
                mtest_g.begin_result = MTEST_BEGIN_INVALID_ARGUMENT;
                return mtest_g.begin_result;
            }
            mtest_g.filter = value;
        } else if (strcmp(arg, "-v") == 0 || strcmp(arg, "--verbose") == 0) {
            mtest_g.verbose = 1;
        } else if (strcmp(arg, "-x") == 0 || strcmp(arg, "--stop-on-fail") == 0) {
            mtest_g.stop_on_fail = 1;
        } else if (strcmp(arg, "-l") == 0 || strcmp(arg, "--list") == 0) {
            mtest_g.list_only = 1;
        } else if (strcmp(arg, "-h") == 0 || strcmp(arg, "--help") == 0) {
            mtest_print_usage_(argv[0]);
            mtest_g.begin_result = MTEST_BEGIN_HELP_REQUESTED;
            return mtest_g.begin_result;
        } else if (strcmp(arg, "--no-color") == 0) {
            mtest_g.color_enabled = 0;
        } else {
            mtest_write_fmt_("Unknown option: %s\n", arg);
            mtest_print_usage_(argv[0]);
            mtest_g.begin_result = MTEST_BEGIN_INVALID_ARGUMENT;
            return mtest_g.begin_result;
        }
    }
#else
    (void)argc;
    (void)argv;
#endif

    mtest_write_fmt_("\n%s=== %s ===%s\n",
                     mtest_color_(MTEST_CLR_BOLD, ""),
                     argv[0],
                     mtest_color_(MTEST_CLR_RESET, ""));
    if (mtest_g.filter != NULL) {
        mtest_write_fmt_("%s  filter: %s%s\n",
                         mtest_color_(MTEST_CLR_DIM, ""),
                         mtest_g.filter,
                         mtest_color_(MTEST_CLR_RESET, ""));
    }
    return mtest_g.begin_result;
}

static inline int mtest_end_(void) {
    int exit_code;

    if (mtest_g.filter != NULL && mtest_g.tests_selected == 0) {
        mtest_write_fmt_("No tests matched filter: %s\n", mtest_g.filter);
        return 1;
    }

    mtest_write_fmt_("\n%s=== Results: ", mtest_color_(MTEST_CLR_BOLD, ""));
    if (mtest_g.tests_failed == 0) {
        mtest_write_fmt_("%s%d/%d passed",
                         mtest_color_(MTEST_CLR_GREEN, ""),
                         mtest_g.tests_passed,
                         mtest_g.tests_run);
    } else {
        mtest_write_fmt_("%s%d/%d passed, %d FAILED",
                         mtest_color_(MTEST_CLR_RED, ""),
                         mtest_g.tests_passed,
                         mtest_g.tests_run,
                         mtest_g.tests_failed);
    }
    if (mtest_g.tests_skipped > 0) {
        mtest_write_fmt_("%s, %d skipped",
                         mtest_color_(MTEST_CLR_YELLOW, ""),
                         mtest_g.tests_skipped);
    }
    if (mtest_g.tests_filtered_out > 0) {
        mtest_write_fmt_("%s, %d filtered",
                         mtest_color_(MTEST_CLR_CYAN, ""),
                         mtest_g.tests_filtered_out);
    }
    mtest_write_fmt_("%s ===%s",
                     mtest_color_(MTEST_CLR_RESET, ""),
                     mtest_color_(MTEST_CLR_RESET, ""));
    mtest_write_fmt_("%s (discovered=%d selected=%d run=%d suites=%d asserts=%d failed_asserts=%d)",
                     mtest_color_(MTEST_CLR_DIM, ""),
                     mtest_g.tests_discovered,
                     mtest_g.tests_selected,
                     mtest_g.tests_run,
                     mtest_g.suites_entered,
                     mtest_g.asserts_total,
                     mtest_g.asserts_failed);
    mtest_write_cstr_("\n\n");

    exit_code = (mtest_g.tests_failed > 0) ? 1 : 0;
    if (mtest_g.begin_result == MTEST_BEGIN_INVALID_ARGUMENT) {
        exit_code = 1;
    } else if (mtest_g.begin_result == MTEST_BEGIN_HELP_REQUESTED) {
        exit_code = 0;
    }
    return exit_code;
}

/* â”€â”€ Test definition macros â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€ */

/** Define a test function. */
#define MTEST(name)  static void name(void)

/** Define a suite function. */
#define MTEST_SUITE(name)  static void mtest_suite_##name(void)

/** Declare a suite defined in another translation unit. */
#define MTEST_SUITE_DECLARE(name)  void mtest_suite_##name(void)

/** Define a suite with external linkage for multi-TU use. */
#define MTEST_SUITE_DEFINE(name)  void mtest_suite_##name(void)

/** Run a suite. */
#define MTEST_SUITE_RUN(name)  do {                                       \
    if (mtest_g.stop_on_fail && mtest_g.tests_failed > 0) {                \
        break;                                                             \
    }                                                                      \
    mtest_g.current_suite = #name;                                         \
    mtest_g.suites_entered++;                                              \
    mtest_g.suites_run++;                                                  \
    mtest_write_fmt_("\n%s[%s]%s\n",                                       \
                     mtest_color_(MTEST_CLR_BOLD, ""),                    \
                     #name,                                                \
                     mtest_color_(MTEST_CLR_RESET, ""));                  \
    mtest_suite_##name();                                                  \
    mtest_g.current_suite = NULL;                                          \
} while (0)

/** Run a test within a suite. */
#define MTEST_RUN(name)  do {                                             \
    mtest_g.tests_discovered++;                                            \
    if (mtest_g.stop_on_fail && mtest_g.tests_failed > 0) {                \
        break;                                                             \
    }                                                                      \
    if (mtest_g.filter != NULL &&                                          \
        strstr(#name, mtest_g.filter) == NULL) {                           \
        mtest_g.tests_filtered_out++;                                      \
        break;                                                             \
    }                                                                      \
    if (mtest_g.list_only) {                                               \
        mtest_write_fmt_("  %s\n", #name);                                 \
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
    int mtest_asserts_before_ = mtest_g.asserts_total;                     \
    mtest_write_fmt_("  %-55s ", #name);                                   \
    name();                                                                \
    if (mtest_g.current_outcome == MTEST_OUTCOME_RUNNING) {                \
        mtest_g.current_outcome = MTEST_OUTCOME_PASSED;                    \
    }                                                                      \
    if (mtest_g.current_outcome == MTEST_OUTCOME_PASSED) {                 \
        mtest_g.tests_passed++;                                            \
        mtest_write_fmt_("%sPASS%s\n",                                     \
                         mtest_color_(MTEST_CLR_GREEN, ""),              \
                         mtest_color_(MTEST_CLR_RESET, ""));              \
        if (mtest_g.verbose) {                                             \
            mtest_write_fmt_("    asserts: %d\n",                          \
                             mtest_g.asserts_total - mtest_asserts_before_);\
        }                                                                  \
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
        break;                                                             \
    }                                                                      \
    if (mtest_g.filter != NULL &&                                          \
        strstr(#name, mtest_g.filter) == NULL) {                           \
        mtest_g.tests_filtered_out++;                                      \
        break;                                                             \
    }                                                                      \
    if (mtest_g.list_only) {                                               \
        mtest_write_fmt_("  %s\n", #name);                                 \
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
    int mtest_asserts_before_ = mtest_g.asserts_total;                     \
    mtest_write_fmt_("  %-55s ", #name);                                   \
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
        mtest_write_fmt_("%sPASS%s\n",                                     \
                         mtest_color_(MTEST_CLR_GREEN, ""),              \
                         mtest_color_(MTEST_CLR_RESET, ""));              \
        if (mtest_g.verbose) {                                             \
            mtest_write_fmt_("    asserts: %d\n",                          \
                             mtest_g.asserts_total - mtest_asserts_before_);\
        }                                                                  \
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
        mtest_write_fmt_("%sFAIL%s\n",                                    \
                         mtest_color_(MTEST_CLR_RED, ""),                \
                         mtest_color_(MTEST_CLR_RESET, ""));              \
        mtest_g.tests_failed++;                                            \
    }                                                                      \
    mtest_g.current_outcome = MTEST_OUTCOME_FAILED;                        \
    mtest_g.current_failed++;                                              \
    mtest_g.asserts_failed++;                                              \
    mtest_write_fmt_("    %s%s:%d: %s" fmt "\n",                           \
                     mtest_color_(MTEST_CLR_RED, ""),                    \
                     __FILE__, __LINE__,                                   \
                     mtest_color_(MTEST_CLR_RESET, ""), __VA_ARGS__);     \
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

/* Pointer assertions are defined for object pointers. Function pointers are
 * not portably printable through %p, so they are outside this API contract.
 */

/** Assert pointer is NULL. */
#define MTEST_ASSERT_NULL(ptr) do {                                       \
    mtest_g.asserts_total++;                                               \
    const void *mtest_ptr_ = (const void *)(ptr);                         \
    if (mtest_ptr_ != NULL) {                                              \
        MTEST_FAIL_("expected NULL, got %p", mtest_ptr_);                 \
    }                                                                      \
} while (0)

/** Assert pointer is not NULL. */
#define MTEST_ASSERT_NOT_NULL(ptr) do {                                   \
    mtest_g.asserts_total++;                                               \
    const void *mtest_ptr_ = (const void *)(ptr);                         \
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

/** Assert string contains substring.
 * Null haystack or needle fails safely. Empty needle follows strstr semantics.
 */
#define MTEST_ASSERT_STR_CONTAINS(haystack, needle) do {                  \
    mtest_g.asserts_total++;                                               \
    const char *mtest_h_ = (haystack);                                     \
    const char *mtest_n_ = (needle);                                       \
    if (mtest_h_ == NULL || mtest_n_ == NULL) {                            \
        MTEST_FAIL_("expected \"%s\" to contain \"%s\"",            \
                    mtest_h_ ? mtest_h_ : "(null)",                      \
                    mtest_n_ ? mtest_n_ : "(null)");                      \
    } else if (strstr(mtest_h_, mtest_n_) == NULL) {                       \
        MTEST_FAIL_("expected \"%s\" to contain \"%s\"",            \
                    mtest_h_, mtest_n_);                                  \
    }                                                                      \
} while (0)

/** Assert two memory regions are equal.
 * Zero length passes even when pointers are NULL. Negative lengths fail.
 */
#define MTEST_ASSERT_MEM_EQ(expected, actual, len) do {                   \
    mtest_g.asserts_total++;                                               \
    const unsigned char *mtest_e_ = (const unsigned char *)(expected);     \
    const unsigned char *mtest_a_ = (const unsigned char *)(actual);       \
    long long mtest_len_check_ = (long long)(len);                         \
    size_t mtest_len_ = (size_t)mtest_len_check_;                          \
    if (mtest_len_check_ < 0) {                                            \
        MTEST_FAIL_("memory length must be non-negative: %s", #len);     \
    } else if (mtest_len_ == 0) {                                          \
    } else if (mtest_e_ == NULL || mtest_a_ == NULL) {                     \
        MTEST_FAIL_("memory mismatch (%zu bytes): expected %p, got %p",  \
                    mtest_len_, (const void *)mtest_e_,                   \
                    (const void *)mtest_a_);                              \
    } else {                                                               \
        size_t mtest_i_;                                                   \
        for (mtest_i_ = 0; mtest_i_ < mtest_len_; ++mtest_i_) {            \
            if (mtest_e_[mtest_i_] != mtest_a_[mtest_i_]) {                \
                MTEST_FAIL_("memory mismatch (%zu bytes) at offset %zu: " \
                            "%02x != %02x",                               \
                            mtest_len_, mtest_i_,                          \
                            (unsigned)mtest_e_[mtest_i_],                  \
                            (unsigned)mtest_a_[mtest_i_]);                 \
            }                                                              \
        }                                                                  \
    }                                                                      \
} while (0)

#if MTEST_ENABLE_FLOAT

#include <math.h>

/** Assert two floats are exactly equal. NaN never passes. */
#define MTEST_ASSERT_FLOAT_EQ(expected, actual) do {                      \
    mtest_g.asserts_total++;                                               \
    float mtest_e_ = (float)(expected);                                    \
    float mtest_a_ = (float)(actual);                                      \
    if (mtest_e_ != mtest_a_) {                                            \
        MTEST_FAIL_("expected %g, got %g", (double)mtest_e_,             \
                    (double)mtest_a_);                                     \
    }                                                                      \
} while (0)

/** Assert two floats are approximately equal using absolute epsilon only. */
#define MTEST_ASSERT_NEAR(expected, actual, epsilon) do {                  \
    mtest_g.asserts_total++;                                               \
    double mtest_e_ = (double)(expected);                                  \
    double mtest_a_ = (double)(actual);                                    \
    double mtest_eps_ = (double)(epsilon);                                 \
    if (mtest_eps_ < 0.0) {                                                \
        MTEST_FAIL_("epsilon must be non-negative: %s", #epsilon);       \
    } else if (isnan(mtest_e_) || isnan(mtest_a_)) {                       \
        MTEST_FAIL_("%s", "expected finite values, got NaN");            \
    } else if (isinf(mtest_e_) || isinf(mtest_a_)) {                       \
        if (mtest_e_ != mtest_a_) {                                        \
            MTEST_FAIL_("expected %g, got %g", mtest_e_, mtest_a_);     \
        }                                                                  \
    } else if (fabs(mtest_e_ - mtest_a_) > mtest_eps_) {                   \
        MTEST_FAIL_("expected %g +/- %g, got %g",                         \
                    mtest_e_, mtest_eps_, mtest_a_);                       \
    }                                                                      \
} while (0)

/** Assert value is NaN. */
#define MTEST_ASSERT_IS_NAN(value) do {                                    \
    mtest_g.asserts_total++;                                               \
    double mtest_v_ = (double)(value);                                     \
    if (!isnan(mtest_v_)) {                                                \
        MTEST_FAIL_("expected NaN, got %g", mtest_v_);                   \
    }                                                                      \
} while (0)

/** Assert value is finite. */
#define MTEST_ASSERT_IS_FINITE(value) do {                                 \
    mtest_g.asserts_total++;                                               \
    double mtest_v_ = (double)(value);                                     \
    if (!isfinite(mtest_v_)) {                                             \
        MTEST_FAIL_("expected finite value, got %g", mtest_v_);          \
    }                                                                      \
} while (0)

/** Assert value is infinite. */
#define MTEST_ASSERT_IS_INF(value) do {                                   \
    mtest_g.asserts_total++;                                               \
    double mtest_v_ = (double)(value);                                     \
    if (!isinf(mtest_v_)) {                                                \
        MTEST_FAIL_("expected infinity, got %g", mtest_v_);              \
    }                                                                      \
} while (0)

#else

#define MTEST_ASSERT_FLOAT_EQ(...)                                         \
    MTEST_FLOAT_ASSERTIONS_DISABLED_USE_MTEST_ENABLE_FLOAT_1
#define MTEST_ASSERT_NEAR(...)                                             \
    MTEST_FLOAT_ASSERTIONS_DISABLED_USE_MTEST_ENABLE_FLOAT_2
#define MTEST_ASSERT_IS_NAN(...)                                           \
    MTEST_FLOAT_ASSERTIONS_DISABLED_USE_MTEST_ENABLE_FLOAT_3
#define MTEST_ASSERT_IS_FINITE(...)                                        \
    MTEST_FLOAT_ASSERTIONS_DISABLED_USE_MTEST_ENABLE_FLOAT_4
#define MTEST_ASSERT_IS_INF(...)                                           \
    MTEST_FLOAT_ASSERTIONS_DISABLED_USE_MTEST_ENABLE_FLOAT_5

#endif

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
        mtest_write_fmt_("%sFAIL%s\n",                                    \
                         mtest_color_(MTEST_CLR_RED, ""),                \
                         mtest_color_(MTEST_CLR_RESET, ""));              \
        mtest_write_fmt_("    %s%s:%d: teardown cannot skip: %s%s\n",      \
                         mtest_color_(MTEST_CLR_RED, ""),                \
                         __FILE__, __LINE__, (reason),                     \
                         mtest_color_(MTEST_CLR_RESET, ""));              \
        return;                                                            \
    }                                                                      \
    mtest_g.current_outcome = MTEST_OUTCOME_SKIPPED;                       \
    mtest_g.current_skip_reason = (reason);                                \
    mtest_write_fmt_("%sSKIP%s (%s)\n",                                   \
                     mtest_color_(MTEST_CLR_YELLOW, ""),                 \
                     mtest_color_(MTEST_CLR_RESET, ""),                   \
                     (reason));                                            \
    mtest_g.tests_skipped++;                                               \
    return;                                                                \
} while (0)

/* â”€â”€ Begin / End â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€ */

/**
 * Parse CLI args and print header.
 * Supports: --filter=NAME, -v/--verbose, -x/--stop-on-fail, -l/--list,
 * -h/--help, and --no-color.
 */
#define MTEST_BEGIN(argc, argv) mtest_begin_((argc), (argv))

/** Print summary and return exit code. */
#define MTEST_END()  mtest_end_()

/* â”€â”€ Implementation (define in exactly one .c file) â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€ */

#ifdef MTEST_IMPLEMENTATION

mtest_state_t mtest_g;

#endif /* MTEST_IMPLEMENTATION */

#endif /* MTEST_H */

