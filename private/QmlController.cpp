#include "QmlController.h"
#include "Win.h"

#include "AppCore.h"
#include "DWMThumbnail.h"
#include "SettingsContainer.h"
#include "TreeItem.h"
#include "WindowController.h"
#include "TreeIconImageProvider.h"

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
	void timerEvent(QTimerEvent* e) override {
		incubateFor(5);
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

	m_qmlEngine->setIncubationController(new PeriodicIncubationController());

	m_qmlEngine->addImageProvider(QLatin1String("treeIcon"), new TreeIconImageProvider);

	connect(m_qmlEngine, &QQmlEngine::quit, this, &QmlController::cleanup);
}

void QmlController::cleanup()
{
	while(m_windows.length() > 0)
	{
		QQuickWindow* window = m_windows.takeLast();
		window->close();
	}
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
	qInfo() << "Creating window for" << url.url() << "with geometry" << geometry << "under context" << newContext;

	QQuickWindow* newWindow = new QQuickWindow();
	newWindow->setGeometry(geometry);

	QQmlComponent component(m_qmlEngine, url);
	if(component.errors().length() > 0)
	{
		qCritical() << component.errors();
	}

	QQmlContext* parentContext = newContext ? newContext : getRootContext();
	QQuickItem* newItem = qobject_cast<QQuickItem*>(component.create(parentContext));
	newItem->setParent(newWindow);
	newItem->setParentItem(newWindow->contentItem());

	m_windows.append(newWindow);

	return newWindow;
}
