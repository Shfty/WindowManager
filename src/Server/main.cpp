#include <QApplication>
#include <QQuickStyle>

#include <Logging.h>

#include "ServerCore.h"

int main(int argc, char *argv[])
{
	QApplication::setApplicationName("Window Manager Server");
	QApplication::setOrganizationName("Josh Palmer");
	QApplication::setOrganizationDomain("https://josh-palmer.com");
	QApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
	QApplication app(argc, argv);

	QQuickStyle::setStyle("Universal");

	qInstallMessageHandler(Logging::logHandler);

	ServerCore serverCore;

	return app.exec();
}
