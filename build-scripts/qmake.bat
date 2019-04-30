cd build
call "C:\Program Files (x86)\Microsoft Visual Studio 14.0\VC\vcvarsall.bat" x64
call "C:\SDK\Qt\5.11.0\msvc2017_64\bin\qmake.exe" P:\Personal\C++\WindowManager\WindowManager.pro -spec win32-msvc "CONFIG+=debug" "CONFIG+=qml_debug"
