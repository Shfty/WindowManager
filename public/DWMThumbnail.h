#ifndef DWMTHUMBNAIL_H
#define DWMTHUMBNAIL_H

#include <QtQuick/QQuickItem>

#include "Win.h"

class DWMThumbnail : public QQuickItem
{
	Q_OBJECT

	Q_PROPERTY(HWND hwnd MEMBER m_hwnd NOTIFY hwndChanged())
	Q_PROPERTY(QQuickItem* clipTarget MEMBER m_clipTarget NOTIFY clipTargetChanged())

public:
	DWMThumbnail(QQuickItem* parent = nullptr);
	~DWMThumbnail() override;

signals:
	void hwndChanged();
	void thumbnailOpacityChanged();
	void clipTargetChanged();

private slots:
	void updateThumbnail();
	void drawThumbnail();

private:
	HWND m_hwnd;
	QQuickItem* m_clipTarget;

	HTHUMBNAIL m_thumbnail;
};

#endif // DWMTHUMBNAIL_H