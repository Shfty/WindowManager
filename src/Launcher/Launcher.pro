QT += widgets quick quickcontrols2 network
CONFIG += c++17
LIBS += -luser32 -lAdvAPI32 -lpowrprof

CONFIG(release, debug|release):DEFINES += QT_NO_DEBUG_OUTPUT QT_NO_INFO_OUTPUT QT_NO_WARNING_OUTPUT

DEFINES += QT_DEPRECATED_WARNINGS

# Tidy output folders
Release:DESTDIR = release
Release:OBJECTS_DIR = release/.obj
Release:MOC_DIR = release/.moc
Release:RCC_DIR = release/.rcc
Release:UI_DIR = release/.ui

Debug:DESTDIR = debug
Debug:OBJECTS_DIR = debug/.obj
Debug:MOC_DIR = debug/.moc
Debug:RCC_DIR = debug/.rcc
Debug:UI_DIR = debug/.ui

INCLUDEPATH += public

HEADERS += \
	public/IPCServer.h \
	public/LauncherCore.h \
	public/OverlayController.h \
	public/SubprocessController.h \
	public/SystemWindow.h \
	public/TrayIcon.h \
	public/WinShellController.h \
	public/WinShellController/TaskBarWindow.h \
	public/WinShellController/TrayWindow.h \
	public/WindowController.h \
	public/WindowModelThread.h

SOURCES += \
	main.cpp \
	private/IPCServer.cpp \
	private/LauncherCore.cpp \
	private/OverlayController.cpp \
	private/SubprocessController.cpp \
	private/SystemWindow.cpp \
	private/TrayIcon.cpp \
	private/WinShellController.cpp \
	private/WinShellController/TaskBarWindow.cpp \
	private/WinShellController/TrayWindow.cpp \
	private/WindowController.cpp \
	private/WindowModelThread.cpp

RESOURCES += qml.qrc \
	graphics.qrc

# Shared library
win32:CONFIG(release, debug|release): LIBS += -L$$OUT_PWD/../Shared/release/ -lShared
else:win32:CONFIG(debug, debug|release): LIBS += -L$$OUT_PWD/../Shared/debug/ -lShared

INCLUDEPATH += $$PWD/../Shared/public
DEPENDPATH += $$PWD/../Shared/public

win32-g++:CONFIG(release, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../Shared/release/libShared.a
else:win32-g++:CONFIG(debug, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../Shared/debug/libShared.a
else:win32:!win32-g++:CONFIG(release, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../Shared/release/Shared.lib
else:win32:!win32-g++:CONFIG(debug, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../Shared/debug/Shared.lib
