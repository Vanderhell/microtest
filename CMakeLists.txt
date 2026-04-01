cmake_minimum_required(VERSION 3.16)
project(microtest VERSION 1.0.0 LANGUAGES C)

include(GNUInstallDirs)

set(CMAKE_C_STANDARD 99)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS OFF)

option(MICROTEST_BUILD_TESTS "Build microtest self-tests" ON)

add_library(microtest INTERFACE)
target_include_directories(microtest INTERFACE
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
)

if(MICROTEST_BUILD_TESTS)
    enable_testing()
    add_executable(test_microtest tests/test_all.c)
    target_link_libraries(test_microtest PRIVATE microtest)
    if(NOT MSVC)
        target_link_libraries(test_microtest PRIVATE m)
    endif()
    add_test(NAME microtest_selftest COMMAND test_microtest)
endif()

install(FILES include/mtest.h DESTINATION ${CMAKE_INSTALL_INCLUDEDIR})
