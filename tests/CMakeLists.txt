cmake_minimum_required(VERSION 3.10)

enable_testing()

include(${SP_3RD_PARTY_DIR}/doctest/doctest.cmake)

## --------------------------------------------------------
## unit test 1: basics
## --------------------------------------------------------
add_executable(basics basics.cpp)
target_include_directories(basics PRIVATE ${SP_3RD_PARTY_DIR}/doctest)
doctest_discover_tests(basics)

