add_executable(player player.cpp gamepad_xinput.cpp)
# target_include_directories(player PUBLIC dsp rtaudio)
target_link_libraries(player PUBLIC dsp rtaudio XInput)

set_target_properties(player PROPERTIES CXX_CLANG_TIDY "${CLANG_TIDY_COMMAND}")
target_compile_options(player PRIVATE -Wall -Wpedantic -Werror)