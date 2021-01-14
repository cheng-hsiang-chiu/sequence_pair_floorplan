# CMake version
cmake_minimum_required (VERSION 3.10)
MESSAGE(STATUS "CMAKE_ROOT: " ${CMAKE_ROOT})

# Project name
project(SequencePair_floorplan VERSION 1.0.0 LANGUAGES CXX)

# build options
option(SP_BUILD_TESTS "Enables builds of tests" ON)
option(SP_BUILD_MAIN "Enables builds of main programs" ON)


set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)


# project-specific variables
set(SP_INC_INSTALL_DIR "${CMAKE_INSTALL_PREFIX}/include")
set(SP_LIB_INSTALL_DIR "${CMAKE_INSTALL_PREFIX}/lib")
set(SP_UTEST_DIR ${PROJECT_SOURCE_DIR}/unittests)
set(SP_3RD_PARTY_DIR ${PROJECT_SOURCE_DIR}/3rd-party)
set(SP_SRC_DIR ${PROJECT_SOURCE_DIR}/src)
set(SP_MAIN_DIR ${PROJECT_SOURCE_DIR}/main)
set(SP_DEFAULT_BUILD_TYPE "Release")
set(SP_CIRCUIT_DIR ${PROJECT_SOURCE_DIR}/circuits)
set(SP_LIB_DIR ${PROJECT_SOURCE_DIR}/lib)

# Turn on the verbose
set(CMAKE_VERBOSE_MAKEFILE ON)

# error setting
add_library(error_settings INTERFACE)

target_compile_options(
  error_settings
  INTERFACE
  $<$<CXX_COMPILER_ID:AppleClang>:-Wall -Wextra -Wfatal-errors>
  $<$<AND:$<COMPILE_LANGUAGE:CXX>,$<CXX_COMPILER_ID:Clang>>:-Wall -Wextra -Wfatal-errors>
  $<$<AND:$<COMPILE_LANGUAGE:CXX>,$<CXX_COMPILER_ID:GNU>>:-Wall -Wextra -Wfatal-errors>
  $<$<AND:$<COMPILE_LANGUAGE:CXX>,$<CXX_COMPILER_ID:MSVC>>:/W3 /permissive->
  #$<$<COMPILE_LANGUAGE:CUDA>:-Xcompiler=-Wall,-Wextra,-Wfatal-errors>
  #$<$<CXX_COMPILER_ID:Clang>:-Wall -Wextra -Wfatal-errors>
  #$<$<CXX_COMPILER_ID:GNU>:-Wall -Wextra -Wfatal-errors>
  #$<$<CXX_COMPILER_ID:MSVC>:/W3 /permissive->
  #$<$<CXX_COMPILER_ID:NVIDIA>:-Xcompiler=-Wall,-Wextra,-Wfatal-errors>
)


# -----------------------------------------------------------------------------
# Floorplan Libraries
# -----------------------------------------------------------------------------
add_subdirectory(${SP_SRC_DIR})
add_subdirectory(${SP_LIB_DIR})

# -----------------------------------------------------------------------------
# main floorplan programs
# -----------------------------------------------------------------------------
if(SP_BUILD_MAIN)
  add_subdirectory(${SP_MAIN_DIR})
endif(SP_BUILD_MAIN)

# -----------------------------------------------------------------------------
# Unittest
# -----------------------------------------------------------------------------

include(CTest)

if(SP_BUILD_TESTS)
  add_subdirectory(tests)
endif(SP_BUILD_TESTS)


