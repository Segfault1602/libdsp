set(LIB_SOURCES excitation_models.cpp bowed_string.cpp delayline.cpp filter.cpp
                junction.cpp phaseshapers.cpp)

add_library(dsp STATIC ${LIB_SOURCES})

target_include_directories(dsp PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})

if(NOT MSVC)
  target_compile_options(dsp PRIVATE -Wall -Wpedantic -Werror)
else()
  target_compile_options(dsp PRIVATE /W4 /WX /analyze)
endif()
