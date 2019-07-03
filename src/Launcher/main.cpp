#include <QApplication>
#include <QQuickStyle>

#include <Logging.h>

#include "LauncherCore.h"

int main(int argc, char *argv[])
{
	QApplication::setOrganizationName("Josh Palmer");
	QApplication::setOrganizationDomain("https://josh-palmer.com");
	QGuiApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
	QApplication app(argc, argv);

	QQuickStyle::setStyle("Universal");

	qInstallMessageHandler(Logging::logHandler);

	LauncherCore launcherCore;

	return app.exec();
}
