#include "DWMThumbnail.h"

#include <tchar.h>
#include <QQuickWindow>
#include <QScreen>

#include "WindowController.h"

DWMThumbnail::DWMThumbnail(QQuickItem *parent) : QQuickItem(parent),
												   m_hwnd(nullptr),
												   m_clipTarget(nullptr),
												   m_thumbnail(nullptr)
{
	setObjectName("Managed Window");

	connect(this, SIGNAL(hwndChanged()), this, SLOT(updateThumbnail()));
	connect(this, SIGNAL(windowChanged(QQuickWindow*)), this, SLOT(updateThumbnail()));
}

DWMThumbnail::~DWMThumbnail()
{
	if (m_thumbnail != nullptr)
	{
		DwmUnregisterThumbnail(m_thumbnail);
	}
}

void DWMThumbnail::updateThumbnail()
{
	if (m_thumbnail != nullptr)
	{
		DwmUnregisterThumbnail(m_thumbnail);
		m_thumbnail = nullptr;
	}

	QQuickWindow* parentWindow = window();
	if(parentWindow == nullptr) return;

	HWND parentHwnd = reinterpret_cast<HWND>(parentWindow->winId());
	if (parentHwnd == nullptr) return;

	if(m_hwnd == nullptr) return;

	DwmRegisterThumbnail(parentHwnd, m_hwnd, &m_thumbnail);
	QObject::disconnect(this, SLOT(drawThumbnail()));
	QObject::connect(parentWindow, SIGNAL(frameSwapped()), this, SLOT(drawThumbnail()));
	parentWindow->update();
}

void DWMThumbnail::drawThumbnail()
{
	if(m_thumbnail == nullptr) return;
	if(m_hwnd == nullptr) return;

	// Calculate source / dest rectangles
	RECT winRect;
	GetWindowRect(m_hwnd, &winRect);

	qreal winWidth = winRect.right - winRect.left;
	qreal winHeight = winRect.bottom - winRect.top;

	QPoint sourcePos = QPoint(0, 0);
	QSizeF sourceSize = QSizeF(winWidth, winHeight);

	QPointF destPos = mapToScene(QPointF(0, 0));
	QSizeF destSize = QSizeF(width(), height());

	QRectF sourceRect = QRectF(sourcePos, sourceSize);
	QRectF destRect = QRectF(destPos, destSize);

	if(m_clipTarget != nullptr)
	{
		QPointF clipTargetPos = m_clipTarget->mapToScene(QPointF(0, 0));
		QSizeF clipTargetSize = m_clipTarget->size();
		QRectF clippingRect = QRectF(clipTargetPos, clipTargetSize);

		destRect = destRect.intersected(clippingRect);

		QPointF relativePos = clippingRect.topLeft() - destPos;
		QRectF relativeClipRect = QRectF(relativePos, clippingRect.size());

		sourceRect = sourceRect.intersected(relativeClipRect);
	}

	QQuickWindow* parentWindow = window();
	QScreen* parentScreen = parentWindow->screen();
	qreal dpr = parentScreen->devicePixelRatio();

	sourceRect.setRect(
		sourceRect.x() * dpr,
		sourceRect.y() * dpr,
		sourceRect.width() * dpr,
		sourceRect.height() * dpr
	);

	destRect.setRect(
		destRect.x() * dpr,
		destRect.y() * dpr,
		destRect.width() * dpr,
		destRect.height() * dpr
	);

	// Account for extended margins
	RECT extendedFrame;
	DwmGetWindowAttribute(m_hwnd, DWMWA_EXTENDED_FRAME_BOUNDS, &extendedFrame, sizeof(RECT));

	QMargins extendedMargins = QMargins(
		extendedFrame.left - winRect.left,
		extendedFrame.top - winRect.top,
		extendedFrame.right - winRect.right,
		extendedFrame.bottom - winRect.bottom
	);

	if(m_clipTarget == nullptr)
	{
		sourceRect.adjust(extendedMargins.left(),
						  extendedMargins.top(),
						  extendedMargins.right(),
						  extendedMargins.bottom());
	}
	else
	{
		sourceRect.adjust(extendedMargins.left(),
						  extendedMargins.top(),
						  extendedMargins.left(),
						  extendedMargins.top());
	}

	// Convert rectangles into winapi format
	RECT source = {
		static_cast<LONG>(sourceRect.left()),
		static_cast<LONG>(sourceRect.top()),
		static_cast<LONG>(sourceRect.right()),
		static_cast<LONG>(sourceRect.bottom())
	};

	RECT dest = {
		static_cast<LONG>(destRect.left()),
		static_cast<LONG>(destRect.top()),
		static_cast<LONG>(destRect.right()),
		static_cast<LONG>(destRect.bottom())
	};

	// Set the thumbnail properties for use
	DWM_THUMBNAIL_PROPERTIES dskThumbProps;
	dskThumbProps.dwFlags = DWM_TNP_RECTSOURCE | DWM_TNP_RECTDESTINATION | DWM_TNP_VISIBLE | DWM_TNP_SOURCECLIENTAREAONLY | DWM_TNP_OPACITY;

	// Use the window frame and client area
	dskThumbProps.fSourceClientAreaOnly = FALSE;
	dskThumbProps.fVisible = property("visible").toBool();

	// TODO: accumulated opacity
	dskThumbProps.opacity = static_cast<unsigned char>(opacity() * 255.0);
	dskThumbProps.rcSource = source;
	dskThumbProps.rcDestination = dest;

	// Display the thumbnail
	DwmUpdateThumbnailProperties(m_thumbnail, &dskThumbProps);
}
