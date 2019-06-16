QT += network widgets quick quickcontrols2
CONFIG += c++17
LIBS += -luser32 -ldwmapi -lAdvAPI32 -lpowrprof

DEFINES += QT_DEPRECATED_WARNINGS

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
	public/AppCore.h \
	public/DWMThumbnail.h \
	public/QmlController.h \
	public/SettingsContainer.h \
	public/SystemWindow.h \
	public/TreeIconImageProvider.h \
	public/TreeItem.h \
	public/WMObject.h \
	public/Win.h \
	public/WinShellController.h \
	public/WinShellController/TaskBarWindow.h \
	public/WinShellController/TrayWindow.h \
	public/WindowController.h \
	public/WindowView.h \
	public/WindowView/EnumWindowsThread.h \
	public/WindowInfo.h

SOURCES += \
	main.cpp \
	private/AppCore.cpp \
	private/DWMThumbnail.cpp \
	private/QmlController.cpp \
	private/SettingsContainer.cpp \
	private/SystemWindow.cpp \
	private/TreeIconImageProvider.cpp \
	private/TreeItem.cpp \
	private/WMObject.cpp \
	private/WinShellController.cpp \
	private/WinShellController/TaskBarWindow.cpp \
	private/WinShellController/TrayWindow.cpp \
	private/WindowController.cpp \
	private/WindowView.cpp \
	private/WindowView/EnumWindowsThread.cpp \
	private/WindowInfo.cpp

RESOURCES += qml.qrc
