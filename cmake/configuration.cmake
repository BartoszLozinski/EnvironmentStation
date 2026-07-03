function(configure_embedded_target target)


    target_compile_options(${target}
        PRIVATE
            -Wall
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

    target_compile_definitions(${target}
        PUBLIC 
            STM32L476xx
    )

    set_target_properties(${target}
        PROPERTIES
            C_STANDARD 11
            C_STANDARD_REQUIRED ON
            CXX_STANDARD 20
            CXX_STANDARD_REQUIRED ON
    )


endfunction()