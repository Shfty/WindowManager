#include "AppCore.h"
#include "Win.h"

#include <QGuiApplication>
#include <QQuickStyle>
#include <QQuickWindow>
#include <QScreen>

int main(int argc, char *argv[])
{
	Q_INIT_RESOURCE(graphics);

	QGuiApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
	QQuickStyle::setStyle("Universal");
	QQuickWindow::setDefaultAlphaBuffer(true);

	qRegisterMetaType<HWND>();
	qRegisterMetaType<QScreen*>();

	QGuiApplication app(argc, argv);
	AppCore appCore;
	return app.exec();
}
