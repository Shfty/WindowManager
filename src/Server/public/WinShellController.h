#ifndef WINSHELLCONTROLLER_H
#define WINSHELLCONTROLLER_H

#include <QObject>
#include <QPointF>
#include <QSizeF>

#include <QLoggingCategory>
Q_DECLARE_LOGGING_CATEGORY(winShellController);

class TrayWindow;
class TaskBarWindow;

class WinShellController : public QObject
{
	Q_OBJECT

public:
	explicit WinShellController(QObject* parent = nullptr);

public slots:
	void startup();

	void toggleTray(QPointF position);

	void shutdown();
	void reboot();
	void sleep();
	void logout();

	void cleanup();

private:
	void elevatePrivileges();

	TrayWindow* m_trayWindow;
	TaskBarWindow* m_taskBarWindow;
};

#endif // WINSHELLCONTROLLER_H
