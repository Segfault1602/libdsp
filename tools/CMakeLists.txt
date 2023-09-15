add_executable(resampler resampler.cpp)
target_include_directories(resampler PRIVATE externals/libsamplerate/include)
target_link_libraries(resampler PUBLIC dsp sndfile samplerate)

set_target_properties(resampler PROPERTIES CXX_CLANG_TIDY
                                           "${CLANG_TIDY_COMMAND}")
if(NOT MSVC)
  target_compile_options(resampler PRIVATE -Wall -Wpedantic -Werror)
endif()

# effect player executable
add_executable(effect_player effect_player.cpp dsp_tester.cpp)
target_link_libraries(effect_player PUBLIC dsp sndfile rtaudio)

set_target_properties(effect_player PROPERTIES CXX_CLANG_TIDY
                                               "${CLANG_TIDY_COMMAND}")
if(NOT MSVC)
  target_compile_options(effect_player PRIVATE ${CLANG_COMPILER_OPTION})
endif()

add_subdirectory(player)
