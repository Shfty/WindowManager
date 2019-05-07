#include "QMLApplication.h"
#include "Win.h"

#include "ManagedWindow.h"
#include "Settings.h"
#include "TreeItem.h"
#include "WindowManager.h"

#include <QQmlEngine>
#include <QQmlContext>
#include <QQuickWindow>
#include <QQuickStyle>
#include <QQmlIncubationController>
#include <QGuiApplication>

class PeriodicIncubationController : public QObject,
                                     public QQmlIncubationController
{
public:
    PeriodicIncubationController() {
        m_timerId = startTimer(16);
	}

	~PeriodicIncubationController() {
		killTimer(m_timerId);
	}

protected:
    void timerEvent(QTimerEvent *) override {
        incubateFor(8);
    }

private:
	int m_timerId;
};

QMLApplication::QMLApplication(QObject* parent)
	: QObject(parent),
	m_qmlEngine(new QQmlEngine(this)),
	m_configWindow(nullptr)
{
	qmlRegisterType<ManagedWindow>("ManagedWindow", 1, 0, "ManagedWindow");
	qmlRegisterInterface<HWND>("HWND");

	static PeriodicIncubationController inc;
	m_qmlEngine->setIncubationController(&inc);

	connect(m_qmlEngine, SIGNAL(quit()), QGuiApplication::instance(), SLOT(quit()));

	QQmlContext* rootContext = getRootContext();
	rootContext->setContextProperty("settings", &Settings::instance());
	rootContext->setContextProperty("windowManager", &WindowManager::instance());

	m_configWindow = createWindow(QUrl("qrc:/qml/config/ConfigWindow.qml"), QRect(0, 0, 1280, 720));

}

QMLApplication::~QMLApplication()
{

}

QQmlContext* QMLApplication::getRootContext() const
{
	if(m_qmlEngine != nullptr)
	{
		return m_qmlEngine->rootContext();
	}

	return nullptr;
}

QQuickWindow* QMLApplication::createWindow(QUrl url, QRect geometry, QQmlContext* newContext)
{
	QQmlComponent component(m_qmlEngine, url);
	QQmlContext* parentContext = newContext ? newContext : getRootContext();
	QQuickItem* newItem = qobject_cast<QQuickItem*>(component.create(parentContext));

	qDebug() << component.errors();

	QQuickWindow* newWindow = new QQuickWindow();
	newWindow->setGeometry(geometry);
	newItem->setParentItem(newWindow->contentItem());

	return newWindow;
}

void QMLApplication::showConfigWindow()
{
	if(m_configWindow != nullptr)
	{
		m_configWindow->show();
	}
}