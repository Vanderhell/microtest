cmake_minimum_required(VERSION 3.16)
project(microtest VERSION 1.1.0 LANGUAGES C)

include(GNUInstallDirs)
include(CMakePackageConfigHelpers)

if(MSVC)
    foreach(flag_var CMAKE_C_FLAGS_DEBUG CMAKE_C_FLAGS_RELWITHDEBINFO)
        if(DEFINED ${flag_var})
            string(REPLACE "/RTC1" "" ${flag_var} "${${flag_var}}")
            set(${flag_var} "${${flag_var}}" CACHE STRING "" FORCE)
        endif()
    endforeach()
endif()

option(MICROTEST_BUILD_TESTS "Build microtest verification targets" ON)
option(MICROTEST_BUILD_EXAMPLES "Build example targets" OFF)
option(MICROTEST_ENABLE_COLOR "Enable ANSI color output" ON)
option(MICROTEST_ENABLE_FLOAT "Enable floating-point assertions" ON)
option(MICROTEST_ENABLE_CLI "Enable command-line parsing" ON)
option(MICROTEST_ENABLE_ENV "Enable environment-variable integration" ON)
option(MICROTEST_ENABLE_EXIT "Enable hosted exit helpers" ON)

add_library(microtest INTERFACE)
add_library(microtest::microtest ALIAS microtest)

target_compile_features(microtest INTERFACE c_std_99)
target_include_directories(microtest INTERFACE
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
)
target_compile_definitions(microtest INTERFACE
    $<$<NOT:$<BOOL:${MICROTEST_ENABLE_COLOR}>>:MTEST_ENABLE_COLOR=0>
    $<$<NOT:$<BOOL:${MICROTEST_ENABLE_FLOAT}>>:MTEST_ENABLE_FLOAT=0>
    $<$<NOT:$<BOOL:${MICROTEST_ENABLE_CLI}>>:MTEST_ENABLE_CLI=0>
    $<$<NOT:$<BOOL:${MICROTEST_ENABLE_ENV}>>:MTEST_ENABLE_ENV=0>
    $<$<NOT:$<BOOL:${MICROTEST_ENABLE_EXIT}>>:MTEST_ENABLE_EXIT=0>
)

set(microtest_package_dir "${CMAKE_INSTALL_LIBDIR}/cmake/microtest")
set(microtest_build_package_dir "${CMAKE_CURRENT_BINARY_DIR}/microtest")

configure_package_config_file(
    "${CMAKE_CURRENT_SOURCE_DIR}/cmake/microtestConfig.cmake.in"
    "${microtest_build_package_dir}/microtestConfig.cmake"
    INSTALL_DESTINATION "${microtest_package_dir}"
)
write_basic_package_version_file(
    "${microtest_build_package_dir}/microtestConfigVersion.cmake"
    VERSION "${PROJECT_VERSION}"
    COMPATIBILITY SameMinorVersion
)

install(TARGETS microtest EXPORT microtestTargets)
install(FILES include/mtest.h DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}")
export(
    EXPORT microtestTargets
    FILE "${microtest_build_package_dir}/microtestTargets.cmake"
    NAMESPACE microtest::
)
install(
    EXPORT microtestTargets
    NAMESPACE microtest::
    DESTINATION "${microtest_package_dir}"
)
install(
    FILES
        "${microtest_build_package_dir}/microtestConfig.cmake"
        "${microtest_build_package_dir}/microtestConfigVersion.cmake"
    DESTINATION "${microtest_package_dir}"
)

if(MICROTEST_BUILD_TESTS)
    enable_testing()

    function(microtest_add_executable target source)
        add_executable("${target}" "${source}")
        target_link_libraries("${target}" PRIVATE microtest)
        if(MICROTEST_ENABLE_FLOAT AND NOT MSVC)
            target_link_libraries("${target}" PRIVATE m)
        endif()
    endfunction()

    microtest_add_executable(test_microtest "${CMAKE_CURRENT_SOURCE_DIR}/tests/test_all.c")
    microtest_add_executable(test_microtest_float_off "${CMAKE_CURRENT_SOURCE_DIR}/tests/test_all.c")
    target_compile_definitions(test_microtest_float_off PRIVATE MTEST_ENABLE_FLOAT=0)

    microtest_add_executable(test_microtest_embedded "${CMAKE_CURRENT_SOURCE_DIR}/tests/test_all.c")
    target_compile_definitions(test_microtest_embedded PRIVATE
        MTEST_ENABLE_COLOR=0
        MTEST_ENABLE_FLOAT=0
        MTEST_ENABLE_CLI=0
        MTEST_ENABLE_ENV=0
        MTEST_ENABLE_EXIT=0
    )

    microtest_add_executable(test_output_callback "${CMAKE_CURRENT_SOURCE_DIR}/tests/output_callback.c")
    microtest_add_executable(test_output_callback_nocolor "${CMAKE_CURRENT_SOURCE_DIR}/tests/output_callback.c")
    target_compile_definitions(test_output_callback_nocolor PRIVATE MTEST_ENABLE_COLOR=0)

    microtest_add_executable(test_stop_on_fail "${CMAKE_CURRENT_SOURCE_DIR}/tests/stop_on_fail.c")
    microtest_add_executable(test_multi_tu "${CMAKE_CURRENT_SOURCE_DIR}/tests/multi_tu/main.c")
    target_sources(test_multi_tu PRIVATE
        "${CMAKE_CURRENT_SOURCE_DIR}/tests/multi_tu/test_math.c"
        "${CMAKE_CURRENT_SOURCE_DIR}/tests/multi_tu/test_strings.c"
    )

    if(MICROTEST_ENABLE_FLOAT AND NOT MSVC)
        target_link_libraries(test_output_callback PRIVATE m)
        target_link_libraries(test_output_callback_nocolor PRIVATE m)
        target_link_libraries(test_stop_on_fail PRIVATE m)
        target_link_libraries(test_multi_tu PRIVATE m)
    endif()

    add_test(NAME microtest_selftest COMMAND test_microtest)
    add_test(NAME microtest_selftest_float_off COMMAND test_microtest_float_off)
    add_test(NAME microtest_embedded_profile COMMAND test_microtest_embedded)
    add_test(NAME output_callback COMMAND test_output_callback)
    add_test(NAME output_callback_nocolor COMMAND test_output_callback_nocolor)
    add_test(NAME stop_on_fail COMMAND test_stop_on_fail)
    add_test(NAME multi_tu COMMAND test_multi_tu)

    add_test(
        NAME cli_behavior
        COMMAND "${CMAKE_COMMAND}"
            -DTEST_EXE=$<TARGET_FILE:test_microtest>
            -DTEST_DIR=${CMAKE_CURRENT_SOURCE_DIR}
            -P "${CMAKE_CURRENT_SOURCE_DIR}/tests/verify_cli.cmake"
    )
    add_test(
        NAME install_consumer
        COMMAND "${CMAKE_COMMAND}"
            -DPROJECT_BINARY_DIR=${CMAKE_CURRENT_BINARY_DIR}
            -DPROJECT_SOURCE_DIR=${CMAKE_CURRENT_SOURCE_DIR}
            -P "${CMAKE_CURRENT_SOURCE_DIR}/tests/verify_install.cmake"
    )
endif()

if(MICROTEST_BUILD_EXAMPLES)
    # Placeholder for future examples. The option exists for package completeness.
endif()
