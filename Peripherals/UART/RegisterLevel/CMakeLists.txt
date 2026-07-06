set(target_name UART_RegisterLevel)

#set(${target_name}_src)


set(${target_name}_hpp  ../IUart.hpp
                        ../LineParser.hpp
                        ../Utils/RingBuffer.hpp
                        Uart.hpp
                        ../../Config.hpp
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
