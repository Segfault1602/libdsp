add_executable(resampler resampler.cpp)
target_include_directories(resampler PRIVATE externals/libsamplerate/include)
target_link_libraries(resampler PUBLIC tool_common dsp sndfile samplerate)

if(NOT MSVC)
  target_compile_options(resampler PRIVATE -Wall -Wpedantic -Werror)
endif()
