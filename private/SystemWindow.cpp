#include "SystemWindow.h"

#include "AppCore.h"
#include "WindowController.h"
#include "WindowView.h"

SystemWindow::SystemWindow(QObject* parent)
	: WMObject(parent)
{
	setObjectName("System Window");

	WindowController* wc = getWindowController();
	connect(this, SIGNAL(beginMoveWindows()), wc, SLOT(beginMoveWindows()));
	connect(this, SIGNAL(moveWindow(HWND, QPoint)), wc, SLOT(moveWindow(HWND, QPoint)));
	connect(this, SIGNAL(endMoveWindows()), wc, SLOT(endMoveWindows()));
	connect(this, SIGNAL(showWindow(HWND)), wc, SLOT(showWindow(HWND)));
	connect(this, SIGNAL(hideWindow(HWND)), wc, SLOT(hideWindow(HWND)));
}

SystemWindow::~SystemWindow()
{
}

void SystemWindow::setPosition(QPoint position)
{
	HWND trayIconHwnd = getWindowHwnd();

	emit beginMoveWindows();
	emit moveWindow(trayIconHwnd, position);
	emit endMoveWindows();
}

void SystemWindow::show()
{
	HWND hwnd = getWindowHwnd();

	emit showWindow(hwnd);
}

void SystemWindow::hide()
{
	HWND hwnd = getWindowHwnd();

	emit hideWindow(hwnd);
}

void SystemWindow::toggle()
{
	WindowView* wv = getWindowView();
	HWND trayIconHwnd = getWindowHwnd();

	if(wv->isWindowVisible(trayIconHwnd))
	{
		hide();
	}
	else
	{
		show();
	}
}

WindowController* SystemWindow::getWindowController()
{
	AppCore* appCore = getAppCore();
	return appCore->property("windowController").value<WindowController*>();
}

WindowView* SystemWindow::getWindowView()
{
	AppCore* appCore = getAppCore();
	return appCore->property("windowView").value<WindowView*>();
}

HWND SystemWindow::getWindowHwnd()
{
	WindowView* wv = getWindowView();
	return wv->findWindow(m_windowTitle, m_windowClass);
}
