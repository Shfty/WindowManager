#include "WinShellController.h"

#include <QGuiApplication>

#include "WinShellController/TaskBarWindow.h"
#include "WinShellController/TrayIcon.h"
#include "WinShellController/TrayWindow.h"

WinShellController::WinShellController(QObject* parent)
	: WMObject(parent)
	, m_trayIcon(new TrayIcon(this))
	, m_trayWindow(new TrayWindow(this))
	, m_taskBarWindow(new TaskBarWindow(this))
{
	qRegisterMetaType<WinShellController*>();
	setObjectName("Windows Shell Controller");

	m_trayIcon->show();
	m_taskBarWindow->hide();
}

void WinShellController::cleanup()
{
	m_trayIcon->hide();
	m_taskBarWindow->show();
}

void WinShellController::toggleTrayIconWindow(QPointF position)
{
	HWND hwndButton = m_taskBarWindow->getTrayButtonHwnd();
	if(hwndButton)
	{
		SendMessage(hwndButton, BM_CLICK , 0, 0);
		m_trayWindow->setPosition(position.toPoint());
	}
}
