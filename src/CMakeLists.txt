set(LIB_SOURCES
    basic_oscillators.cpp
    bow_table.cpp
    dsp_base.cpp
    bowed_string.cpp
    delayline.cpp
    filter.cpp
    interpolation_strategy.cpp
    junction.cpp
    line.cpp
    phaseshapers.cpp
    sinc_resampler.cpp
    waveguide.cpp)

add_library(dsp STATIC ${LIB_SOURCES})

target_include_directories(dsp PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})

if(NOT MSVC)
  target_compile_options(dsp PRIVATE ${CLANG_COMPILER_OPTION})
else()
  target_compile_options(dsp PRIVATE /W4 /WX /analyze)
endif()
