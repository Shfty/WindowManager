TEMPLATE = subdirs
SUBDIRS = \
	App \
	Launcher

Launcher.subdir = src/Launcher
App.subdir = src/App

Launcher.depends = App