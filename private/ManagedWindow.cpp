#include "ManagedWindow.h"

#include <Win.h>
#include <tchar.h>

#include "WindowController.h"

ManagedWindow::ManagedWindow(QQuickItem *parent) : QQuickItem(parent),
												   m_hwnd(nullptr),
												   m_parentHwnd(nullptr),
												   m_thumbnailOpacity(1.0),
												   m_thumbnail(nullptr)
{
	setObjectName("Managed Window");

	connect(this, SIGNAL(hwndChanged()), this, SLOT(updateThumbnail()));
	connect(this, SIGNAL(parentHwndChanged()), this, SLOT(updateThumbnail()));
	
	setFlag(ItemHasContents, true);
}

ManagedWindow::~ManagedWindow()
{
	if (m_thumbnail != nullptr)
	{
		DwmUnregisterThumbnail(m_thumbnail);
	}
}

QSGNode *ManagedWindow::updatePaintNode(QSGNode *oldNode, QQuickItem::UpdatePaintNodeData *updatePaintNodeData)
{
	drawThumbnail();

	// Schedule a repaint
	update();

	return QQuickItem::updatePaintNode(oldNode, updatePaintNodeData);
}

void ManagedWindow::updateThumbnail()
{
	if (m_thumbnail != nullptr)
	{
		DwmUnregisterThumbnail(m_thumbnail);
		m_thumbnail = nullptr;
	}

	if (m_parentHwnd != nullptr && m_hwnd != nullptr)
	{
		DwmRegisterThumbnail(m_parentHwnd, m_hwnd, &m_thumbnail);
	}
}

void ManagedWindow::drawThumbnail()
{
	if(m_thumbnail == nullptr)
	{
		return;
	}

	// Calculate source / dest rectangles
	RECT winRect;
	GetWindowRect(m_hwnd, &winRect);

	QPointF sourcePos = QPointF(0, 0);
	QSizeF sourceSize = QSizeF(winRect.right - winRect.left, winRect.bottom - winRect.top);
	QPointF destPos = mapToScene(QPointF(0, 0));
	QSizeF destSize = QSizeF(width(), height());

/*
	// Non-scaling source / dest rectangles
	QSizeF delta = destSize - sourceSize;
	QSizeF halfDelta = delta * 0.5f;

	if(halfDelta.width() > 0)
	{
		destPos.setX(destPos.x() + halfDelta.width());
	}

	if(halfDelta.height() > 0)
	{
		destPos.setY(destPos.y() + halfDelta.height());
	}

	if(halfDelta.width() < 0)
	{
		sourcePos.setX(sourcePos.x() - halfDelta.width());
	}

	if(halfDelta.height() < 0)
	{
		sourcePos.setY(sourcePos.y() - halfDelta.height());
	}

	sourceSize.setWidth(qMin(sourceSize.width(), destSize.width()));
	sourceSize.setHeight(qMin(sourceSize.height(), destSize.height()));

	destSize.setWidth(qMin(sourceSize.width(), destSize.width()));
	destSize.setHeight(qMin(sourceSize.height(), destSize.height()));
*/

	QRectF sourceRect = QRectF(sourcePos, sourceSize);
	QRectF destRect = QRectF(destPos, destSize);

	if(m_clipTarget != nullptr)
	{
		QPointF clipTargetPos = m_clipTarget->mapToScene(QPointF(0, 0));
		QRectF clippingRect = QRectF(clipTargetPos, m_clipTarget->size());

		destRect = destRect.intersected(clippingRect);

		QPointF relativePos = clippingRect.topLeft() - destPos;
		QRectF relativeClipRect = QRectF(relativePos, clippingRect.size());
		
		sourceRect = sourceRect.intersected(relativeClipRect);
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
	dskThumbProps.opacity = static_cast<unsigned char>(m_thumbnailOpacity * 255.0);
	dskThumbProps.rcSource = source;
	dskThumbProps.rcDestination = dest;

	// Display the thumbnail
	DwmUpdateThumbnailProperties(m_thumbnail, &dskThumbProps);
}
