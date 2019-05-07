call "build-scripts/config.bat"
cd build
call "%qtdir%/windeployqt.exe" --qmldir ../qml ./debug
