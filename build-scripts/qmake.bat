call "build-scripts/config.bat"
cd build
call "%vcdir%/vcvarsall.bat" x64
call "%qtdir%/qmake.exe" ../WindowManager.pro -spec win32-msvc "CONFIG+=debug" "CONFIG+=qml_debug"