add_library(dsp STATIC phaseshapers.cpp)

    target_include_directories(dsp PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})

        target_compile_options(dsp PRIVATE - Wall - Wpedantic - Werror)