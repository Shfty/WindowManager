#include "AppCore.h"
#include "Win.h"

#include <QGuiApplication>
#include <QQuickStyle>
#include <QQuickWindow>
#include <QScreen>
#include <QDateTime>

#include <Logging.h>

int main(int argc, char *argv[])
{
	QGuiApplication app(argc, argv);
	QGuiApplication::setApplicationName("Window Manager");
	QGuiApplication::setOrganizationName("Josh Palmer");
	QGuiApplication::setOrganizationDomain("https://josh-palmer.com");
	//QGuiApplication::setQuitOnLastWindowClosed(false);
	QGuiApplication::setAttribute(Qt::AA_EnableHighDpiScaling);

	QStringList args = QCoreApplication::arguments();
	if(args.length() <= 2)
	{
		qCritical("No ID / save file specified, quitting...");
		return EXIT_FAILURE;
	}

	// Setup QML renderer
	QString productType = QSysInfo::productType();
	QString productVersion = QSysInfo::productVersion();

	QQuickStyle::setStyle("Universal");

	qInstallMessageHandler(Logging::logHandler);

	AppCore appCore;

	return app.exec();
}
