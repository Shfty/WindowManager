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

class WindowView;
class QScreen;
class WindowObject;

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

	Q_PROPERTY(WindowObject* windowInfo READ getWindowInfo WRITE setWindowInfo NOTIFY windowInfoChanged)

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

public:
	explicit TreeItem(QObject* parent = nullptr);

	void setupWindow(WindowObject* wi);

	int getActiveIndex();

	int getIndex();
	int getDepth();
	bool getIsVisible();
	bool getWindowVisible();

	QVariant addChild(TreeItem* newChild);
	bool removeChild(TreeItem* childToRemove);

	TreeItem* getTreeParent() const;
	QObjectList getTreeChildren() const;

	void moveChild(TreeItem* child, int delta);

	WindowObject* getWindowInfo() const { return m_windowInfo; }
	void setWindowInfo(WindowObject* newWindowInfo);

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

	void windowInfoChanged();

	void borderlessChanged();

	void launchUriChanged();
	void launchParamsChanged();
	void autoLaunchChanged();

	void autoGrabTitleChanged();
	void autoGrabClassChanged();

	void startupCompleteChanged();

	void setWindowStyle(HWND hwnd, qint32 style);

public slots:
	void startup();
	void cleanup();

	void launch();

	void toggleFlow();
	void toggleLayout();

	void moveUp();
	void moveDown();
	void remove();

	QVariant addChild(QString title = "", QString flow = "", QString layout = "", WindowObject* windowInfo = nullptr);
	void moveChild(int fromIndex, int toIndex);

	void setActiveChild(TreeItem* activeChild);
	void scrollActiveIndex(int delta);
	void setActive();

	void restoreWindowStyle();

protected slots:
	void tryAutoGrabWindow();

private:
	WindowView* getWindowView();

	// Serialized properties
	QString m_flow;
	QString m_layout;
	HWND m_savedHwnd;
	WindowObject* m_windowInfo;

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
	bool m_wantsAutoLaunch;
};

Q_DECLARE_METATYPE(TreeItem*)
Q_DECLARE_METATYPE(const TreeItem*)

#endif // TREEITEM_H
