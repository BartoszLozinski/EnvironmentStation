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

configure_embedded_target(${target_name}
    HEADER_ONLY True
)

configure_HAL_target(${target_name}
    HEADER_ONLY True
)
