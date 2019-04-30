#include "SystemWindow/TaskBarWindow.h"

#include <QDebug>

TaskBarWindow::TaskBarWindow(QObject *parent)
	: SystemWindow(parent)
{
	m_windowClass = "Shell_TrayWnd";
	this->hide();
}

TaskBarWindow::~TaskBarWindow()
{
	this->show();
}
