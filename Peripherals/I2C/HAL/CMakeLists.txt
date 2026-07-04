set(target_name I2C_HAL)

set(${target_name}_src  I2C.cpp
                        I2C_IT.cpp
)

set(${target_name}_hpp  ../I2CBase.hpp
                        I2C.hpp
                        I2C_IT.hpp
)

add_library(${target_name} STATIC)

cmake_path(GET  CMAKE_CURRENT_SOURCE_DIR    PARENT_PATH     PERIHPERALS_DIR)

configure_embedded_target(${target_name})
configure_HAL_target(${target_name})

target_sources(${target_name}
    PRIVATE
        ${${target_name}_src}
        ${CMAKE_SOURCE_DIR}
    PUBLIC
        FILE_SET HEADERS
        BASE_DIRS ${PERIHPERALS_DIR}
        FILES ${${target_name}_hpp}
)
