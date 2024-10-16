cmake_minimum_required(VERSION 3.21.1)

project(esplamp VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_COMPILE_COMMANDS ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON CACHE INTERNAL "")
set(CMAKE_BUILD_TYPE Release)

find_package(Qt6 6.5 COMPONENTS Widgets Gui Core Quick Qml QuickControls2 Network REQUIRED)
qt_standard_project_setup()

if (ANDROID)
    set(QT_ANDROID_PACKAGE_SOURCE_DIR "${CMAKE_SOURCE_DIR}/android")
    set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -s -Os")
    set(CMAKE_EXE_LINKER_FLAGS_RELEASE "${CMAKE_EXE_LINKER_FLAGS_RELEASE} -s")

    # Enable shrinking and optimizations
    set(ANDROID_ABI "arm64-v8a")  # Use a single ABI to reduce size
    set(ANDROID_PLATFORM android-21)  # Set minimum Android platform
    set(ANDROID_PROGUARD_CONFIG_PATH "${CMAKE_SOURCE_DIR}/android/proguard-rules.pro")

    set(MAIN_ACTIVITY "org.qtproject.example.esplampapp/org.qtproject.qt.android.bindings.QtActivity")
    set(APK_PATH "${CMAKE_BINARY_DIR}/src/app/android-build/esplampapp.apk")
    set(ANDROID_PACKAGE "org.qtproject.example.esplampapp")

    ### custom commands
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
endif()

add_subdirectory(src)
