#ifndef WINDOWMANAGER_H
#define WINDOWMANAGER_H

#include "Win.h"
#include "WindowInfo.h"
#include <QMetaType>
#include <QObject>
#include <QRect>
#include <QVariant>
#include <QWindow>

class EnumWindowsThread;

class WindowManager : public QObject
{
	Q_OBJECT

	Q_PROPERTY(QStringList windowStringList READ getWindowStringList NOTIFY windowStringListChanged)
	Q_PROPERTY(QVariantMap windowList READ getWindowList NOTIFY windowListChanged)

public:
	explicit WindowManager(QObject* parent = nullptr);
	~WindowManager();

	static WindowManager& instance()
	{
		static WindowManager* _instance = nullptr;
		if(_instance == nullptr)
		{
			_instance = new WindowManager();
		}
		return *_instance;
	}

	HWND getWindowByRegex(const QString& titlePattern = "", const QString& classPattern = "");

	QStringList getWindowStringList();
	QVariantMap getWindowList();

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
	Q_INVOKABLE WindowInfo getWindowInfo(HWND hwnd);
	Q_INVOKABLE QString getWindowTitle(HWND hwnd);
	Q_INVOKABLE QString getWindowClass(HWND hwnd);

	Q_INVOKABLE void attachWindowToDesktop(QWindow* window);
	Q_INVOKABLE void detachWindowFromDesktop(QWindow* window);
	Q_INVOKABLE void setBackgroundWindow(QWindow* hwnd);

signals:
	void windowScanFinished();
	void windowListChanged();
	void windowStringListChanged();

public slots:
	void onWindowAdded(WindowInfo windowInfo);
	void onWindowChanged(WindowInfo windowInfo);
	void onWindowRemoved(WindowInfo windowInfo);

private:
	HDWP m_dwp;

	EnumWindowsThread* m_thread;
	QMap<QString, WindowInfo> m_windowMap;
};

#endif // WINDOWMANAGER_H