#include "WindowController.h"

#include <QGuiApplication>
#include <QScreen>

#include <QDebug>
Q_LOGGING_CATEGORY(windowController, "launcher.windowController")

#include <Win.h>

WindowController::WindowController(QObject* parent)
	: QObject(parent)
	, m_dwp(nullptr)
{
	qRegisterMetaType<WindowController*>();
	setObjectName("Window Controller");
}

void WindowController::startup()
{
}

void WindowController::registerUnderlayWindow(HWND hwnd)
{
	m_underlayWindows.insert(hwnd, QRect());
}

void WindowController::registerOverlayWindow(HWND hwnd)
{
	m_overlayWindows.insert(hwnd, QRect());
}

void WindowController::windowAdded(HWND window, QString winTitle, QString winClass, QString winProcess, qint32 winStyle)
{
	Q_UNUSED(winTitle);
	Q_UNUSED(winClass);
	Q_UNUSED(winProcess);
	Q_UNUSED(winStyle);

	m_hiddenWindows.remove(window);
	m_underlayWindows.remove(window);
	m_visibleWindows.remove(window);
	m_overlayWindows.remove(window);

	m_overlayWindows.insert(window, QRect());
}

void WindowController::windowRemoved(HWND window)
{
	m_hiddenWindows.remove(window);
	m_underlayWindows.remove(window);
	m_visibleWindows.remove(window);
	m_overlayWindows.remove(window);
}

void WindowController::moveWindow(HWND hwnd, QRect geometry, bool visible)
{
	qCInfo(windowController) << "moveWindow" << hwnd << geometry << visible;
	if(visible)
	{
		m_underlayWindows.remove(hwnd);
		m_hiddenWindows.remove(hwnd);
		m_overlayWindows.remove(hwnd);

		m_visibleWindows.insert(hwnd, geometry);
	}
	else
	{
		m_underlayWindows.remove(hwnd);
		m_visibleWindows.remove(hwnd);
		m_overlayWindows.remove(hwnd);

		m_hiddenWindows.insert(hwnd, geometry);
	}
}

void WindowController::endMoveWindows()
{
	HWND insertAfter;

	m_dwp = BeginDeferWindowPos(0);
	{
		insertAfter = HWND_TOP;

		qCInfo(windowController) << "Deferring overlay windows";
		for(HWND hwnd : m_overlayWindows.keys())
		{
			moveWindow_internal(hwnd, insertAfter, m_overlayWindows[hwnd]);
			insertAfter = hwnd;
		}

		qCInfo(windowController) << "Deferring visible windows";
		for(HWND hwnd : m_visibleWindows.keys())
		{
			moveWindow_internal(hwnd, insertAfter, m_visibleWindows[hwnd]);
			insertAfter = hwnd;
		}

		qCInfo(windowController) << "Deferring underlay windows";
		for(HWND hwnd : m_underlayWindows.keys())
		{
			moveWindow_internal(hwnd, insertAfter, m_underlayWindows[hwnd]);
			insertAfter = hwnd;
		}

		qCInfo(windowController) << "Deferring hidden windows";
		for(HWND hwnd : m_hiddenWindows.keys())
		{
			moveWindow_internal(hwnd, insertAfter, m_hiddenWindows[hwnd]);
			insertAfter = hwnd;
		}

	}
	EndDeferWindowPos(m_dwp);

	m_dwp = nullptr;
}

void WindowController::setWindowStyle(HWND hwnd, qint32 style)
{
	SetWindowLong(hwnd, GWL_STYLE, style);
	SetWindowPos(hwnd, hwnd, NULL, NULL, NULL, NULL, SWP_NOMOVE | SWP_NOSIZE | SWP_NOZORDER | SWP_NOACTIVATE | SWP_FRAMECHANGED);
}

void WindowController::moveWindow_internal(HWND hwnd, HWND insertAfter, QRect geometry)
{
	Q_ASSERT_X(hwnd != nullptr, "moveWindow", "moveWindow called with invalid HWND");
	Q_ASSERT_X(m_dwp != nullptr, "moveWindow", "m_dwp nullptr before call to DeferWindowPos");

	uint flags = SWP_NOACTIVATE;

	if(geometry.isEmpty())
	{
		flags |= SWP_NOMOVE;
		flags |= SWP_NOSIZE;
	}

	m_dwp = DeferWindowPos(
		m_dwp,
		hwnd,
		insertAfter,
		geometry.x(),
		geometry.y(),
		geometry.width(),
		geometry.height(),
		flags
	);

	Q_ASSERT_X(m_dwp != nullptr, "moveWindow_internal", "m_dwp nullptr after call to DeferWindowPos");

	qCInfo(windowController) << "Deferring window pos for" << hwnd << "above" << insertAfter << "with geometry" << geometry;
}
