#include "public/QMLController.h"

#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QQmlContext>
#include <QQmlComponent>
#include <QQuickWindow>
#include <QQuickItem>

#include <QDebug>
Q_LOGGING_CATEGORY(qmlController, "app.qmlController")

#include <Win.h>
#include <TreeIconImageProvider.h>

#include "ClientCore.h"
#include "TreeItem.h"

#ifdef QT_DEBUG
#define QML_PREFIX QString("./src/Client/")
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
	m_qmlEngine->rootContext()->setContextProperty("HWND_NULL", QVariant::fromValue<HWND>(nullptr));
#ifdef QT_DEBUG
	m_qmlEngine->rootContext()->setContextProperty("QT_DEBUG", true);
#else
	m_qmlEngine->rootContext()->setContextProperty("QT_DEBUG", false);
#endif

	connect(m_qmlEngine, &QQmlApplicationEngine::objectCreated, this, &QMLController::qmlLoaded);
}

void QMLController::startup()
{
	reloadQml();
}

void QMLController::ipcReady()
{
	emit sendMessage({
		"WindowChanged",
		QVariant::fromValue<HWND>(reinterpret_cast<HWND>(m_qmlWindow->winId()))
	});
}

void QMLController::reloadQml()
{
	qCInfo(qmlController) << "Reloading QML";

	if(m_qmlWindow != nullptr)
	{
		qCInfo(qmlController) << "Closing existing window";
		m_qmlWindow->close();
		m_qmlWindow->destroy();
		m_qmlWindow = nullptr;

		emit sendMessage({
			"WindowChanged",
			QVariant::fromValue<HWND>(nullptr)
		});
	}

	qCInfo(qmlController) << "Loading main.qml";
	m_qmlEngine->clearComponentCache();
	m_qmlEngine->load(QML_PREFIX + "qml/main.qml");
}

void QMLController::qmlLoaded()
{
	m_qmlWindow = qobject_cast<QQuickWindow*>(m_qmlEngine->rootObjects().last());

	qCInfo(qmlController) << "Assigning app core reference";
	m_qmlWindow->setProperty("clientCore", QVariant::fromValue<ClientCore*>(ClientCore::getInstance(this)));

	qCInfo(qmlController) << "Assigning monitor index";
	int monitorIndex = QGuiApplication::arguments().at(2).toInt();
	m_qmlWindow->setScreen(QGuiApplication::screens()[monitorIndex]);

	qCInfo(qmlController) << "Showing window";
	m_qmlWindow->setProperty("_q_showWithoutActivating", QVariant(true));
	m_qmlWindow->setVisibility(QWindow::Windowed);

	emit sendMessage({
		"WindowChanged",
		QVariant::fromValue<HWND>(reinterpret_cast<HWND>(m_qmlWindow->winId()))
	});
}

void QMLController::closeWindow()
{
	qCInfo(qmlController) << "Closing window";

	if(m_qmlWindow)
	{
		m_qmlWindow->close();
	}
}
