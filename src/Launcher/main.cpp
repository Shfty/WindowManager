#include <QApplication>
#include <QDebug>

#include "LauncherCore.h"

int main(int argc, char *argv[])
{
	QApplication app(argc, argv);
	QApplication::setOrganizationName("Josh Palmer");
	QApplication::setOrganizationDomain("https://josh-palmer.com");

	Q_INIT_RESOURCE(graphics);

	LauncherCore launcherCore;

	return app.exec();
}
