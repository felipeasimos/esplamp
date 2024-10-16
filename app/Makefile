android-clean:
	cmake --build build/android --target clean
android-cmake-init:
	QT_HOST_PATH=~/Packages/Qt/6.5.0/gcc_64/ ~/Packages/Qt/6.5.0/android_arm64_v8a/bin/qt-cmake . -DANDROID_NDK_ROOT=${HOME}/Packages/android/ndk/23.1.7779620 -DANDROID_SDK_ROOT=${HOME}/Packages/android -DQT_CHAINLOAD_TOOLCHAIN_FILE=${HOME}/Packages/android/ndk/23.1.7779620/build/cmake/android.toolchain.cmake -GNinja -B build/android/ -S .
android-build:
	cmake --build build/android --config Release
	cmake --build build/android --target apk --config Release
android-run:
	androiddeployqt --input build/android/src/android-esplampapp-deployment-settings.json --output build/android/src/android-build/ --gradle --reinstall --debug
	cmake --build build/android --target launch_apk
android-release:
	androiddeployqt --input build/android/src/android-esplampapp-deployment-settings.json --output build/android/src/android-build/ --gradle --release --deployment bundled --reinstall --sign test.keystore test-alias
	cmake --build build/android --target launch_apk --config Release
linux-clean:
	cmake --build build/linux --target clean
linux-cmake-init:
	~/Packages/Qt/6.5.0/gcc_64/bin/qt-cmake -GNinja -S . -B build/linux
linux-build:
	cmake --build build/linux
linux-run:
	./build/linux/src/esplampapp
