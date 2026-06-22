cmake_minimum_required(VERSION 3.16)
project(microtest_consumer C)

set(CMAKE_C_STANDARD 99)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS OFF)

find_package(microtest CONFIG REQUIRED)

add_executable(consumer main.c)
target_link_libraries(consumer PRIVATE microtest::microtest)
