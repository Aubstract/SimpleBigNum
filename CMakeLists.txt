cmake_minimum_required(VERSION 3.24)
project(SimpleBigNum)

set(CMAKE_CXX_STANDARD 17)
include_directories(src)

set(SOURCE_FILES
    src/main.cpp
    src/UIntX.h)

add_executable(main ${SOURCE_FILES})

include(FetchContent)
FetchContent_Declare(
        googletest
        URL https://github.com/google/googletest/archive/refs/heads/main.zip
)
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)

FetchContent_MakeAvailable(googletest)

set(TEST_FILES
        src/UIntX.h
        tst/UIntXTest.cpp)


add_executable(tst ${TEST_FILES})

target_link_libraries(tst gtest gtest_main)

include(GoogleTest)
gtest_discover_tests(tst)