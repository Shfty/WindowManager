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

	connect(QGuiApplication::instance(), SIGNAL(aboutToQuit()), m_trayIcon, SLOT(hide()));
}

void WinShellController::showTrayIconWindow(QPointF position, QSizeF size)
{
	m_trayWindow->setPosition(position.toPoint(), size.toSize());
	m_trayWindow->toggle();
}
