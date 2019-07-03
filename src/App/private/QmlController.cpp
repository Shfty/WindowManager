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

#ifdef QT_DEBUG
#define QML_PREFIX QString("./src/App/")
#else
#define QML_PREFIX QString("qrc:/")
#endif

QMLController::QMLController(QObject *parent)
	: QObject(parent)
	, m_qmlEngine(nullptr)
	, m_qmlWindow(nullptr)
{
	qCInfo(qmlController) << "Startup";

	m_qmlEngine = new QQmlApplicationEngine(this);
	m_qmlEngine->addImageProvider(QLatin1String("treeIcon"), new TreeIconImageProvider);

	connect(m_qmlEngine, &QQmlApplicationEngine::objectCreated, [=](){
		m_qmlWindow = qobject_cast<QQuickWindow*>(m_qmlEngine->rootObjects().last());

		qCInfo(qmlController) << "Assigning app core reference";
		m_qmlWindow->setProperty("appCore", QVariant::fromValue<AppCore*>(AppCore::getInstance(this)));

		qCInfo(qmlController) << "Assigning monitor index";
		int monitorIndex = QGuiApplication::arguments().at(2).toInt();
		m_qmlWindow->setScreen(QGuiApplication::screens()[monitorIndex]);

		qCInfo(qmlController) << "Showing window";
		m_qmlWindow->setProperty("_q_showWithoutActivating", QVariant(true));
		m_qmlWindow->setVisibility(QWindow::Windowed);
	});

	reloadQml();
}

void QMLController::reloadQml()
{
	qCInfo(qmlController) << "Reloading QML";

	if(m_qmlWindow != nullptr)
	{
		qCInfo(qmlController) << "Closing existing window";
		m_qmlWindow->close();
		m_qmlWindow->deleteLater();
	}

	qCInfo(qmlController) << "Loading main.qml";
	m_qmlEngine->clearComponentCache();

	m_qmlEngine->load(QML_PREFIX + "qml/main.qml");
}

void QMLController::closeWindow()
{
	qCInfo(qmlController) << "Closing window";

	if(m_qmlWindow)
	{
		m_qmlWindow->close();
	}
}
