set(target_name STM32_HAL)

add_library(${target_name}   STATIC)

file(GLOB HAL_SOURCES
    STM32L4xx_HAL_Driver/Src/*.c
)

target_sources(${target_name}
    PRIVATE
        ${HAL_SOURCES})

configure_embedded_target(${target_name})

target_include_directories(${target_name}
    PRIVATE
        STM32L4xx_HAL_Driver/Legacy
    PUBLIC
        ${CMAKE_SOURCE_DIR}/Core/Inc
        CMSIS/Device/ST/STM32L4xx/Include
        CMSIS/Include
        STM32L4xx_HAL_Driver/Inc
)

target_compile_definitions(${target_name}
    PRIVATE
        USE_HAL_DRIVER
)
