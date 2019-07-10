#include "WinShellController.h"

#include <QDebug>
Q_LOGGING_CATEGORY(winShellController, "launcher.winShellController")

#include "WinShellController/TaskBarWindow.h"
#include "WinShellController/TrayWindow.h"
#include "Win.h"

WinShellController::WinShellController(QObject* parent)
	: QObject(parent)
	, m_trayWindow(new TrayWindow(this))
	, m_taskBarWindow(new TaskBarWindow(this))
{
	qRegisterMetaType<WinShellController*>();
	setObjectName("Windows Shell Controller");
}

void WinShellController::startup()
{
	qCInfo(winShellController) << "Startup";

	elevatePrivileges();
	m_taskBarWindow->hide();
}

void WinShellController::elevatePrivileges()
{
	qCInfo(winShellController, "Elevating privileges");

	HANDLE hToken = nullptr;
	if(OpenProcessToken(GetCurrentProcess(), TOKEN_ADJUST_PRIVILEGES | TOKEN_QUERY, &hToken))
	{
		TOKEN_PRIVILEGES tkp;
		if(LookupPrivilegeValue(nullptr, SE_SHUTDOWN_NAME, &tkp.Privileges[0].Luid))
		{
			tkp.PrivilegeCount = 1;
			tkp.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED;

			if(AdjustTokenPrivileges(hToken, false, &tkp, 0, nullptr, nullptr))
			{
				qCInfo(winShellController) << "Privileges elevated";
				return;
			}
		}
	}

	qCWarning(winShellController) << "Privilege elevation failed";
}

void WinShellController::cleanup()
{
	m_taskBarWindow->show();
}

void WinShellController::toggleTray(QPointF position)
{
	HWND hwndButton = m_taskBarWindow->getTrayButtonHwnd();
	if(hwndButton)
	{
		SendMessage(hwndButton, BM_CLICK , 0, 0);
		m_trayWindow->setPosition(position.toPoint());
	}
}

void WinShellController::shutdown()
{
	qCInfo(winShellController) << "Triggering Shutdown";
	ExitWindowsEx(EWX_SHUTDOWN, 0);
}

void WinShellController::reboot()
{
	qCInfo(winShellController) << "Triggering Reboot";
	ExitWindowsEx(EWX_REBOOT, 0);
}

void WinShellController::sleep()
{
	qCInfo(winShellController) << "Triggering Sleep";
	SetSuspendState(true, false, false);
}

void WinShellController::logout()
{
	qCInfo(winShellController) << "Logging out";
	ExitWindowsEx(EWX_LOGOFF, 0);
}
