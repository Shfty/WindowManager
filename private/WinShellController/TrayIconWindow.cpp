#include "WinShellController/TrayWindow.h"

#include <QDebug>

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
