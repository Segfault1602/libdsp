cmake_minimum_required(VERSION 3.25)
project(libdsp)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED On)

include_directories(include)

add_subdirectory(src)
add_subdirectory(tests)
add_subdirectory(externals/googletest)