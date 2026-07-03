cmake_minimum_required(VERSION 3.28)
project(UnitTests)

include(${CMAKE_SOURCE_DIR}/../cmake/configuration.cmake)

find_package(GTest REQUIRED)
set(CMAKE_CXX_COMPILER  clang++-18)
set(CMAKE_CXX_EXTENSIONS    OFF)

set(GMOCK_LIBRARIES     /usr/lib/libgmock.a
                        /usr/lib/libgmock_main.a
)

add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/../Devices ${CMAKE_CURRENT_BINARY_DIR}/Devices)

add_executable(${PROJECT_NAME}  mainTests.cpp)

target_link_libraries(${PROJECT_NAME}   PRIVATE     ${GTEST_LIBRARIES}
                                                    ${GMOCK_LIBRARIES}
                                                    Devices
)

target_compile_options(${PROJECT_NAME}  PRIVATE     -Wall
                                                    -Werror
                                                    -Wpedantic
                                                    -Wextra
                                                    $<$<CONFIG:DEBUG>: -Og -g>
                                                    $<$<CONFIG:RELEASE>: -O3>
)

set_target_properties(${PROJECT_NAME}   PROPERTIES  CXX_STANDARD 20     CXX_STANDARD_REQUIRED ON)
