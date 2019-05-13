#include "WinShellController/TaskBarWindow.h"

TaskBarWindow::TaskBarWindow(QObject* parent)
	: SystemWindow(parent)
{
	setObjectName("TaskBar Window");

	m_windowClass = "Shell_TrayWnd";
	hide();
}

TaskBarWindow::~TaskBarWindow()
{
	show();
}
