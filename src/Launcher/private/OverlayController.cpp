#include "OverlayController.h"

#include <QQmlApplicationEngine>
#include <QQmlContext>
#include <QQuickWindow>

#include <QDebug>
Q_LOGGING_CATEGORY(qmlController, "launcher.overlayController")

#include <TreeIconImageProvider.h>

#include "LauncherCore.h"

OverlayController::OverlayController(QObject* parent)
	: QObject(parent)
	, m_qmlEngine(nullptr)
	, m_qmlWindow(nullptr)
{

}

void OverlayController::startup()
{
	qCInfo(qmlController) << "Startup";

	// Register QML types
	qCInfo(qmlController) << "Registering QML Types";
	qmlRegisterInterface<HWND>("HWND");

	m_qmlEngine = new QQmlApplicationEngine(this);
	m_qmlEngine->addImageProvider(QLatin1String("treeIcon"), new TreeIconImageProvider);
	m_qmlEngine->rootContext()->setContextProperty("launcherCore", LauncherCore::getInstance(this));
	m_qmlEngine->load("qrc:/qml/main.qml");

	m_qmlWindow = qobject_cast<QQuickWindow*>(m_qmlEngine->rootObjects()[0]);

	connect(m_qmlWindow, SIGNAL(windowSelected(QVariant)), this, SIGNAL(windowSelected(QVariant)));
	connect(m_qmlWindow, SIGNAL(reloadRequested()), this, SIGNAL(reloadRequested()));
	connect(m_qmlWindow, SIGNAL(quitRequested()), this, SIGNAL(quitRequested()));

	emit windowReady(m_qmlWindow);
}

void OverlayController::moveWindow(QPoint pos, QSize size)
{
	m_qmlWindow->setPosition(pos);
	m_qmlWindow->resize(size);
}

void OverlayController::showWindow(QString item)
{
	QString currentItem = m_qmlWindow->property("currentItem").toString();

	if(currentItem == item && m_qmlWindow->isVisible())
	{
		m_qmlWindow->hide();
	}
	else if(!m_qmlWindow->isVisible())
	{
		m_qmlWindow->show();
	}

	m_qmlWindow->setProperty("currentItem", item);
}

void OverlayController::cleanup()
{
	if(m_qmlWindow != nullptr)
	{
		m_qmlWindow->close();
	}
}
