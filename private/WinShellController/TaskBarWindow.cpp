#include "WinShellController/TaskBarWindow.h"

#include <QGuiApplication>

TaskBarWindow::TaskBarWindow(QObject* parent)
	: SystemWindow(parent)
{
	setObjectName("TaskBar Window");

	m_windowClass = "Shell_TrayWnd";
	hide();

	connect(QGuiApplication::instance(), &QGuiApplication::aboutToQuit, [=]() {
		show();
	});
}
