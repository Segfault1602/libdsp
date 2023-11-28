
FetchContent_Declare(
    libsamplerate
    GIT_REPOSITORY https://github.com/libsndfile/libsamplerate.git
    GIT_TAG 0.2.2
    FIND_PACKAGE_ARGS NAMES libsamplerate
    )
FetchContent_MakeAvailable(libsamplerate)

add_executable(resampler resampler.cpp)
target_include_directories(resampler PRIVATE externals/libsamplerate/include)
target_link_libraries(resampler PUBLIC tool_common dsp sndfile samplerate)

if(NOT MSVC)
  target_compile_options(resampler PRIVATE -Wall -Wpedantic -Werror)
endif()
