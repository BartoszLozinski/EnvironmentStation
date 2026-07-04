set(target_name Timer)

set(${target_name}_src  SoftwareTimer.cpp)

set(${target_name}_hpp  ../TimerBase.hpp
                        SoftwareTimer.hpp
)

file(READ
    "${CMAKE_SOURCE_DIR}/Core/Inc/stm32l4xx_hal_conf.h"
    HAL_CONF
)

# Check if the define exists uncommented (not inside /* */)
if("${HAL_CONF}" MATCHES "#define[ \t]+HAL_TIM_MODULE_ENABLED[ \t]*\n" OR "${HAL_CONF}" MATCHES "#define[ \t]+HAL_TIM_MODULE_ENABLED[ \t]*$")
    set(HAL_TIM_FOUND TRUE)
else()
    set(HAL_TIM_FOUND FALSE)
endif()

if (HAL_TIM_FOUND)

    list(APPEND ${target_name}_src
        InputCapture.cpp
        InputCaptureIT.cpp
        Pwm.cpp
    )

    list(APPEND ${target_name}_hpp
        ../InputCaptureInterface.hpp
        InputCapture.hpp
        InputCaptureIT.hpp
        ../IPwm.hpp
        Pwm.hpp
    )

    message(STATUS "TIM HAL module enabled")

else()

    message(STATUS "TIM HAL module not enabled")

endif()

#####################################################################
# End of Check

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
configure_HAL_target(${target_name})

set_target_properties(${target_name}   PROPERTIES  CXX_STANDARD 20     CXX_STANDARD_REQUIRED ON)
