#include "SystemWindow/TrayIconWindow.h"

#include <QDebug>

#include "Win.h"
#include "WindowManager.h"

TrayIconWindow::TrayIconWindow(QObject* parent)
	: SystemWindow(parent)
{
	m_windowClass = "NotifyIconOverflowWindow";
}

TrayIconWindow::~TrayIconWindow()
{
}
