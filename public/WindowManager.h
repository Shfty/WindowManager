#ifndef WINDOWMANAGER_H
#define WINDOWMANAGER_H

#include <QMetaType>
#include <QObject>
#include <QRect>
#include <QVariant>
#include <QWindow>

#include "WindowInfo.h"

class EnumWindowsThread;

class WindowManager : public QObject
{
	Q_OBJECT

	Q_PROPERTY(QList<QObject*> windowList READ getWindowList NOTIFY windowListChanged)

public:
	explicit WindowManager(QObject* parent = nullptr);
	~WindowManager();

	static WindowManager& instance()
	{
		static WindowManager inst;
		return inst;
	}

	HWND getWindowByRegex(const QString& titlePattern = "", const QString& classPattern = "");

	QObjectList getWindowList();

	Q_INVOKABLE QRect getDesktopRect();
	Q_INVOKABLE QRect getDesktopWorkArea();
	Q_INVOKABLE QPoint getOffscreenArea();

	Q_INVOKABLE HWND getWindowHwnd(QWindow* window);
	Q_INVOKABLE QPoint getWindowPosition(HWND hwnd);
	Q_INVOKABLE QSize getWindowSize(HWND hwnd);

	Q_INVOKABLE void beginMoveWindows();
	Q_INVOKABLE void moveWindow(HWND hwnd, QPoint position, QSize size = QSize());
	Q_INVOKABLE void moveWindow(QWindow* window, QPoint position, QSize size = QSize());
	Q_INVOKABLE void endMoveWindows();

	Q_INVOKABLE HWND findWindow(QString winTitle = "", QString winClass = "");

	Q_INVOKABLE void showWindow(HWND hwnd);
	Q_INVOKABLE void hideWindow(HWND hwnd);

	Q_INVOKABLE bool isWindowVisible(HWND hwnd);

	Q_INVOKABLE bool hasWindowInfo(HWND hwnd);
	Q_INVOKABLE WindowInfo* getWindowInfo(HWND hwnd);
	Q_INVOKABLE QString getWindowTitle(HWND hwnd);
	Q_INVOKABLE QString getWindowClass(HWND hwnd);

	Q_INVOKABLE void attachWindowToDesktop(QWindow* window);
	Q_INVOKABLE void detachWindowFromDesktop(QWindow* window);
	Q_INVOKABLE void setBackgroundWindow(QWindow* hwnd);

signals:
	void windowScanFinished();
	void windowListChanged();

public slots:
	void onWindowAdded(HWND hwnd, QString title);
	void onWindowTitleChanged(HWND hwnd, QString title);
	void onWindowRemoved(HWND hwnd);

private:
	HDWP m_dwp;
	WindowInfo* m_placeholder;

	EnumWindowsThread* m_thread;
	QMap<QString, WindowInfo*> m_windowMap;
};

#endif // WINDOWMANAGER_H