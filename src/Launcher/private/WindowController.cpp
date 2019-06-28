#include "WindowController.h"

#include <QGuiApplication>
#include <QScreen>
#include <QMetaEnum>

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
	QMetaEnum metaLayers = QMetaEnum::fromType<Layer>();
	for(int i = 0; i < metaLayers.keyCount(); ++i)
	{
		m_layers.insert(Layer(i), QMap<HWND, QRect>());
	}
}

void WindowController::registerUnderlayWindow(HWND hwnd)
{
	windowRemoved(hwnd);
	m_layers[Underlay].insert(hwnd, QRect());
}

void WindowController::registerOverlayWindow(HWND hwnd)
{
	windowRemoved(hwnd);
	m_layers[Overlay].insert(hwnd, QRect());
}

void WindowController::windowAdded(HWND window, QString winTitle, QString winClass, QString winProcess, qint32 winStyle)
{
	Q_UNUSED(winTitle);
	Q_UNUSED(winClass);
	Q_UNUSED(winProcess);
	Q_UNUSED(winStyle);

	windowRemoved(window);
	m_layers[Unmanaged].insert(window, QRect());
}

void WindowController::windowRemoved(HWND window)
{
	for(auto& layer : m_layers)
	{
		layer.remove(window);
	}
}

void WindowController::moveWindow(HWND hwnd, QRect geometry, bool visible)
{
	qCInfo(windowController) << "moveWindow" << hwnd << geometry << visible;
	if(visible)
	{
		windowRemoved(hwnd);
		m_layers[Visible].insert(hwnd, geometry);
	}
	else
	{
		windowRemoved(hwnd);
		m_layers[Hidden].insert(hwnd, geometry);
	}
}

void WindowController::commitWindowMove()
{
	HWND insertAfter;

	m_dwp = BeginDeferWindowPos(0);
	{
		insertAfter = HWND_TOP;

		auto layerIt = m_layers.constEnd();
		while(layerIt != m_layers.constBegin())
		{
			--layerIt;

			const QMap<HWND, QRect>& layer = layerIt.value();
			for(HWND hwnd : layer.keys())
			{
				moveWindow_internal(hwnd, insertAfter, layer.value(hwnd));
				insertAfter = hwnd;
			}
		}
	}
	EndDeferWindowPos(m_dwp);

	m_dwp = nullptr;
}

void WindowController::closeWindow(HWND hwnd)
{
	PostMessage(hwnd, WM_CLOSE, 0, 0);
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
}
