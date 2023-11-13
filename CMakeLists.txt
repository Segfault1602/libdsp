cmake_minimum_required(VERSION 3.25)
project(libdsp)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED On)

find_program(CLANG_TIDY_EXE NAMES "clang-tidy")
set(CLANG_TIDY_COMMAND "${CLANG_TIDY_EXE}")
set(CMAKE_OSX_DEPLOYMENT_TARGET "13")

set(CLANG_COMPILER_OPTION ${CLANG_COMPILER_OPTION} -Wall -Wpedantic -Werror)
set(MSVC_COMPILER_OPTION /W4 /analyze)
set(GCC_COMPILER_OPTION ${CLANG_COMPILER_OPTION})

include_directories(include)

add_subdirectory(src)

option(LIBDSP_LIB_ONLY "Only build libdsp static library" OFF)
option(LIBDSP_BUILD_TESTS "Build libdsp tests" ON)

if(LIBDSP_BUILD_TESTS)
  add_subdirectory(tests)
  set(gtest_force_shared_crt
      ON
      CACHE BOOL "" FORCE)
  add_subdirectory(externals/googletest)
  endif()

if (NOT LIBDSP_LIB_ONLY OR LIBDSP_BUILD_TESTS)
    set(BUILD_PROGRAMS
    OFF
    CACHE BOOL "Don't build libsndfile programs!")
    set(BUILD_EXAMPLES
    OFF
    CACHE BOOL "Don't build libsndfile examples!")
    set(BUILD_REGTEST
    OFF
    CACHE BOOL "Don't build libsndfile regtest!")
    set(BUILD_TESTING
    OFF
    CACHE BOOL "Don't build libsndfile tests!" FORCE)
  add_subdirectory(externals/libsndfile)
  target_compile_definitions(sndfile PRIVATE _CRT_SECURE_NO_WARNINGS _USE_MATH_DEFINES)
  if(NOT MSVC)
    target_compile_options(sndfile PRIVATE -Wno-deprecated-declarations)
  endif()
endif()

if(NOT LIBDSP_LIB_ONLY)

  set(BUILD_SHARED_LIBS
      OFF
      CACHE BOOL "Build libsndfile shared library" FORCE)
  add_definitions(-D_CRT_SECURE_NO_WARNINGS)

  add_subdirectory(tools)

  set(CMAKE_WARN_DEPRECATED
      OFF
      CACHE BOOL "" FORCE)
  add_subdirectory(externals/rtaudio)
  set(RTMIDI_BUILD_TESTING
      OFF
      CACHE BOOL "" FORCE)
  set(RTMIDI_TARGETNAME_UNINSTALL
      "rtmidi_uninstall"
      CACHE STRING "" FORCE)
  add_subdirectory(externals/rtmidi)

  set(BUILD_TESTING
  OFF
  CACHE BOOL "Don't build libsndfile tests!" FORCE)
  add_subdirectory(externals/libsamplerate)
endif()
