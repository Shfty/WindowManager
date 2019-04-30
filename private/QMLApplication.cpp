#include "QMLApplication.h"
#include "Win.h"

#include <QQuickStyle>
#include <QQmlContext>
#include "Process.h"
#include "ManagedWindow.h"
#include "WindowManager.h"
#include "TreeItem.h"

QMLApplication::QMLApplication(QObject *parent) : QQmlApplicationEngine(parent)
{
	QQuickStyle::setStyle("Universal");

	qmlRegisterType<Process>("Process", 1, 0, "Process");
	qmlRegisterType<ManagedWindow>("ManagedWindow", 1, 0, "ManagedWindow");
	qmlRegisterInterface<HWND>("HWND");

	rootContext()->setContextProperty("windowManager", &WindowManager::instance());

	load(QUrl(QStringLiteral("qrc:/qml/main.qml")));
}

void QMLApplication::setModel(TreeItem* model)
{
	rootContext()->setContextProperty("treeModel", model);
}

void QMLApplication::showWindow()
{
	rootContext()->setContextProperty("windowVisible", QVariant::fromValue(true));
}
