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
		m_layers.insert(Layer(i), QMap<WindowInfo, QRect>());
	}
}

void WindowController::registerUnderlayWindow(WindowInfo wi)
{
	removeHwnd(wi.hwnd);
	m_layers[Underlay].insert(wi, QRect());
	updateWindowLayout();
}

void WindowController::registerOverlayWindow(WindowInfo wi)
{
	removeHwnd(wi.hwnd);
	m_layers[Overlay].insert(wi, QRect());
	updateWindowLayout();
}

void WindowController::windowCreated(WindowInfo wi)
{
	qCInfo(windowController) << "windowCreated" << wi;

	removeHwnd(wi.hwnd);
	m_layers[Unmanaged].insert(wi, QRect());
	//updateWindowLayout();
}

void WindowController::windowDestroyed(HWND hwnd)
{
	qCInfo(windowController) << "windowDestroyed" << hwnd;

	removeHwnd(hwnd);
	//updateWindowLayout();
}

void WindowController::moveWindow(HWND hwnd, QRect geometry, bool visible)
{
	qCInfo(windowController) << "moveWindow" << hwnd << geometry << visible;

	// Calculate extended frame
	RECT winRect;
	GetWindowRect(hwnd, &winRect);

	RECT extendedFrame;
	DwmGetWindowAttribute(hwnd, DWMWA_EXTENDED_FRAME_BOUNDS, &extendedFrame, sizeof(RECT));

	QMargins extendedMargins = QMargins(
		extendedFrame.left - winRect.left,
		extendedFrame.top - winRect.top,
		winRect.right - extendedFrame.right,
		winRect.bottom - extendedFrame.bottom
	);

	// Apply extended frame
	geometry.adjust(-extendedMargins.left(), -extendedMargins.top(), extendedMargins.right(), extendedMargins.bottom());

	WindowInfo wi = findWindowInfo(hwnd);
	removeHwnd(hwnd);
	m_layers[visible ? Visible : Hidden].insert(wi, geometry);

	SetWindowPos(hwnd, HWND_NOTOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);
}

void WindowController::updateWindowLayout()
{
	qCInfo(windowController) << "updateWindowLayout";

	m_dwp = BeginDeferWindowPos(0);
	Q_ASSERT_X(m_dwp != nullptr, "moveWindow", "BeginDeferWindowPos failed");

	HWND insertAfter = HWND_NOTOPMOST;

	auto layerIt = m_layers.constEnd();
	while(layerIt != m_layers.constBegin())
	{
		--layerIt;

		const QMap<WindowInfo, QRect>& layer = layerIt.value();
		for(WindowInfo wi : layer.keys())
		{
			moveWindow_internal(layerIt.key(), wi, insertAfter, layer.value(wi));
			insertAfter = wi.hwnd;
		}
	}

	bool result = EndDeferWindowPos(m_dwp);
	Q_ASSERT_X(result, "moveWindow", "EndDeferWindowPos failed");

	qCInfo(windowController) << "updateWindowLayout Complete";

	m_dwp = nullptr;
}

void WindowController::setActiveWindow(HWND hwnd)
{
	SetActiveWindow(hwnd);
}

void WindowController::closeWindow(HWND hwnd)
{
	PostMessage(hwnd, WM_CLOSE, 0, 0);
}

void WindowController::setWindowStyle(HWND hwnd, qint32 style)
{
	quint32 flags = SWP_NOMOVE | SWP_NOSIZE | SWP_NOZORDER | SWP_NOACTIVATE | SWP_FRAMECHANGED;

	SetWindowLong(hwnd, GWL_STYLE, style);
	SetWindowPos(hwnd, hwnd, NULL, NULL, NULL, NULL, flags);
}

WindowInfo WindowController::findWindowInfo(HWND hwnd)
{
	for(auto& layer : m_layers)
	{
		for(auto& entry : layer.keys())
		{
			if(entry.hwnd == hwnd)
			{
				return entry;
			}
		}
	}

	return WindowInfo();
}

void WindowController::removeHwnd(HWND hwnd)
{
	for(auto& layer : m_layers)
	{
		for(auto& entry : layer.keys())
		{
			if(entry.hwnd == hwnd)
			{
				layer.remove(entry);
				break;
			}
		}
	}
}

void WindowController::moveWindow_internal(Layer layer, WindowInfo wi, HWND insertAfter, QRect geometry)
{
	Q_ASSERT_X(wi.hwnd != nullptr, "moveWindow", "moveWindow called with invalid HWND");
	Q_ASSERT_X(m_dwp != nullptr, "moveWindow", "m_dwp nullptr before call to DeferWindowPos");

	qCInfo(windowController) << "Deferring window position for" << layer << "window" << wi << " after " << insertAfter << " at " << geometry;

	uint flags = SWP_NOACTIVATE;

	if(geometry.isEmpty())
	{
		flags |= SWP_NOMOVE;
		flags |= SWP_NOSIZE;
	}

	m_dwp = DeferWindowPos(
		m_dwp,
		wi.hwnd,
		insertAfter,
		geometry.x(),
		geometry.y(),
		geometry.width(),
		geometry.height(),
		flags
	);

	QString errorString;
	QTextStream ts(&errorString);
	ts << "m_dwp nullptr after call to DeferWindowPos with WindowInfo " << wi << ", error: " << QString::number(GetLastError());
	Q_ASSERT_X(m_dwp != nullptr, "moveWindow_internal", errorString.toStdString().c_str());
}
