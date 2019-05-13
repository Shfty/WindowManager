#include "WinShellController/TrayWindow.h"

#include "Win.h"
#include "WindowController.h"

TrayWindow::TrayWindow(QObject* parent)
	: SystemWindow(parent)
{
	m_windowClass = "NotifyIconOverflowWindow";
}

TrayWindow::~TrayWindow()
{
}
