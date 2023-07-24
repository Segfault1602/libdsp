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

set_target_properties(player PROPERTIES CXX_CLANG_TIDY "${CLANG_TIDY_COMMAND}")

if(NOT MSVC)
  target_compile_options(player PRIVATE -Wall -Wpedantic -Werror)
else()
  target_compile_options(dsp PRIVATE /W4 /WX)
endif()
