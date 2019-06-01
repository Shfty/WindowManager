#include "WinShellController/TrayWindow.h"

#include "Win.h"
#include "WindowController.h"

TrayWindow::TrayWindow(QObject* parent)
	: SystemWindow(parent)
{
	setObjectName("Tray Window");

	m_windowClass = "NotifyIconOverflowWindow";
}
