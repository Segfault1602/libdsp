cmake_minimum_required(VERSION 3.25)
project(libdsp)

include(FetchContent)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED On)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON CACHE INTERNAL "")

find_program(CLANG_TIDY_EXE NAMES "clang-tidy")
set(CLANG_TIDY_COMMAND "${CLANG_TIDY_EXE}" "-config-file=${CMAKE_CURRENT_SOURCE_DIR}/.clang-tidy")
set(CMAKE_OSX_DEPLOYMENT_TARGET "13")

set(CLANG_COMPILER_OPTION ${CLANG_COMPILER_OPTION} -Wall -Wpedantic -Werror -fsave-optimization-record)
set(MSVC_COMPILER_OPTION /W4 /analyze)
set(GCC_COMPILER_OPTION -Wall -Wpedantic)

include_directories(include)

add_subdirectory(src)

option(LIBDSP_LIB_ONLY "Only build libdsp static library" OFF)
option(LIBDSP_BUILD_TESTS "Build libdsp tests" ON)

if(LIBDSP_BUILD_TESTS)
    enable_testing()
    add_subdirectory(tests)
endif()

if (NOT LIBDSP_LIB_ONLY OR LIBDSP_BUILD_TESTS)
    FetchContent_Declare(
        libsndfile
        GIT_REPOSITORY https://github.com/libsndfile/libsndfile.git
        GIT_TAG 1.2.2
        )
    set(BUILD_PROGRAMS OFF CACHE BOOL "Don't build libsndfile programs!")
    set(BUILD_EXAMPLES OFF CACHE BOOL "Don't build libsndfile examples!")
    set(BUILD_REGTEST OFF CACHE BOOL "Don't build libsndfile regtest!")
    set(BUILD_TESTING OFF CACHE BOOL "Don't build libsndfile tests!" FORCE)
    set(BUILD_PROGRAMS OFF CACHE BOOL "Don't build libsndfile programs!" FORCE)
    set(ENABLE_EXTERNAL_LIBS OFF CACHE BOOL "Disable external libs support!" FORCE)
    set(BUILD_TESTING OFF CACHE BOOL "Disable libsndfile tests!" FORCE)
    FetchContent_MakeAvailable(libsndfile)
    target_compile_definitions(sndfile PRIVATE _CRT_SECURE_NO_WARNINGS _USE_MATH_DEFINES)
    if(NOT MSVC)
        target_compile_options(sndfile PRIVATE -Wno-deprecated-declarations)
    endif()
endif()

if(NOT LIBDSP_LIB_ONLY)
    add_subdirectory(tools)
endif()
