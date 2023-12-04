FetchContent_Declare(
    nanobench
    GIT_REPOSITORY https://github.com/martinus/nanobench.git
    GIT_TAG v4.1.0
    GIT_SHALLOW TRUE)

FetchContent_Declare(
    doctest
    GIT_REPOSITORY https://github.com/doctest/doctest.git
    GIT_TAG v2.4.11
    GIT_SHALLOW TRUE)

FetchContent_MakeAvailable(nanobench doctest)

add_executable(perf_tests
    perf_tests.cpp
    basicosc_perf.cpp
    phaseshaper_perf.cpp)
target_include_directories(perf_tests PRIVATE ${doctest_SOURCE_DIR}/doctest)
target_link_libraries(perf_tests PRIVATE nanobench dsp doctest)