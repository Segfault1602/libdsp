add_library(dsp STATIC
phaseshapers.cpp
)

target_include_directories(dsp PRIVATE
${CMAKE_CURRENT_SOURCE_DIR}
)