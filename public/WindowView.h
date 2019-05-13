#ifndef WINDOWVIEW_H
#define WINDOWVIEW_H

#include <QMetaType>
#include <QObject>
#include <QRect>
#include <QVariant>
#include <QWindow>

#include "WindowInfo.h"

class EnumWindowsThread;

class WindowView : public QObject
{
	Q_OBJECT

	Q_PROPERTY(QList<QObject*> windowList READ getWindowList NOTIFY windowListChanged)
	Q_PROPERTY(QList<QObject*> screenList READ getScreenList NOTIFY screenListChanged)

public:
	explicit WindowView(QObject* parent = nullptr);
	~WindowView();

	WindowInfo* getWindowByRegex(const QString& titlePattern = "", const QString& classPattern = "");

	QObjectList getWindowList();
	QObjectList getScreenList();

	Q_INVOKABLE QPoint getOffscreenArea();

	Q_INVOKABLE HWND getWindowHwnd(QWindow* window);

	Q_INVOKABLE HWND findWindow(QString winTitle = "", QString winClass = "", HWND after = nullptr, HWND parent = nullptr);

	Q_INVOKABLE bool isWindowVisible(HWND hwnd);

	Q_INVOKABLE bool hasWindowInfo(HWND hwnd);
	Q_INVOKABLE WindowInfo* getWindowInfo(HWND hwnd);

signals:
	void windowScanFinished();
	void windowListChanged();
	void screenListChanged();

public slots:
	void onWindowAdded(HWND hwnd, QString title, QString winClass);
	void onWindowTitleChanged(HWND hwnd, QString title);
	void onWindowRemoved(HWND hwnd);

private:
	HDWP m_dwp;
	WindowInfo* m_placeholder;

	EnumWindowsThread* m_thread;
	QMap<HWND, WindowInfo*> m_windowMap;
};

#endif // WINDOWVIEW_H