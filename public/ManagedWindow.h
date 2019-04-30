#ifndef MANAGEDWINDOW_H
#define MANAGEDWINDOW_H

#include <QtQuick/QQuickItem>
#include <dwmapi.h>

class ManagedWindow : public QQuickItem
{
	Q_OBJECT

	Q_PROPERTY(HWND hwnd MEMBER m_hwnd NOTIFY hwndChanged())
	Q_PROPERTY(HWND parentHwnd MEMBER m_parentHwnd NOTIFY parentHwndChanged())
	Q_PROPERTY(qreal thumbnailOpacity MEMBER m_thumbnailOpacity NOTIFY thumbnailOpacityChanged())

  public:
	ManagedWindow(QQuickItem *parent = nullptr);
	~ManagedWindow() override;

	QSGNode *updatePaintNode(QSGNode* oldNode, QQuickItem::UpdatePaintNodeData* updatePaintNodeData) override;

  signals:
	void hwndChanged();
	void parentHwndChanged();
	void thumbnailOpacityChanged();

  private slots:
	void updateThumbnail();

  private:
	HWND m_hwnd;
	HWND m_parentHwnd;
	qreal m_thumbnailOpacity;

	HTHUMBNAIL m_thumbnail;
};

#endif // MANAGEDWINDOW_H