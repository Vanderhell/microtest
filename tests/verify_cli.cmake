if(NOT DEFINED TEST_EXE)
    message(FATAL_ERROR "TEST_EXE is required")
endif()

string(ASCII 27 esc)

function(expect_run expected_code)
    set(options)
    set(one_value NAME)
    set(multi_value ARGS MUST_CONTAIN MUST_NOT_CONTAIN)
    cmake_parse_arguments(RUN "${options}" "${one_value}" "${multi_value}" ${ARGN})

    execute_process(
        COMMAND "${TEST_EXE}" ${RUN_ARGS}
        RESULT_VARIABLE run_code
        OUTPUT_VARIABLE run_stdout
        ERROR_VARIABLE run_stderr
    )

    if(NOT run_code EQUAL expected_code)
        message(FATAL_ERROR
            "${RUN_NAME}: expected exit ${expected_code}, got ${run_code}\nstdout:\n${run_stdout}\nstderr:\n${run_stderr}")
    endif()

    set(run_output "${run_stdout}${run_stderr}")
    foreach(needle IN LISTS RUN_MUST_CONTAIN)
        string(FIND "${run_output}" "${needle}" found_at)
        if(found_at EQUAL -1)
            message(FATAL_ERROR
                "${RUN_NAME}: missing output '${needle}'\noutput:\n${run_output}")
        endif()
    endforeach()
    foreach(needle IN LISTS RUN_MUST_NOT_CONTAIN)
        string(FIND "${run_output}" "${needle}" found_at)
        if(NOT found_at EQUAL -1)
            message(FATAL_ERROR
                "${RUN_NAME}: unexpected output '${needle}'\noutput:\n${run_output}")
        endif()
    endforeach()
endfunction()

expect_run(0 NAME help ARGS --help MUST_CONTAIN "Usage:" "--filter=NAME")
expect_run(1 NAME empty-filter ARGS --filter= MUST_CONTAIN "Usage:" "--filter=NAME")
expect_run(1 NAME unknown ARGS --no-such-option MUST_CONTAIN "Unknown option: --no-such-option" "Usage:")
expect_run(1 NAME no-match ARGS --filter=THIS_FILTER_MUST_NOT_MATCH MUST_CONTAIN "No tests matched filter: THIS_FILTER_MUST_NOT_MATCH")
expect_run(0 NAME list-mode ARGS -l --no-color MUST_CONTAIN "test_assert_eq" MUST_NOT_CONTAIN "PASS" "FAIL")
expect_run(0 NAME verbose ARGS -v --no-color MUST_CONTAIN "asserts:" "PASS" MUST_NOT_CONTAIN "${esc}")
expect_run(0 NAME no-color ARGS --no-color --filter=test_assert_eq MUST_CONTAIN "test_assert_eq" "PASS" MUST_NOT_CONTAIN "${esc}")
