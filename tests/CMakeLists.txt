set(BINARY ${CMAKE_PROJECT_NAME}_tests.out)

set(CMAKE_BUILD_TYPE Debug)

set(TEST_SOURCES
    main.cpp
    circular_buffer_tests.cpp
    sinc_resampler_tests.cpp)

set(SOURCES ${TEST_SOURCES})

add_executable(${BINARY} ${TEST_SOURCES})

add_test(NAME ${BINARY} COMMAND ${BINARY})

target_link_libraries(${BINARY} PUBLIC libdsp gtest)