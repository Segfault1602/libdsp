FetchContent_Declare(
    googletest
    GIT_REPOSITORY https://github.com/google/googletest.git
    # Using a branch name isn't usually recommended but googletest uses a 'stable at main' workfow
    GIT_TAG main
    FIND_PACKAGE_ARGS NAMES GTest
    )
    # For Windows: Prevent overriding the parent project's compiler/linker settings
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)
include(GoogleTest)

add_subdirectory(perf)

set(TEST_SOURCES
    main_tests.cpp
    basic_oscillators_tests.cpp
    circular_buffer_tests.cpp
    delayline_tests.cpp
    rms_tests.cpp
    sinc_resampler_tests.cpp
    test_utils.cpp
    waveguide_tests.cpp
    waveguide_gates_tests.cpp)

set(SOURCES ${TEST_SOURCES})

add_executable(libdsp_tests ${TEST_SOURCES})
target_link_libraries(libdsp_tests PUBLIC dsp PRIVATE sndfile GTest::gtest_main GTest::gmock_main)

if ("${CMAKE_CXX_COMPILER_ID}" MATCHES "Clang")
  target_compile_options(libdsp_tests PRIVATE ${CLANG_COMPILER_OPTION})
elseif("${CMAKE_CXX_COMPILER_ID}" STREQUAL "MSVC")
  target_compile_options(libdsp_tests PRIVATE ${MSVC_COMPILER_OPTION})
elseif ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU")
  target_compile_options(libdsp_tests PRIVATE ${GCC_COMPILER_OPTION})
endif()

gtest_discover_tests(libdsp_tests WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/.. XML_OUTPUT_DIR ${PROJECT_SOURCE_DIR}/test_results)
