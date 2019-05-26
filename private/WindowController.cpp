#include "WindowController.h"

#include "Win.h"

WindowController::WindowController(QObject* parent)
	: QObject(parent)
	, m_dwp(nullptr)
{
	qRegisterMetaType<WindowController*>();
	setObjectName("Window Controller");
}

WindowController::~WindowController()
{

}

void WindowController::beginMoveWindows()
{
	m_dwp = BeginDeferWindowPos(0);
}

void WindowController::moveWindow(HWND hwnd, QPoint position, QSize size, qlonglong layer)
{
	Q_ASSERT_X(hwnd != nullptr, "moveWindow", "moveWindow called with invalid HWND");
	Q_ASSERT_X(m_dwp != nullptr, "moveWindow", "moveWindow called before beginMoveWindows");

	UINT flags = SWP_NOACTIVATE;
	if(size.isEmpty())
	{
		flags |= SWP_NOSIZE;
	}

	DeferWindowPos(
		m_dwp,
		hwnd,
		reinterpret_cast<HWND>(layer),
		position.x(),
		position.y(),
		size.width(),
		size.height(),
		flags
	);
}

void WindowController::endMoveWindows()
{
	EndDeferWindowPos(m_dwp);
	m_dwp = nullptr;
}

void WindowController::showWindow(HWND hwnd)
{
	Q_ASSERT_X(hwnd != nullptr, "showWindow", "showWindow called with invalid HWND");

	ShowWindow(hwnd, SW_SHOW);
}

void WindowController::hideWindow(HWND hwnd)
{
	Q_ASSERT_X(hwnd != nullptr, "hideWindow", "hideWindow called with invalid HWND");

	ShowWindow(hwnd, SW_HIDE);
}
