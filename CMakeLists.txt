cmake_minimum_required(VERSION 3.15)
project(MergeSortLab)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

enable_testing()

add_executable(lab6
    prog/main.cpp
    prog/stats.h
    prog/mergesort_topdown.h
    prog/mergesort_bottomup.h
    prog/mergesort_optimized.h
    prog/mergesort_kway10.h
    prog/mergesort_kway10_optimized.h
)

include(FetchContent)
FetchContent_Declare(
    googletest
    URL https://github.com/google/googletest/archive/refs/tags/v1.14.0.zip
)
FetchContent_MakeAvailable(googletest)

add_executable(tests
    tests/test.cpp
)

target_link_libraries(tests
    PRIVATE
        gtest
        gtest_main
)

add_test(NAME AllTests COMMAND tests)
