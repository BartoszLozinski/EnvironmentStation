set(target_name Scheduler)

set(${target_name}_hpp  Task.hpp
                        Scheduler.hpp
)


configure_library(${target_name}
    HEADER_ONLY True
    HEADERS ${${target_name}_hpp}
)

configure_embedded_target(${target_name}
    HEADER_ONLY True
)
