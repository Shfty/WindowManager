#include "AppCore.h"

#include <QGuiApplication>
#include <QQuickStyle>
#include <QQuickWindow>

int main(int argc, char *argv[])
{
	Q_INIT_RESOURCE(graphics);

	QApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
	QQuickStyle::setStyle("Universal");
	QQuickWindow::setDefaultAlphaBuffer(true);

	QGuiApplication app(argc, argv);

	AppCore appCore;
	QObject::connect(&app, SIGNAL(aboutToQuit()), &appCore, SLOT(handleAboutToQuit()));

	return app.exec();
}
