#ifndef OVERLAYCONTROLLER_H
#define OVERLAYCONTROLLER_H

#include <QObject>
#include <QPoint>
#include <QSize>

#include <QLoggingCategory>
Q_DECLARE_LOGGING_CATEGORY(qmlController)

class QQmlApplicationEngine;
class QQuickWindow;
class WindowInfo;

class OverlayController : public QObject
{
	Q_OBJECT

public:
	explicit OverlayController(QObject* parent = nullptr);

signals:
	void windowSelected(QVariant windowInfo);
	void quitRequested();

public slots:
	void moveWindow(QPoint pos, QSize size);
	void showWindow(QString item);
	void cleanup();

private:
	QQmlApplicationEngine* m_qmlEngine;
	QQuickWindow* m_qmlWindow;
};

#endif // OVERLAYCONTROLLER_H
