set(target_name ADC_HAL)

set(${target_name}_src  Adc.cpp
)

set(${target_name}_hpp  ../AdcBase.hpp
                        Adc.hpp
)

add_library(${target_name} STATIC)

cmake_path(GET  CMAKE_CURRENT_SOURCE_DIR    PARENT_PATH     PERIHPERALS_DIR)

target_sources(${target_name}
    PRIVATE
        ${${target_name}_src}
        ${CMAKE_SOURCE_DIR}
    PUBLIC
        FILE_SET HEADERS
        BASE_DIRS ${PERIHPERALS_DIR}
        FILES ${${target_name}_hpp}
)

configure_embedded_target(${target_name})

target_link_libraries(${target_name}    PRIVATE     STM32_HAL)

target_compile_definitions(${target_name}
    PRIVATE
        USE_HAL_DRIVER
)
