add_executable(resampler resampler.cpp test_utils.cpp)
target_include_directories(resampler PRIVATE externals/libsamplerate/include)
target_link_libraries(resampler PUBLIC dsp sndfile samplerate)

if(NOT MSVC)
  target_compile_options(resampler PRIVATE -Wall -Wpedantic -Werror)
endif()

# effect player executable
add_executable(effect_player
    arpegiator.cpp
    bowedstring_tests.cpp
    effect_player.cpp
    dsp_tester.cpp
    phaseshapers_tests.cpp
    stringensemble_tests.cpp
    test_utils.cpp
    waveguide_tests.cpp)

target_link_libraries(effect_player PUBLIC dsp sndfile rtaudio)

if(NOT MSVC)
  target_compile_options(effect_player PRIVATE ${CLANG_COMPILER_OPTION})
endif()

add_subdirectory(player)
add_subdirectory(violonist)
