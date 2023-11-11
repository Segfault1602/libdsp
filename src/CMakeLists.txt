set(LIB_SOURCES
    basic_oscillators.cpp
    bow_table.cpp
    chorus.cpp
    dsp_base.cpp
    bowed_string.cpp
    delayline.cpp
    filter.cpp
    interpolation_strategy.cpp
    junction.cpp
    line.cpp
    phaseshapers.cpp
    rms.cpp
    sinc_resampler.cpp
    string_ensemble.cpp
    termination.cpp
    waveguide.cpp
    waveguide_gate.cpp)

add_library(dsp STATIC ${LIB_SOURCES})

target_include_directories(dsp PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})

if ("${CMAKE_CXX_COMPILER_ID}" MATCHES "Clang")
  target_compile_options(dsp PRIVATE ${CLANG_COMPILER_OPTION})
elseif("${CMAKE_CXX_COMPILER_ID}" STREQUAL "MSVC")
  target_compile_options(dsp PRIVATE ${MSVC_COMPILER_OPTION})
elseif ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU")
    MESSAGE("GNU")
  target_compile_options(dsp PRIVATE ${GCC_COMPILER_OPTION})
endif()
