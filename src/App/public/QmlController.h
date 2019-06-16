#ifndef QMLCONTROLLER_H
#define QMLCONTROLLER_H

#include "WMObject.h"

class QQuickWindow;
class QQmlContext;
class QQmlEngine;
class QQuickItem;
class QQuickWindow;

class QmlController : public WMObject
{
	Q_OBJECT

public:
	explicit QmlController(QObject* parent = nullptr);

	QQmlContext* getRootContext() const;
	QQuickWindow* createWindow(QUrl url, QRect geometry, QQmlContext* newContext = nullptr);

	void cleanup();

signals:
	void exitRequested();

private:
	QQmlEngine* m_qmlEngine;

	QList<QQuickWindow*> m_windows;
};

#endif // QMLCONTROLLER_H