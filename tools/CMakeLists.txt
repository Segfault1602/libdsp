FetchContent_Declare(
    rtmidi
    GIT_REPOSITORY https://github.com/thestk/rtmidi.git
    GIT_TAG 6.0.0
    )

FetchContent_Declare(
    rtaudio
    GIT_REPOSITORY https://github.com/thestk/rtaudio.git
    GIT_TAG 6.0.1
    )

set(RTMIDI_TARGETNAME_UNINSTALL "rtmidi_uninstall" CACHE STRING "" FORCE)
set(RTAUDIO_TARGETNAME_UNINSTALL "rtaudio_uninstall" CACHE STRING "" FORCE)
FetchContent_MakeAvailable(rtmidi rtaudio)

target_compile_definitions(rtaudio PRIVATE -D_CRT_SECURE_NO_WARNINGS)

add_subdirectory(player)
add_subdirectory(violonist)
add_subdirectory(audio_test)
add_subdirectory(resampler)
add_subdirectory(common)