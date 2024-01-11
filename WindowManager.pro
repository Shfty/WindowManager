TEMPLATE = subdirs

SUBDIRS += Shared Client Server WindowList

Server.subdir = src/Server
Client.subdir = src/Client
Shared.subdir = src/Shared
WindowList.subdir = src/WindowList

Server.depends = Client WindowList
Client.depends = Shared