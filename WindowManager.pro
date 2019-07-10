TEMPLATE = subdirs

SUBDIRS += Shared Client Server

Server.subdir = src/Server
Client.subdir = src/Client
Shared.subdir = src/Shared

Server.depends = Client
Client.depends = Shared