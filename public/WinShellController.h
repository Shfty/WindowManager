#ifndef WINSHELLCONTROLLER_H
#define WINSHELLCONTROLLER_H

#include "WMObject.h"
#include <QPointF>
#include <QSizeF>

class TrayIcon;
class TrayWindow;
class TaskBarWindow;

class WinShellController : public WMObject
{
	Q_OBJECT

public:
	explicit WinShellController(QObject* parent = nullptr);

	void cleanup();

public slots:
	void toggleTrayIconWindow(QPointF position);

private:
	TrayIcon* m_trayIcon;
	TrayWindow* m_trayWindow;
	TaskBarWindow* m_taskBarWindow;
};

#endif // WINSHELLCONTROLLER_H
