#include "WinShellController/TrayWindow.h"

#include <QDebug>
Q_LOGGING_CATEGORY(trayWindow, "Tray Window")

#include "Win.h"
//#include "WindowController.h"

TrayWindow::TrayWindow(QObject* parent)
	: SystemWindow(parent)
{
	setObjectName("Tray Window");

	qCInfo(trayWindow, "Startup");

	m_windowClass = "NotifyIconOverflowWindow";
}
