cmake_minimum_required(VERSION 3.21.1)

project(esplampapp VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
# set(CMAKE_BUILD_TYPE Release)

find_package(Qt6 6.5 COMPONENTS Widgets Gui Core Quick Qml QuickControls2 REQUIRED)
qt_standard_project_setup()
add_subdirectory(src)

### custom commands
set(MAIN_ACTIVITY "org.qtproject.example.esplamp/org.qtproject.qt.android.bindings.QtActivity")
set(APK_PATH "${CMAKE_BINARY_DIR}/src/app/android-build/esplamp.apk")
set(ANDROID_PACKAGE "org.qtproject.example.esplamp")
add_custom_command(
    OUTPUT launch_apk_command
    COMMAND adb shell am start -S -n ${MAIN_ACTIVITY}
    COMMMENT "Launching APK on device"
)
add_custom_command(
    OUTPUT clear_app_data
    COMMAND adb shell pm clear ${ANDROID_PACKAGE}
)
add_custom_target(
    launch_apk
    DEPENDS launch_apk_command
    COMMENT "launch the APK on device"
)
# add_dependencies(launch_apk apk)
