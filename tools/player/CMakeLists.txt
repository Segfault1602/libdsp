set(PLAYER_SRC player.cpp midi_controller.cpp)

set(PLAYER_LIBS dsp rtaudio rtmidi sndfile)

if(WIN32)
  set(PLAYER_SRC ${PLAYER_SRC} gamepad_xinput.cpp)
  set(PLAYER_LIBS ${PLAYER_LIBS} XInput)
else()
  set(PLAYER_SRC ${PLAYER_SRC} gamepad_dummy.cpp)
endif()

add_executable(player ${PLAYER_SRC})
target_link_libraries(player PUBLIC ${PLAYER_LIBS})

if(NOT MSVC)
  target_compile_options(player PRIVATE ${CLANG_COMPILER_OPTION})
else()
  target_compile_options(dsp PRIVATE ${MSVC_COMPILER_OPTION})
endif()
