#ifndef DWMTHUMBNAIL_H
#define DWMTHUMBNAIL_H

#include <QtQuick/QQuickItem>

#include <QLoggingCategory>
Q_DECLARE_LOGGING_CATEGORY(dwmThumbnail);

#include <Win.h>

class DWMThumbnail : public QQuickItem
{
	Q_OBJECT

	Q_PROPERTY(HWND hwnd MEMBER m_hwnd NOTIFY hwndChanged())

public:
	DWMThumbnail(QQuickItem* parent = nullptr);
	~DWMThumbnail() override;

signals:
	void hwndChanged();
	void thumbnailOpacityChanged();

private slots:
	void updateThumbnail();
	void drawThumbnail();

private:
	HWND m_hwnd;

	HTHUMBNAIL m_thumbnail;
};

#endif // DWMTHUMBNAIL_H