QT += widgets quick quickcontrols2 network
CONFIG += c++17
LIBS += -luser32 -ldwmapi

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
	public/ClientCore.h \
	public/DWMThumbnail.h \
	public/IPCClient.h \
	public/QMLController.h \
	public/TreeItem.h \
	public/TreeModel.h

SOURCES += \
	main.cpp \
	private/ClientCore.cpp \
	private/DWMThumbnail.cpp \
	private/IPCClient.cpp \
	private/QMLController.cpp \
	private/TreeItem.cpp \
	private/TreeModel.cpp

RESOURCES += qml.qrc

# Shared library
win32:CONFIG(release, debug|release): LIBS += -L$$OUT_PWD/../Shared/release/ -lShared
else:win32:CONFIG(debug, debug|release): LIBS += -L$$OUT_PWD/../Shared/debug/ -lShared

INCLUDEPATH += $$PWD/../Shared/public
DEPENDPATH += $$PWD/../Shared/public

win32-g++:CONFIG(release, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../Shared/release/libShared.a
else:win32-g++:CONFIG(debug, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../Shared/debug/libShared.a
else:win32:!win32-g++:CONFIG(release, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../Shared/release/Shared.lib
else:win32:!win32-g++:CONFIG(debug, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../Shared/debug/Shared.lib
