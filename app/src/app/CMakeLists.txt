qt_add_executable(esplamp
    main.cpp
)

target_link_libraries(esplamp PRIVATE Qt6::Widgets)

set_target_properties(esplamp PROPERTIES
    LINUX ON
)
