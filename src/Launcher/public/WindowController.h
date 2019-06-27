#ifndef WINDOWCONTROLLER_H
#define WINDOWCONTROLLER_H

#include <QMetaType>
#include <QObject>
#include <QRect>
#include <QVariant>
#include <QWindow>

#include <QLoggingCategory>
Q_DECLARE_LOGGING_CATEGORY(windowController);

#include <WindowInfo.h>

class WindowController : public QObject
{
	Q_OBJECT

public:
	explicit WindowController(QObject* parent = nullptr);

public slots:
	void startup();

	void registerUnderlayWindow(HWND hwnd);
	void registerOverlayWindow(HWND hwnd);

	void windowAdded(HWND window, QString winTitle, QString winClass, QString winProcess, qint32 winStyle);
	void windowRemoved(HWND window);

	void moveWindow(HWND hwnd, QRect geometry, bool visible);
	void endMoveWindows();

	void setWindowStyle(HWND hwnd, qint32 style);

private:
	void moveWindow_internal(HWND hwnd, HWND insertAfter, QRect geometry);

	HDWP m_dwp;

	QMap<HWND, QRect> m_hiddenWindows;
	QMap<HWND, QRect> m_underlayWindows;
	QMap<HWND, QRect> m_visibleWindows;
	QMap<HWND, QRect> m_overlayWindows;
};

#endif // WINDOWCONTROLLER_H