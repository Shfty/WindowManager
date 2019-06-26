#include "public/QMLController.h"

#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QQmlContext>
#include <QQmlComponent>
#include <QQuickWindow>
#include <QQuickItem>

#include <QDebug>
Q_LOGGING_CATEGORY(qmlController, "app.qmlController")

#include <TreeIconImageProvider.h>

#include "AppCore.h"
#include "TreeItem.h"

QMLController::QMLController(QObject *parent)
	: QObject(parent)
	, m_qmlEngine(nullptr)
	, m_qmlWindow(nullptr)
{
	qCInfo(qmlController) << "Startup";

	m_qmlEngine = new QQmlApplicationEngine(this);

	m_qmlEngine->rootContext()->setContextProperty("appCore", AppCore::getInstance(this));
	m_qmlEngine->addImageProvider(QLatin1String("treeIcon"), new TreeIconImageProvider);
	m_qmlEngine->load("qrc:/qml/main.qml");

	m_qmlWindow = qobject_cast<QQuickWindow*>(m_qmlEngine->rootObjects()[0]);

	int monitorIndex = QGuiApplication::arguments().at(2).toInt();
	m_qmlWindow->setScreen(QGuiApplication::screens()[monitorIndex]);

	m_qmlWindow->setProperty("_q_showWithoutActivating", QVariant(true));
	m_qmlWindow->setVisibility(QWindow::Windowed);
}

void QMLController::closeWindow()
{
	qCInfo(qmlController) << "Closing window";

	if(m_qmlWindow)
	{
		m_qmlWindow->close();
	}
}
