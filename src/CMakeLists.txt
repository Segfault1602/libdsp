set(LIB_SOURCES
    dsp_base.cpp
    bow_table.cpp
    bowed_string.cpp
    delayline.cpp
    filter.cpp
    junction.cpp
    line.cpp
    phaseshapers.cpp)

add_library(dsp STATIC ${LIB_SOURCES})

target_include_directories(dsp PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})

if(NOT MSVC)
  target_compile_options(dsp PRIVATE ${CLANG_COMPILER_OPTION})
else()
  target_compile_options(dsp PRIVATE /W4 /WX /analyze)
endif()
