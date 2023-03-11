add_library(libdsp STATIC
phaseshapers.cpp
sinc_resampler.cpp
)

target_include_directories(libdsp PRIVATE
${CMAKE_CURRENT_SOURCE_DIR}
)