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
		~QmlController();

		QQmlContext* getRootContext() const;
		QQuickWindow* createWindow(QUrl url, QRect geometry, QQmlContext* newContext = nullptr);

	private:
		QQmlEngine* m_qmlEngine;
};

#endif // QMLCONTROLLER_H