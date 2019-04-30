QT += widgets quick quickcontrols2
CONFIG += c++11
LIBS += -luser32 -ldwmapi

# The following define makes your compiler emit warnings if you use
# any feature of Qt which as been marked deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

INCLUDEPATH += public

RESOURCES += qml.qrc \
	graphics.qrc

# Additional import path used to resolve QML modules in Qt Creator's code model
QML_IMPORT_PATH =

# Additional import path used to resolve QML modules just for Qt Quick Designer
QML_DESIGNER_IMPORT_PATH =

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

HEADERS += \
    public/TreeItem.h \
    public/SystemWindow/StartMenuWindow.h \
    public/SystemWindow/TaskBarWindow.h \
    public/SystemWindow/TrayIconWindow.h \
    public/AppCore.h \
    public/EnumWindowsThread.h \
    public/ManagedWindow.h \
    public/Process.h \
    public/QMLApplication.h \
    public/Singleton.h \
    public/SystemWindow.h \
    public/TrayIcon.h \
    public/Win.h \
    public/WindowInfo.h \
    public/WindowManager.h

SOURCES += \
    private/TreeItem.cpp \
    private/SystemWindow/StartMenuWindow.cpp \
    private/SystemWindow/TaskBarWindow.cpp \
    private/SystemWindow/TrayIconWindow.cpp \
    private/AppCore.cpp \
    private/EnumWindowsThread.cpp \
    private/ManagedWindow.cpp \
    private/QMLApplication.cpp \
    private/SystemWindow.cpp \
    private/TrayIcon.cpp \
    private/WindowManager.cpp \
    main.cpp



