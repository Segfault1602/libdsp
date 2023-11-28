add_library(tool_common STATIC tool_utils.cpp)
target_link_libraries(tool_common PUBLIC sndfile)

target_include_directories(tool_common PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
