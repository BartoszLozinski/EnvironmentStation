function(configure_embedded_target target)

    set(options HEADER_ONLY)
    cmake_parse_arguments(ARG "${options}" "" "" ${ARGN})

    if (ARG_HEADER_ONLY)
        set(ACCESS_TYPE INTERFACE)
    else()
        set(ACCESS_TYPE PRIVATE)
    endif()

    target_compile_options(${target}
        ${ACCESS_TYPE}
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
        ${ACCESS_TYPE}
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


function(configure_HAL_target target)


    set(options HEADER_ONLY)
    cmake_parse_arguments(ARG "${options}" "" "" ${ARGN})

    if (ARG_HEADER_ONLY)
        set(ACCESS_TYPE INTERFACE)
    else()
        set(ACCESS_TYPE PRIVATE)
    endif()

    target_link_libraries(${target} ${ACCESS_TYPE} STM32_HAL)

    target_compile_definitions(${target}
        ${ACCESS_TYPE}
            USE_HAL_DRIVER
    )


endfunction()


function(configure_library target)


    set(options HEADER_ONLY)
    set(multiValueArgs SOURCES HEADERS)
    cmake_parse_arguments(ARG "${options}" "" "${multiValueArgs}" ${ARGN})

    cmake_path(GET CMAKE_CURRENT_SOURCE_DIR PARENT_PATH parent_dir)

    if(ARG_HEADER_ONLY)
        add_library(${target} INTERFACE)
        target_sources(${target}
            INTERFACE
                FILE_SET HEADERS
                BASE_DIRS ${parent_dir}
                FILES ${ARG_HEADERS}
        )
    else()
        add_library(${target} STATIC)
        target_sources(${target}
            PRIVATE
                ${ARG_SOURCES}
            PUBLIC
                FILE_SET HEADERS
                BASE_DIRS ${parent_dir}
                FILES ${ARG_HEADERS}
        )
    endif()


endfunction()