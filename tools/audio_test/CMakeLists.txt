# effect player executable
add_executable(effect_player
    arpegiator.cpp
    bowedstring_tests.cpp
    effect_player.cpp
    dsp_tester.cpp
    phaseshapers_tests.cpp
    stringensemble_tests.cpp
    waveguide_tests.cpp)

target_link_libraries(effect_player PUBLIC tool_common dsp sndfile rtaudio)

if(NOT MSVC)
  target_compile_options(effect_player PRIVATE ${CLANG_COMPILER_OPTION})
endif()
