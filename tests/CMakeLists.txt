set(TEST_SOURCES
    main_tests.cpp
    circular_buffer_tests.cpp
    sinc_resampler_tests.cpp)

set(SOURCES ${TEST_SOURCES})

add_executable(libdsp_tests ${TEST_SOURCES})

add_test(NAME libdsp_tests COMMAND libdsp_tests)

target_link_libraries(libdsp_tests PUBLIC libdsp gtest)

# Player executable
add_executable(dsp_player
    player.cpp
    test_utils.cpp)
target_link_libraries(dsp_player PUBLIC libdsp sndfile rtaudio)