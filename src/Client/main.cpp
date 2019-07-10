#include "ClientCore.h"
#include "Win.h"

#include <QGuiApplication>
#include <QQuickStyle>
#include <QQuickWindow>
#include <QScreen>
#include <QDateTime>

#include <Logging.h>

int main(int argc, char *argv[])
{
	QGuiApplication::setApplicationName("Window Manager Client");
	QGuiApplication::setOrganizationName("Josh Palmer");
	QGuiApplication::setOrganizationDomain("https://josh-palmer.com");
	//QGuiApplication::setQuitOnLastWindowClosed(false);
	QGuiApplication::setAttribute(Qt::AA_EnableHighDpiScaling);

	QGuiApplication app(argc, argv);

	QStringList args = QCoreApplication::arguments();
	if(args.length() <= 2)
	{
		qCritical("No ID / save file specified, quitting...");
		return EXIT_FAILURE;
	}

	// Setup QML renderer
	QQuickStyle::setStyle("Universal");

	qInstallMessageHandler(Logging::logHandler);

	ClientCore clientCore;

	return app.exec();
}
