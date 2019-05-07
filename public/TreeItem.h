#ifndef TREEITEM_H
#define TREEITEM_H

#include <QObject>

#include "Win.h"
#include <QRect>
#include <QString>
#include <QVariant>

class QScreen;
class QQuickWindow;
class QQuickView;

class TreeItem : public QObject
{
	Q_OBJECT

	Q_PROPERTY(TreeItem* parent READ getTreeParent() NOTIFY parentChanged)
	Q_PROPERTY(QList<QObject*> children READ getTreeChildren() NOTIFY childrenChanged)
	Q_PROPERTY(int activeIndex READ getActiveIndex() NOTIFY activeIndexChanged)

	Q_PROPERTY(QString flow MEMBER m_flow NOTIFY flowChanged)
	Q_PROPERTY(QString layout MEMBER m_layout NOTIFY layoutChanged)
	Q_PROPERTY(int index READ getIndex() NOTIFY indexChanged)
	Q_PROPERTY(int depth READ getDepth() NOTIFY depthChanged)
	Q_PROPERTY(QScreen* monitor READ getMonitor() NOTIFY monitorChanged)
	Q_PROPERTY(QRectF bounds READ calculateBounds() NOTIFY boundsChanged)
	Q_PROPERTY(QRectF headerBounds READ calculateHeaderBounds() NOTIFY headerBoundsChanged)
	Q_PROPERTY(QRectF contentBounds READ calculateContentBounds() NOTIFY contentBoundsChanged)
	Q_PROPERTY(HWND hwnd MEMBER m_hwnd NOTIFY hwndChanged)
	Q_PROPERTY(QString launchUri MEMBER m_launchUri NOTIFY launchUriChanged)
	Q_PROPERTY(QString launchParams MEMBER m_launchParams NOTIFY launchParamsChanged)
	Q_PROPERTY(bool isAnimating MEMBER m_isAnimating NOTIFY isAnimatingChanged)

public:
	explicit TreeItem(QObject* parent = nullptr);
	~TreeItem();

	Q_INVOKABLE int getActiveIndex();
	Q_INVOKABLE void setActiveChild(TreeItem* activeChild);
	Q_INVOKABLE void setActive();

	Q_INVOKABLE int getIndex();
	Q_INVOKABLE int getDepth();
	Q_INVOKABLE bool getIsVisible();

	Q_INVOKABLE QScreen* getMonitor();

	Q_INVOKABLE QRectF calculateBounds();
	Q_INVOKABLE QRectF calculateHeaderBounds();
	Q_INVOKABLE QRectF calculateContentBounds();

	Q_INVOKABLE bool isHwndValid();

	Q_INVOKABLE void toggleFlow();
	Q_INVOKABLE void toggleLayout();
	Q_INVOKABLE QVariant addChild(QString title = "", QString flow = "", QString layout = "", int monitorIndex = -1, HWND hwnd = nullptr);
	Q_INVOKABLE void resetHwnd();
	Q_INVOKABLE void moveUp();
	Q_INVOKABLE void moveDown();
	Q_INVOKABLE void remove();

	QPointF getScreenPosition();

	TreeItem* getTreeParent() const;
	QObjectList getTreeChildren() const;

	void moveChild(TreeItem* child, int delta);

	virtual void childEvent(QChildEvent* event) override;

	QDataStream& serialize(QDataStream& out) const;
	QDataStream& deserialize(QDataStream& in);

signals:
	void parentChanged();
	void childrenChanged();
	void childAdded(int index, TreeItem* child);
	void childRemoved(int index, TreeItem* child);
	void activeIndexChanged();

	void flowChanged();
	void layoutChanged();
	void indexChanged();
	void depthChanged();
	void boundsChanged();
	void headerBoundsChanged();
	void contentBoundsChanged();
	void monitorChanged();
	void hwndChanged();
	void launchUriChanged();
	void launchParamsChanged();

	void isAnimatingChanged();

public slots:
	void moveWindowOnscreen();
	void moveWindowOffscreen();
	void launch();

private slots:
	void moveWindowOnscreen_Internal();
	void moveWindowOffscreen_Internal();
	void handleMonitorBoundsChanged();
	void handleAnimatingChanged();

private:
	// Serialized properties
	QString m_flow;
	QString m_layout;
	int m_monitorIndex;
	HWND m_hwnd;
	QString m_launchUri;
	QString m_launchParams;

	QQuickWindow* m_itemWindow;
	QQuickWindow* m_headerWindow;
	QQuickWindow* m_overlayWindow;

	QList<TreeItem*> m_children;

	// Trasient properties
	TreeItem* m_activeChild;
	bool m_isAnimating;
};

QDataStream& operator<<(QDataStream& out, const TreeItem* nestedItem);
QDataStream& operator>>(QDataStream& in, TreeItem* nestedItem);

Q_DECLARE_METATYPE(TreeItem*)
Q_DECLARE_METATYPE(const TreeItem*)

#endif // TREEITEM_H
