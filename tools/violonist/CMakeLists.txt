add_executable(violonist
violonist.cpp
../test_utils.cpp)

target_include_directories(violonist PRIVATE ${PROJECT_SOURCE_DIR}/tools)
target_link_libraries(violonist PRIVATE dsp sndfile)

if(NOT MSVC)
  target_compile_options(violonist PRIVATE ${CLANG_COMPILER_OPTION})
else()
  target_compile_options(violonist PRIVATE ${MSVC_COMPILER_OPTION})
endif()
