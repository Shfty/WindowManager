#ifndef TREEITEM_H
#define TREEITEM_H

#include <QObject>

#include <QRect>
#include <QPoint>
#include <QString>
#include <QVariant>
#include <QJsonObject>

#include <QLoggingCategory>
Q_DECLARE_LOGGING_CATEGORY(treeItem);

#include <Win.h>

class AppCore;
class SettingsContainer;
class WindowController;
class WindowView;
class QScreen;
class QQuickWindow;
class QQuickView;
class WindowInfo;

class TreeItem : public QObject
{
	Q_OBJECT

	Q_PROPERTY(TreeItem* treeParent READ getTreeParent() NOTIFY parentChanged)
	Q_PROPERTY(QList<QObject*> children READ getTreeChildren() NOTIFY childrenChanged)

	Q_PROPERTY(int activeIndex READ getActiveIndex() NOTIFY activeIndexChanged)

	Q_PROPERTY(QString flow MEMBER m_flow NOTIFY flowChanged)
	Q_PROPERTY(QString layout MEMBER m_layout NOTIFY layoutChanged)

	Q_PROPERTY(int index READ getIndex() NOTIFY indexChanged)
	Q_PROPERTY(int depth READ getDepth() NOTIFY depthChanged)

	Q_PROPERTY(QRectF bounds READ getBoundsLocal() NOTIFY boundsChanged)
	Q_PROPERTY(QRectF contentBounds READ getContentBoundsLocal() NOTIFY contentBoundsChanged)

	Q_PROPERTY(QRectF headerBounds READ getHeaderBoundsLocal() NOTIFY headerBoundsChanged)
	Q_PROPERTY(QRectF nodeBounds READ getNodeBoundsLocal() NOTIFY nodeBoundsChanged)

	Q_PROPERTY(WindowInfo* windowInfo READ getWindowInfo WRITE setWindowInfo NOTIFY windowInfoChanged)

	// Node Options
	Q_PROPERTY(bool borderless MEMBER m_borderless NOTIFY borderlessChanged)

	// Launch Options
	Q_PROPERTY(QString launchUri MEMBER m_launchUri NOTIFY launchUriChanged)
	Q_PROPERTY(QString launchParams MEMBER m_launchParams NOTIFY launchParamsChanged)
	Q_PROPERTY(bool autoLaunch MEMBER m_autoLaunch NOTIFY autoLaunchChanged)

	// Auto Grab Options
	Q_PROPERTY(QString autoGrabTitle MEMBER m_autoGrabTitle NOTIFY autoGrabTitleChanged)
	Q_PROPERTY(QString autoGrabClass MEMBER m_autoGrabClass NOTIFY autoGrabClassChanged)

	// Misc
	Q_PROPERTY(bool startupComplete MEMBER m_startupComplete NOTIFY startupCompleteChanged)
	Q_PROPERTY(bool isAnimating MEMBER m_isAnimating NOTIFY isAnimatingChanged)
	Q_PROPERTY(bool isVisible READ getIsVisible NOTIFY isVisibleChanged)
	Q_PROPERTY(bool windowVisible READ getWindowVisible NOTIFY windowVisibleChanged)

public:
	explicit TreeItem(QObject* parent = nullptr);

	void cleanup();
	void cleanup_internal();
	void cleanupWindow(WindowInfo* wi);
	void setupWindow(WindowInfo* wi);

	int getActiveIndex();
	Q_INVOKABLE void setActiveChild(TreeItem* activeChild);
	Q_INVOKABLE void scrollActiveIndex(int delta);
	Q_INVOKABLE void setActive();

	int getIndex();
	int getDepth();
	bool getIsVisible();
	bool getWindowVisible();

	QScreen* getMonitor();

	QRectF getBounds();
	QRectF getHeaderBounds();
	QRectF getContentBounds();
	QRectF getNodeBounds();

	QRectF getBoundsLocal();
	QRectF getHeaderBoundsLocal();
	QRectF getContentBoundsLocal();
	QRectF getNodeBoundsLocal();

	Q_INVOKABLE void toggleFlow();
	Q_INVOKABLE void toggleLayout();

	Q_INVOKABLE QVariant addChild(QString title = "", QString flow = "", QString layout = "", QScreen* monitor = nullptr, WindowInfo* windowInfo = nullptr);
	QVariant addChild(TreeItem* newChild);
	bool removeChild(TreeItem* childToRemove);

	Q_INVOKABLE void moveUp();
	Q_INVOKABLE void moveDown();
	Q_INVOKABLE void remove();

	QPointF getScreenPosition();

	TreeItem* getTreeParent() const;
	QObjectList getTreeChildren() const;

	void moveChild(TreeItem* child, int delta);
	Q_INVOKABLE void moveChild(int fromIndex, int toIndex);

	WindowInfo* getWindowInfo() const { return m_windowInfo; }
	void setWindowInfo(WindowInfo* newWindowInfo);

	QJsonObject toJsonObject() const;
	void loadFromJson(QJsonObject jsonObject);

signals:
	void parentChanged();
	void childrenChanged();
	void childAdded(int index, TreeItem* child);
	void childMoved(int fromIndex, int toIn);
	void childRemoved(int index, TreeItem* child);
	void activeIndexChanged();

	void flowChanged();
	void layoutChanged();

	void indexChanged();
	void depthChanged();

	void boundsChanged();
	void contentBoundsChanged();

	void headerBoundsChanged();
	void nodeBoundsChanged();

	void monitorChanged();
	void windowInfoChanged();

	void borderlessChanged();

	void launchUriChanged();
	void launchParamsChanged();
	void autoLaunchChanged();

	void autoGrabTitleChanged();
	void autoGrabClassChanged();

	void startupCompleteChanged();
	void isAnimatingChanged();
	void animationFinished();
	void isVisibleChanged();
	void windowVisibleChanged();

	void beginMoveWindows();
	void moveWindow(HWND hwnd, QRect geometry, bool visible);
	void endMoveWindows();
	void setWindowStyle(HWND hwnd, qint32 style);

public slots:
	void startup();
	void updateWindowPosition();
	void launch();
	void playShutdownAnimation();

protected:
	void updateWindowPosition_Internal();

protected slots:
	void tryAutoGrabWindow();

private:
	SettingsContainer* getSettingsContainer();
	WindowView* getWindowView();

	qreal getItemMargin();
	qreal getHeaderSize();

	// Serialized properties
	QString m_flow;
	QString m_layout;
	HWND m_savedHwnd;
	WindowInfo* m_windowInfo;

	bool m_borderless;

	QString m_launchUri;
	QString m_launchParams;
	bool m_autoLaunch;

	QString m_autoGrabTitle;
	QString m_autoGrabClass;

	QList<TreeItem*> m_children;

	// Trasient properties
	bool m_startupComplete;
	TreeItem* m_activeChild;
	bool m_isAnimating;
	bool m_wantsAutoLaunch;

	QQuickWindow* m_itemWindow;
	QQuickWindow* m_headerWindow;
};

Q_DECLARE_METATYPE(TreeItem*)
Q_DECLARE_METATYPE(const TreeItem*)

#endif // TREEITEM_H
