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

	enum Layer {
		Hidden,
		Underlay,
		Visible,
		Overlay,
		Unmanaged
	};

	Q_ENUM(Layer)

public slots:
	void startup();

	void registerUnderlayWindow(WindowInfo wi);
	void registerOverlayWindow(WindowInfo wi);

	void windowCreated(WindowInfo wi);
	void windowDestroyed(HWND window);

	void moveWindow(HWND hwnd, QRect geometry, bool visible);
	void updateWindowLayout();

	void setActiveWindow(HWND hwnd);
	void closeWindow(HWND hwnd);

	void setWindowStyle(HWND hwnd, qint32 style);

	void removeHwnd(HWND hwnd);

private:
	WindowInfo findWindowInfo(HWND hwnd);
	void moveWindow_internal(Layer layer, WindowInfo wi, HWND insertAfter, QRect geometry);

	HDWP m_dwp;

	QMap<Layer, QMap<WindowInfo, QRect>> m_layers;
};

#endif // WINDOWCONTROLLER_H