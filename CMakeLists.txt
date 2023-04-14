cmake_minimum_required(VERSION 3.25)
project(libdsp)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED On)

find_program(CLANG_TIDY_EXE NAMES "clang-tidy")
set(CLANG_TIDY_COMMAND "${CLANG_TIDY_EXE}")

include_directories(include)

add_subdirectory(src)

option(LIBDSP_LIB_ONLY "Only build libdsp static library" OFF)

if (NOT LIBDSP_LIB_ONLY)

set(BUILD_SHARED_LIBS OFF CACHE BOOL "Build libsndfile shared library" FORCE)
add_definitions(-D_CRT_SECURE_NO_WARNINGS)

add_subdirectory(tests)
add_subdirectory(tools)

set(CMAKE_WARN_DEPRECATED OFF CACHE BOOL "" FORCE)
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
add_subdirectory(externals/rtaudio)
add_subdirectory(externals/googletest)
add_subdirectory(externals/libsamplerate)

# Link to libsndfile
set(BUILD_PROGRAMS OFF CACHE BOOL "Don't build libsndfile programs!")
set(BUILD_EXAMPLES OFF CACHE BOOL "Don't build libsndfile examples!")
set(BUILD_REGTEST OFF CACHE BOOL "Don't build libsndfile regtest!")
set(BUILD_TESTING OFF CACHE BOOL "Don't build libsndfile tests!" FORCE)


add_subdirectory(externals/libsndfile)
target_compile_definitions(sndfile PRIVATE _CRT_SECURE_NO_WARNINGS _USE_MATH_DEFINES)
target_compile_options(sndfile PRIVATE -Wno-deprecated-declarations)

endif()