add_executable(resampler resampler.cpp)
target_include_directories(resampler PRIVATE externals/libsamplerate/include)
target_link_libraries(resampler PUBLIC dsp sndfile samplerate)

set_target_properties(resampler PROPERTIES CXX_CLANG_TIDY "${CLANG_TIDY_COMMAND}")
target_compile_options(resampler PRIVATE -Wall -Wpedantic -Werror)

# Player executable
add_executable(dsp_player
    player.cpp)
target_link_libraries(dsp_player PUBLIC dsp sndfile rtaudio)

set_target_properties(dsp_player PROPERTIES CXX_CLANG_TIDY "${CLANG_TIDY_COMMAND}")
target_compile_options(dsp_player PRIVATE -Wall -Wpedantic -Werror)
