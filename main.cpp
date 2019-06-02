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
	QGuiApplication::setAttribute(Qt::AA_UseOpenGLES); // Force OpenGL ES to prevent crashes on windows

	// Setup QML renderer
	QString productType = QSysInfo::productType();
	QString productVersion = QSysInfo::productVersion();

	if(productType == "windows" && productVersion == "10")
	{
		QQuickWindow::setSceneGraphBackend(QSGRendererInterface::Direct3D12);
	}

	QQuickStyle::setStyle("Universal");

	qRegisterMetaType<HWND>();
	qRegisterMetaType<QScreen*>();

	QGuiApplication app(argc, argv);
	app.setQuitOnLastWindowClosed(false);

	AppCore appCore;

	return app.exec();
}
