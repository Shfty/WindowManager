#ifndef WINDOWVIEW_H
#define WINDOWVIEW_H

#include <QMetaType>
#include <QObject>
#include <QRect>
#include <QVariant>
#include <QWindow>

#include <QLoggingCategory>
Q_DECLARE_LOGGING_CATEGORY(windowView);

#include "WindowInfo.h"

class WindowView : public QObject
{
	Q_OBJECT

	Q_PROPERTY(QList<QObject*> windowList READ getWindowList NOTIFY windowListChanged)
	Q_PROPERTY(QList<QObject*> screenList READ getScreenList NOTIFY screenListChanged)

public:
	explicit WindowView(QObject* parent = nullptr);

	WindowObject* getWindowByRegex(const QString& titlePattern = "", const QString& classPattern = "");

	QObjectList getWindowList();
	QObjectList getScreenList();

	Q_INVOKABLE HWND getWindowHwnd(QWindow* window);

	Q_INVOKABLE HWND findWindow(QString winTitle = "", QString winClass = "", HWND after = nullptr, HWND parent = nullptr);

	Q_INVOKABLE bool isWindowVisible(HWND hwnd);

	Q_INVOKABLE bool hasWindowInfo(HWND hwnd);
	Q_INVOKABLE WindowObject* getWindowInfo(HWND hwnd);

	Q_INVOKABLE WindowObject* objectToWindowInfo(QObject* obj);

signals:
	void windowListChanged();
	void screenListChanged();

public slots:
	void windowCreated(WindowInfo wi);
	void windowTitleChanged(HWND hwnd, QString title);
	void windowDestroyed(HWND hwnd);

private:
	HDWP m_dwp;
	WindowObject* m_placeholder;
	QMap<HWND, WindowObject*> m_windowMap;
};

#endif // WINDOWVIEW_H