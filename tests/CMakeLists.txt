set(TEST_SOURCES
    main_tests.cpp
    circular_buffer_tests.cpp
    delayline_tests.cpp
    sinc_resampler_tests.cpp
    waveguide_tests.cpp)

set(SOURCES ${TEST_SOURCES})

add_executable(libdsp_tests ${TEST_SOURCES})

if(NOT MSVC)
  target_compile_options(dsp PRIVATE -Wall -Wpedantic -Werror)
else()
  target_compile_options(dsp PRIVATE /W4 /WX)
endif()

add_test(NAME libdsp_tests COMMAND libdsp_tests)

target_link_libraries(libdsp_tests PUBLIC dsp PRIVATE GTest::gtest_main GTest::gmock_main)
