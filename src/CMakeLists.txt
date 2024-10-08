set(LIB_SOURCES
    basic_oscillators.cpp
    bow_table.cpp
    buchla_lpg.cpp
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
    smooth_param.cpp
    string_ensemble.cpp
    termination.cpp
    vector_phaseshaper.cpp
    waveguide.cpp
    waveguide_gate.cpp)

add_library(dsp STATIC ${LIB_SOURCES})

if (CLANG_TIDY_EXE AND NOT LIBDSP_DISABLE_CLANG_TIDY)
    message(STATUS "Enabling clang-tidy for libdsp")
    set_target_properties(dsp PROPERTIES CXX_CLANG_TIDY "${CLANG_TIDY_COMMAND}")
endif()

target_include_directories(dsp PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})

target_compile_options(dsp PRIVATE -g)

target_link_options(dsp PRIVATE -fuse-ld=lld -g -Wl,--pdb=)
if ("${CMAKE_CXX_COMPILER_ID}" MATCHES "Clang")
  target_compile_options(dsp PRIVATE ${CLANG_COMPILER_OPTION})
elseif("${CMAKE_CXX_COMPILER_ID}" STREQUAL "MSVC")
  target_compile_options(dsp PRIVATE ${MSVC_COMPILER_OPTION})
elseif ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU")
  target_compile_options(dsp PRIVATE ${GCC_COMPILER_OPTION})
endif()
