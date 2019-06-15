#include "WinShellController/TrayWindow.h"

#include "Win.h"
#include "WindowController.h"

TrayWindow::TrayWindow(QObject* parent)
	: SystemWindow(parent)
{
	setObjectName("Tray Window");

	qInfo("Startup");

	m_windowClass = "NotifyIconOverflowWindow";
}
