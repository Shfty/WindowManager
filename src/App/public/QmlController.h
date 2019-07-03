#ifndef QMLCONTROLLER_H
#define QMLCONTROLLER_H

#include <QObject>

#include <QLoggingCategory>
Q_DECLARE_LOGGING_CATEGORY(qmlController);

class QQmlApplicationEngine;
class QQuickWindow;
class QQmlComponent;
class QQmlItem;
class TreeItem;

class QMLController : public QObject
{
	Q_OBJECT
public:
	explicit QMLController(QObject *parent = nullptr);

	QQuickWindow* getQmlWindow() const { return m_qmlWindow; }

public slots:
	void reloadQml();
	void closeWindow();

private:
	QQmlApplicationEngine* m_qmlEngine;
	QQuickWindow* m_qmlWindow;
};

#endif // QMLCONTROLLER_H
