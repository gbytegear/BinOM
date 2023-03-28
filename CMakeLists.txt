cmake_minimum_required(VERSION 3.5)

project(BinOM VERSION 0.1 LANGUAGES CXX)

# CXX Compiler config
message("Build type: ${CMAKE_BUILD_TYPE}")
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_FLAGS "-fPIC")
set(CMAKE_CXX_FLAGS_DEBUG_INIT "-g -O0 -fno-inline-small-functions -Wall")
set(CMAKE_CXX_FLAGS_RELEASE_INIT "-O3 -Wall")

# Source files searching
file(GLOB_RECURSE SOURCE_LIB RELATIVE ${CMAKE_SOURCE_DIR} "libbinom/source/*.cxx")
message("Library sources
${SOURCE_LIB}")
file(GLOB_RECURSE SOURCE_TEST RELATIVE ${CMAKE_SOURCE_DIR} "test/*.cxx")
message("Test sources
${SOURCE_TEST}")

add_library(binom STATIC ${SOURCE_LIB})
target_include_directories(binom PRIVATE RELATIVE ${CMAKE_SOURCE_DIR} ".")
target_include_directories(binom PRIVATE RELATIVE ${CMAKE_SOURCE_DIR} "libbinom/include")

add_executable(test ${SOURCE_TEST})
target_include_directories(test PRIVATE RELATIVE ${CMAKE_SOURCE_DIR} ".")
target_include_directories(test PRIVATE RELATIVE ${CMAKE_SOURCE_DIR} "libbinom/include")
target_link_libraries(test binom)
