#include "SystemWindow.h"

#include <QDebug>

#include "AppCore.h"
#include "WindowController.h"
#include "WindowView.h"
#include "Win.h"

SystemWindow::SystemWindow(QObject* parent)
	: WMObject(parent)
{
	setObjectName("System Window");
}

SystemWindow::~SystemWindow()
{
}

void SystemWindow::setPosition(QPoint position)
{
	WindowController* wc = getWindowController();
	HWND trayIconHwnd = getWindowHwnd();

	wc->beginMoveWindows();
	wc->moveWindow(trayIconHwnd, position);
	wc->endMoveWindows();
}

void SystemWindow::show()
{
	WindowController* wc = getWindowController();
	HWND trayIconHwnd = getWindowHwnd();

	wc->showWindow(trayIconHwnd);
}

void SystemWindow::hide()
{
	WindowController* wc = getWindowController();
	HWND trayIconHwnd = getWindowHwnd();

	wc->hideWindow(trayIconHwnd);
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
