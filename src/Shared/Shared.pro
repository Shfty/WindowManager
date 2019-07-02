QT += widgets quick
CONFIG += staticlib
LIBS += -luser32 -lAdvAPI32 -lpowrprof

CONFIG(release, debug|release):DEFINES += QT_NO_DEBUG_OUTPUT QT_NO_INFO_OUTPUT QT_NO_WARNING_OUTPUT

TARGET = Shared
TEMPLATE = lib

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
	public/Logging.h \
	public/SettingsContainer.h \
	public/TreeIconImageProvider.h \
	public/WindowInfo.h \
	public/WindowView.h \
	public/Win.h \
	public/Shared.h

SOURCES += \
	private/SettingsContainer.cpp \
	private/TreeIconImageProvider.cpp \
	private/Win.cpp \
	private/WindowInfo.cpp \
	private/WindowView.cpp \
	private/Shared.cpp
