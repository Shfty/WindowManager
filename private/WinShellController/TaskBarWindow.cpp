#include "WinShellController/TaskBarWindow.h"

#include <QDebug>

TaskBarWindow::TaskBarWindow(QObject* parent)
	: SystemWindow(parent)
{
	m_windowClass = "Shell_TrayWnd";
	hide();
}

TaskBarWindow::~TaskBarWindow()
{
	show();
}
