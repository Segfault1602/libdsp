# the name of the target operating system
set(CMAKE_SYSTEM_NAME               Generic)
set(CMAKE_SYSTEM_PROCESSOR          arm)

# Without that flag CMake is not able to pass test compilation check
set(CMAKE_TRY_COMPILE_TARGET_TYPE   STATIC_LIBRARY)

# which compilers to use for C and C++
set(CMAKE_AR                        arm-none-eabi-ar)
set(CMAKE_ASM_COMPILER              arm-none-eabi-gcc)
set(CMAKE_C_COMPILER                arm-none-eabi-gcc)
set(CMAKE_CXX_COMPILER              arm-none-eabi-g++)
set(CMAKE_LINKER                    arm-none-eabi-ld)
set(CMAKE_OBJCOPY                   arm-none-eabi-objcopy CACHE INTERNAL "")
set(CMAKE_RANLIB                    arm-none-eabi-ranlib CACHE INTERNAL "")
set(CMAKE_SIZE                      arm-none-eabi-size CACHE INTERNAL "")
set(CMAKE_STRIP                     arm-none-eabi-strip CACHE INTERNAL "")


# # where is the target environment located
# set(CMAKE_FIND_ROOT_PATH  /usr/i586-mingw32msvc
#     /home/alex/mingw-install)

# # adjust the default behavior of the FIND_XXX() commands:
# # search programs in the host environment
# set(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER)

# search headers and libraries in the target environment
set(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -mcpu=cortex-m7 -mthumb -mfpu=fpv5-d16 -mfloat-abi=hard -O3 -Wall -Werror -fdata-sections -ffunction-sections")