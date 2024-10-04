android-cmake-init:
	QT_HOST_PATH=~/Packages/Qt/6.5.0/gcc_64/ ~/Packages/Qt/6.5.0/android_arm64_v8a/bin/qt-cmake . -DANDROID_NDK_ROOT=${HOME}/Packages/android/ndk/23.1.7779620 -DANDROID_SDK_ROOT=${HOME}/Packages/android -DQT_CHAINLOAD_TOOLCHAIN_FILE=${HOME}/Packages/android/ndk/23.1.7779620/build/cmake/android.toolchain.cmake -GNinja -B build/android/ -S .
android-build:
	cmake --build build/android
	cmake --build build/android --target apk
android-run:
	androiddeployqt --input build/android/src/android-esplamp-deployment-settings.json --output build/android/src/android-build/ --gradle --reinstall
	cmake --build build/android --target launch_apk
linux-cmake-init:
	~/Packages/Qt/6.5.0/gcc_64/bin/qt-cmake -GNinja -S . -B build/linux
linux-build:
	cmake --build build/linux
linux-run:
	./build/linux/src/esplamp
