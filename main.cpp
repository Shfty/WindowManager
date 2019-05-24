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

	// Setup QML renderer
	QString productType = QSysInfo::productType();
	QString productVersion = QSysInfo::productVersion();

	if(productType == "windows" && productVersion == "10")
	{
		QQuickWindow::setSceneGraphBackend(QSGRendererInterface::Direct3D12);
	}
	else
	{
		// Force OpenGL ES to prevent crashes on windows
		QGuiApplication::setAttribute(Qt::AA_UseOpenGLES);
	}

	qRegisterMetaType<HWND>();
	qRegisterMetaType<QScreen*>();

	QGuiApplication app(argc, argv);
	AppCore appCore;
	return app.exec();
}
