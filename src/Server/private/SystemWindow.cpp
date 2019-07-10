#include "SystemWindow.h"

#include <QDebug>
Q_LOGGING_CATEGORY(systemWindow, "launcher.systemWindow")

#include "Win.h"

SystemWindow::SystemWindow(QObject* parent)
	: QObject(parent)
{
	setObjectName("System Window");

	qCInfo(systemWindow, "Startup");
}

void SystemWindow::setPosition(QPoint position, QSize size)
{
	MoveWindow(
		getWindowHwnd(),
		position.x(), position.y(),
		size.width(), size.height(),
		reinterpret_cast<qlonglong>(HWND_TOP)
	);
}

void SystemWindow::show()
{
	ShowWindow(getWindowHwnd(), SW_SHOW);
}

void SystemWindow::hide()
{
	ShowWindow(getWindowHwnd(), SW_HIDE);
}

#include <QDebug>
void SystemWindow::toggle()
{
	HWND trayIconHwnd = getWindowHwnd();

	RECT rect;
	GetWindowRect(trayIconHwnd, &rect);

	if(IsWindowVisible(trayIconHwnd))
	{
		hide();
	}
	else
	{
		show();
	}
}

HWND SystemWindow::getWindowHwnd()
{
	return FindWindowEx(
		nullptr,
		nullptr,
		reinterpret_cast<LPCWSTR>(m_windowClass.utf16()),
		reinterpret_cast<LPCWSTR>(m_windowTitle.utf16())
	);
}
