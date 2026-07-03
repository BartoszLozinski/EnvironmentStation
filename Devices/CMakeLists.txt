set(target_name Devices)

set(${target_name}_src  LPS25HBBase.cpp
                        LPS25HB.cpp
                        LPS25HB_Async.cpp)

set(${target_name}_hpp  LPS25HBBase.hpp
                        LPS25HB.hpp
                        LPS25HB_Async.hpp
                        Measurement.hpp)

add_library(${target_name}  STATIC)

cmake_path(GET  CMAKE_CURRENT_SOURCE_DIR    PARENT_PATH     DEVICES_DIR)

if(NOT COMMAND configure_embedded_target)
    include("${CMAKE_CURRENT_LIST_DIR}/../cmake/configuration.cmake")
endif()

configure_embedded_target(${target_name})

target_sources(${target_name}
    PRIVATE
        ${${target_name}_src}
    PUBLIC
        FILE_SET HEADERS
        BASE_DIRS ${DEVICES_DIR}
        FILES   ${${target_name}_hpp}
)

