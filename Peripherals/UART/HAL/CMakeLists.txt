set(target_name UART_HAL)

set(${target_name}_hpp  ../IUart.hpp
                        ../LineParser.hpp
                        ../Utils/RingBuffer.hpp
                        Uart.hpp
                        UartIT.hpp
)

configure_library(${target_name}
    HEADER_ONLY True
    HEADERS ${${target_name}_hpp}
)

target_link_libraries(${target_name}    INTERFACE     STM32_HAL)
target_compile_options(${target_name}   INTERFACE     -Wall
                                                    -Werror
                                                    -Wextra
                                                    -Wpedantic
                                                    -fverbose-asm # additional comments for generated assembler code

                                                    $<$<COMPILE_LANGUAGE:C>:-Wstrict-prototypes>

                                                    $<$<COMPILE_LANGUAGE:CXX>:-fno-rtti> # disable virtual class information used by dynamic_cast and typeid
                                                    $<$<COMPILE_LANGUAGE:CXX>:-fno-exceptions> # disable exceptions handling

                                                    $<$<CONFIG:DEBUG>: -Og -g>
                                                    $<$<CONFIG:RELEASE>: -O3>
)

target_compile_definitions(${target_name}
    INTERFACE
        STM32L476xx
        USE_HAL_DRIVER
)

set_target_properties(${target_name}   PROPERTIES  CXX_STANDARD 20     CXX_STANDARD_REQUIRED ON)
