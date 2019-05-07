call "build-scripts/config.bat"
cd build
call "%vcdir%/vcvarsall.bat" x64
call "%qtcreatordir%/jom.exe" clean
