#include "QmlController.h"
#include "Win.h"

#include "DWMThumbnail.h"
#include "SettingsContainer.h"
#include "TreeItem.h"
#include "WindowController.h"

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

	~PeriodicIncubationController() override {
		killTimer(m_timerId);
	}

protected:
	void timerEvent(QTimerEvent *) override {
		incubateFor(8);
	}

private:
	int m_timerId;
};

QmlController::QmlController(QObject* parent)
	: WMObject(parent),
	m_qmlEngine(new QQmlEngine(this))
{
	qRegisterMetaType<QmlController*>();
	setObjectName("QML Controller");

	qmlRegisterType<DWMThumbnail>("DWMThumbnail", 1, 0, "DWMThumbnail");
	qmlRegisterInterface<HWND>("HWND");

	static PeriodicIncubationController inc;
	m_qmlEngine->setIncubationController(&inc);

	connect(m_qmlEngine, SIGNAL(quit()), QGuiApplication::instance(), SLOT(quit()));
}

QmlController::~QmlController()
{

}

QQmlContext* QmlController::getRootContext() const
{
	if(m_qmlEngine != nullptr)
	{
		return m_qmlEngine->rootContext();
	}

	return nullptr;
}

QQuickWindow* QmlController::createWindow(QUrl url, QRect geometry, QQmlContext* newContext)
{

	QQuickWindow* newWindow = new QQuickWindow();
	newWindow->setGeometry(geometry);
	connect(QGuiApplication::instance(), SIGNAL(aboutToQuit()), newWindow, SLOT(deleteLater()));

	QQmlComponent component(m_qmlEngine, url);
	qDebug() << component.errors();

	QQmlContext* parentContext = newContext ? newContext : getRootContext();
	QQuickItem* newItem = qobject_cast<QQuickItem*>(component.create(parentContext));
	newItem->setParent(newWindow);
	newItem->setParentItem(newWindow->contentItem());

	return newWindow;
}
