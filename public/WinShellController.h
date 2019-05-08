#ifndef WINSHELLCONTROLLER_H
#define WINSHELLCONTROLLER_H

#include "WMObject.h"
#include <QPointF>

class TrayIcon;
class TrayWindow;
class TaskBarWindow;

class WinShellController : public WMObject
{
	Q_OBJECT

public:
	explicit WinShellController(QObject* parent = nullptr);
	~WinShellController();

public slots:
	void showTrayIconWindow(QPointF position);

private:
	TrayIcon* m_trayIcon;
	TrayWindow* m_trayWindow;
	TaskBarWindow* m_taskBarWindow;
};

#endif // WINSHELLCONTROLLER_H
