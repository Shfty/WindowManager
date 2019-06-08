#include "WinShellController/TaskBarWindow.h"

#include <QGuiApplication>
#include <QDebug>

TaskBarWindow::TaskBarWindow(QObject* parent)
	: SystemWindow(parent)
{
	setObjectName("TaskBar Window");

	m_windowClass = "Shell_TrayWnd";
}

HWND TaskBarWindow::getTrayButtonHwnd()
{
	HWND hwnd = getWindowHwnd();
	if(hwnd != nullptr)
	{
		HWND hwndTray = FindWindowEx(hwnd, nullptr, L"TrayNotifyWnd", nullptr);
		if(hwndTray)
		{
			return FindWindowEx(hwndTray, nullptr, L"Button", nullptr);
		}
	}
}

