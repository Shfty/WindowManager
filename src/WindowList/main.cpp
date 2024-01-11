#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QThread>
#include <QDebug>
#include <QQuickWindow>

#include <WindowEnumerator.h>

int main(int argc, char *argv[])
{
	// Setup core application
	QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);

	QGuiApplication app(argc, argv);

	// Setup enum thread
	QThread enumThread;
	enumThread.setObjectName("Window Enumerator Thread");

	WindowEnumerator winEnumerator(nullptr);
	winEnumerator.moveToThread(&enumThread);

	QObject::connect(&enumThread, &QThread::started, &winEnumerator, &WindowEnumerator::startup);

	QObject::connect(QGuiApplication::instance(), &QGuiApplication::aboutToQuit, [&](){
		enumThread.quit();
		enumThread.wait();
	});

	// Setup QML window
	QQmlApplicationEngine engine;
	const QUrl url(QStringLiteral("qrc:/main.qml"));
	QObject::connect(&engine, &QQmlApplicationEngine::objectCreated,
					 &app, [url](QObject *obj, const QUrl &objUrl) {
		if (!obj && url == objUrl)
			QCoreApplication::exit(-1);
	}, Qt::QueuedConnection);
	engine.load(url);

	QQuickWindow* appWindow = qobject_cast<QQuickWindow*>(engine.rootObjects().first());
	qInfo() << "Object Name:" << appWindow;

	// Start
	QObject::connect(&winEnumerator, SIGNAL(enumComplete(QVariant)), appWindow, SLOT(windowListUpdated(QVariant)));

	enumThread.start();

	return app.exec();
}
