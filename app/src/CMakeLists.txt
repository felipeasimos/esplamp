qt_add_executable(esplamp
    app/main.cpp
)

target_link_libraries(esplamp PRIVATE Qt6::Gui Qt6::Widgets)

set_target_properties(esplamp PROPERTIES
    LINUX ON
    ANDROID ON
)

qt_add_qml_module(
    esplamp
    URI esplampapp
    VERSION 1.0
    QML_FILES app/main.qml
    RESOURCES
        ./assets/images/logo_scaled.png
)
