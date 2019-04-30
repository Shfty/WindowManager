#include "AppCore.h"

int main(int argc, char *argv[])
{
	Q_INIT_RESOURCE(graphics);

	// Application
	QApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
	QApplication app(argc, argv);

	AppCore appCore;
	QObject::connect(&app, SIGNAL(aboutToQuit()), &appCore, SLOT(handleAboutToQuit()));

	return app.exec();
}
