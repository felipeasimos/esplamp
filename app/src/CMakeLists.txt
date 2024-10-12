qt_add_executable(esplampapp
    app/main.cpp
)
qt_policy(SET QTP0001 NEW)

qt_add_qml_module(
    esplampapp
    URI esplamp
    VERSION 1.0
    QML_FILES app/main.qml
    SOURCES
        app/RGBController.h
        app/RGBController.cpp
        app/DeviceDiscovery.h
        app/DeviceDiscovery.cpp
    RESOURCES
        assets/images/logo_scaled.png
)

target_link_libraries(esplampapp PRIVATE Qt6::Widgets Qt6::Gui Qt6::Core Qt6::Quick Qt6::Qml Qt6::QuickControls2 Qt6::WidgetsPrivate Qt6::QuickPrivate Qt6::QmlPrivate Qt6::Network)
target_include_directories(esplampapp PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/app)

set_target_properties(esplampapp PROPERTIES
    LINUX ON
    ANDROID ON
)
