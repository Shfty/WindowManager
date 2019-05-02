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
	Q_PROPERTY(QQuickItem* clipTarget MEMBER m_clipTarget NOTIFY clipTargetChanged())

  public:
	ManagedWindow(QQuickItem *parent = nullptr);
	~ManagedWindow() override;

	QSGNode *updatePaintNode(QSGNode* oldNode, QQuickItem::UpdatePaintNodeData* updatePaintNodeData) override;

  signals:
	void hwndChanged();
	void parentHwndChanged();
	void thumbnailOpacityChanged();
	void clipTargetChanged();

  private slots:
	void updateThumbnail();
	void drawThumbnail();

  private:
	HWND m_hwnd;
	HWND m_parentHwnd;
	qreal m_thumbnailOpacity;
	QQuickItem* m_clipTarget;

	HTHUMBNAIL m_thumbnail;
};

#endif // MANAGEDWINDOW_H