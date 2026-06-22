if(NOT DEFINED PROJECT_BINARY_DIR OR NOT DEFINED PROJECT_SOURCE_DIR)
    message(FATAL_ERROR "PROJECT_BINARY_DIR and PROJECT_SOURCE_DIR are required")
endif()

set(install_prefix "${PROJECT_BINARY_DIR}/_install_test")
set(consumer_build "${PROJECT_BINARY_DIR}/_consumer_test")
set(consumer_cache "${PROJECT_BINARY_DIR}/_consumer_cache.cmake")

file(REMOVE_RECURSE "${consumer_build}")
file(REMOVE_RECURSE "${install_prefix}")
file(REMOVE "${consumer_cache}")
file(WRITE
    "${consumer_cache}"
    "set(CMAKE_PREFIX_PATH \"${install_prefix}\" CACHE PATH \"\")\n"
    "set(microtest_DIR \"${install_prefix}/lib/cmake/microtest\" CACHE PATH \"\")\n"
)

execute_process(
    COMMAND "${CMAKE_COMMAND}" --install "${PROJECT_BINARY_DIR}" --prefix "${install_prefix}"
    RESULT_VARIABLE install_code
)
if(NOT install_code EQUAL 0)
    message(FATAL_ERROR "install failed with exit ${install_code}")
endif()

execute_process(
    COMMAND "${CMAKE_COMMAND}"
        -S "${PROJECT_SOURCE_DIR}/tests/consumer"
        -B "${consumer_build}"
        -T ClangCL
        -C "${consumer_cache}"
    RESULT_VARIABLE configure_code
)
if(NOT configure_code EQUAL 0)
    message(FATAL_ERROR "consumer configure failed with exit ${configure_code}")
endif()

execute_process(
    COMMAND "${CMAKE_COMMAND}" --build "${consumer_build}" --config Debug
    RESULT_VARIABLE build_code
)
if(NOT build_code EQUAL 0)
    message(FATAL_ERROR "consumer build failed with exit ${build_code}")
endif()

if(WIN32)
    set(consumer_exe "${consumer_build}/Debug/consumer.exe")
else()
    set(consumer_exe "${consumer_build}/consumer")
endif()

execute_process(
    COMMAND "${consumer_exe}"
    RESULT_VARIABLE run_code
)
if(NOT run_code EQUAL 0)
    message(FATAL_ERROR "consumer run failed with exit ${run_code}")
endif()
