#include "SystemWindow.h"

#include <QDebug>

#include "Win.h"
#include "WindowManager.h"

SystemWindow::SystemWindow(QObject *parent)
	: QObject(parent)
{
}

SystemWindow::~SystemWindow()
{
}

void SystemWindow::setPosition(QPoint position)
{
	HWND trayIconHwnd = this->getWindowHwnd();
	WindowManager &wm = WindowManager::instance();

	wm.beginMoveWindows();
	wm.moveWindow(trayIconHwnd, position);
	wm.endMoveWindows();
}

void SystemWindow::show()
{
	HWND trayIconHwnd = this->getWindowHwnd();
	WindowManager &wm = WindowManager::instance();
	wm.showWindow(trayIconHwnd);
}

void SystemWindow::hide()
{
	HWND trayIconHwnd = this->getWindowHwnd();
	WindowManager &wm = WindowManager::instance();
	wm.hideWindow(trayIconHwnd);
}

void SystemWindow::toggle()
{
	HWND trayIconHwnd = this->getWindowHwnd();
	WindowManager &wm = WindowManager::instance();

	if (wm.isWindowVisible(trayIconHwnd))
	{
		this->hide();
	}
	else
	{
		this->show();
	}
}

HWND SystemWindow::getWindowHwnd()
{
	WindowManager &wm = WindowManager::instance();
	return wm.findWindow(m_windowTitle, m_windowClass);
}
