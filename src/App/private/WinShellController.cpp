#include "WinShellController.h"

#include <QGuiApplication>

#include "WinShellController/TaskBarWindow.h"
#include "WinShellController/TrayWindow.h"

WinShellController::WinShellController(QObject* parent)
	: WMObject(parent)
	, m_trayWindow(new TrayWindow(this))
	, m_taskBarWindow(new TaskBarWindow(this))
{
	qRegisterMetaType<WinShellController*>();
	setObjectName("Windows Shell Controller");

	qInfo("Startup");

	m_taskBarWindow->hide();
}

void WinShellController::cleanup()
{
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
