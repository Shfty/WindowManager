TEMPLATE = subdirs

SUBDIRS += Shared App Launcher

Launcher.subdir = src/Launcher
App.subdir = src/App
Shared.subdir = src/Shared

Launcher.depends = App
App.depends = Shared