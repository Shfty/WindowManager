#include "AppCore.h"
#include "Win.h"

#include <QGuiApplication>
#include <QQuickStyle>
#include <QQuickWindow>
#include <QScreen>
#include <QDateTime>

#include <Logging.h>

namespace Logging {
	static AppCore* appCore = nullptr;

	static void appCoreLogHandler(QtMsgType type, const QMessageLogContext& ctx, const QString& msg)
	{
		if(Logging::appCore != nullptr)
		{
			Logging::appCore->logReceived(type, ctx, msg);
		}
	}
}

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

	qInstallMessageHandler(Logging::appCoreLogHandler);

	AppCore appCore;
	Logging::appCore = &appCore;

	return app.exec();
}
