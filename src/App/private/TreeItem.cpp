#include "TreeItem.h"

#include <QApplication>
#include <QDesktopServices>
#include <QDir>
#include <QGuiApplication>
#include <QProcess>
#include <QScreen>
#include <QUrl>
#include <QQuickWindow>
#include <QQmlContext>
#include <QQmlEngine>
#include <QQuickItem>
#include <QJsonArray>

#include <QDebug>
Q_LOGGING_CATEGORY(treeItem, "app.treeItem")

#include <SettingsContainer.h>
#include <WindowInfo.h>
#include <WindowView.h>

#include "TreeModel.h"
#include "AppCore.h"
#include "QMLController.h"
#include "Win.h"

TreeItem::TreeItem(QObject* parent)
	: QObject(parent)
	, m_windowInfo(nullptr)
	, m_borderless(false)
	, m_autoLaunch(false)
	, m_startupComplete(false)
	, m_activeChild(nullptr)
	, m_isAnimating(false)
	, m_wantsAutoLaunch(false)
	, m_itemWindow(nullptr)
	, m_headerWindow(nullptr)
{
	setObjectName("Tree Item");

	qCInfo(treeItem) << "Startup";

	Q_ASSERT(parent != nullptr);

	if(parent->inherits("TreeItem"))
	{
		connect(parent, SIGNAL(contentBoundsChanged()), this, SIGNAL(boundsChanged()));
		connect(this, SIGNAL(animationFinished()), parent, SIGNAL(animationFinished()));
	}

	connect(this, SIGNAL(boundsChanged()), this, SIGNAL(contentBoundsChanged()));
	connect(this, SIGNAL(boundsChanged()), this, SIGNAL(headerBoundsChanged()));

	connect(this, SIGNAL(contentBoundsChanged()), this, SIGNAL(nodeBoundsChanged()));

	connect(this, SIGNAL(activeIndexChanged()), this, SIGNAL(contentBoundsChanged()));
	connect(this, SIGNAL(flowChanged()), this, SIGNAL(contentBoundsChanged()));
	connect(this, SIGNAL(layoutChanged()), this, SIGNAL(contentBoundsChanged()));
	connect(this, SIGNAL(childrenChanged()), this, SIGNAL(contentBoundsChanged()));

	connect(this, &TreeItem::borderlessChanged, [=]() {
		if(m_windowInfo != nullptr)
		{
			if(m_borderless)
			{
				qint32 winStyle = m_windowInfo->getWinStyle();
				winStyle &= ~(WS_CAPTION | WS_THICKFRAME | WS_MINIMIZEBOX | WS_MAXIMIZEBOX | WS_SYSMENU);
				qInfo() << "Emitting SetWindowStyle for " << m_windowInfo->getWinTitle() << " with style " << winStyle;
				emit setWindowStyle(m_windowInfo->getHwnd(), winStyle);
			}
			else
			{
				qInfo() << "Emitting SetWindowStyle for " << m_windowInfo->getWinTitle() << " with style " << m_windowInfo->getWinStyle();
				emit setWindowStyle(m_windowInfo->getHwnd(), m_windowInfo->getWinStyle());
			}

			updateWindowPosition();
		}
	});

	connect(this, &TreeItem::isAnimatingChanged, [=]() {
		if(!m_isAnimating)
		{
			updateWindowPosition();
			animationFinished();
		}

		isVisibleChanged();
		windowVisibleChanged();
	});

	// Window management signals
	TreeModel* tm = TreeModel::getInstance(this);
	connect(this, &TreeItem::moveWindow, tm, &TreeModel::moveWindow);
	connect(this, &TreeItem::commitWindowMove, tm, &TreeModel::commitWindowMove);

	connect(this, SIGNAL(setWindowStyle(HWND, qint32)), tm, SIGNAL(setWindowStyle(HWND, qint32)));
}

void TreeItem::cleanup()
{
	qCInfo(treeItem) << objectName() << "cleaning up";

	cleanup_internal();
	emit commitWindowMove();
}

void TreeItem::cleanup_internal()
{
	cleanupWindow(m_windowInfo);

	for(TreeItem* child : m_children)
	{
		child->cleanup_internal();
	}
}

void TreeItem::cleanupWindow(WindowInfo* wi)
{
	qCInfo(treeItem) << objectName() << "cleaning up window";

	if(wi != nullptr)
	{
		disconnect(wi, SIGNAL(windowClosed()));

		// Restore style
		emit setWindowStyle(m_windowInfo->getHwnd(), m_windowInfo->getWinStyle());
	}
}

void TreeItem::setupWindow(WindowInfo* wi)
{
	if(m_windowInfo)
	{
		connect(wi, &WindowInfo::windowClosed, [=](){
			m_windowInfo = nullptr;
			windowInfoChanged();
		});

		if(m_borderless)
		{
			qint32 winStyle = m_windowInfo->getWinStyle();
			winStyle &= ~(WS_CAPTION | WS_THICKFRAME | WS_MINIMIZEBOX | WS_MAXIMIZEBOX | WS_SYSMENU);
			emit setWindowStyle(m_windowInfo->getHwnd(), winStyle);
		}
	}
}

QScreen* TreeItem::getMonitor()
{
	AppCore* appCore = AppCore::getInstance(this);
	QMLController* qmlController = appCore->getQmlController();
	QQuickWindow* qmlWindow = qmlController->getQmlWindow();
	return qmlWindow->screen();
}

QRectF TreeItem::getBounds()
{
	// If this is the root item, return the window size
	TreeItem* treeParent = getTreeParent();
	if(treeParent == nullptr)
	{
		AppCore* appCore = AppCore::getInstance(this);
		QMLController* qmlController = appCore->getQmlController();
		QQuickWindow* appWindow = qmlController->getQmlWindow();
		return QRectF(QPoint(), appWindow->size());
	}

	QRectF bounds = treeParent->getContentBounds();

	QObjectList parentChildren = treeParent->getTreeChildren();
	int index = parentChildren.indexOf(this);
	int parentChildCount = parentChildren.length();
	QString parentFlow = treeParent->property("flow").toString();
	QString parentLayout = treeParent->property("layout").toString();

	QRectF newBounds = QRectF(bounds);
	qreal itemMargin = getItemMargin();

	if(parentLayout == "Split")
	{
		if(parentFlow == "Horizontal")
		{
			qreal newWidth = (bounds.width() - itemMargin) / parentChildCount;
			qreal dx = index * newWidth;

			newBounds.setWidth(newWidth);
			newBounds.adjust(dx, 0, dx, 0);

			return newBounds.marginsRemoved(QMarginsF(itemMargin, itemMargin, 0, itemMargin));
		}
		else
		{
			qreal newHeight = (bounds.height() - itemMargin) / parentChildCount;
			newBounds.setY(bounds.y() + (index * newHeight));
			newBounds.setWidth(bounds.width());
			newBounds.setHeight(newHeight);

			return newBounds.marginsRemoved(QMarginsF(itemMargin, itemMargin, itemMargin, 0));
		}
	}
	else
	{
		return newBounds.marginsRemoved(QMarginsF(itemMargin, itemMargin, itemMargin, itemMargin));
	}
}

QRectF TreeItem::getContentBounds()
{
	QRectF bounds = getBounds();

	TreeItem* parent = getTreeParent();
	if(parent == nullptr)
	{
		qreal headerSize = getHeaderSize();
		bounds.adjust(0, headerSize, 0, 0);
		return bounds;
	}

	QObjectList parentChildren = parent->getTreeChildren();
	int index = parentChildren.indexOf(this);
	int parentChildCount = parentChildren.length();
	QString parentFlow = parent->property("flow").toString();
	QString parentLayout = parent->property("layout").toString();
	int parentActiveIndex = parent->property("activeIndex").toInt();

	QRectF newBounds = QRectF(bounds);
	qreal headerSize = getHeaderSize();
	qreal itemMargin = getItemMargin();

	if(parentLayout == "Split")
	{
		newBounds.adjust(0, headerSize, 0, 0);
	}
	else if(parentLayout == "Tabbed")
	{
		int deltaIndex = parentActiveIndex - index;
		if(parentFlow == "Horizontal")
		{
			qreal dx = (itemMargin + bounds.width()) * -deltaIndex;
			newBounds.adjust(dx, headerSize, dx, 0);
		}
		else
		{
			qreal dy = (itemMargin + bounds.height()) * -deltaIndex;
			qreal headerHeight = headerSize * parentChildCount;
			newBounds.adjust(0, dy + headerHeight, 0, dy);
		}
	}

	return newBounds;
}

QRectF TreeItem::getHeaderBounds()
{
	QRectF bounds = getBounds();
	qreal headerSize = getHeaderSize();

	TreeItem* parent = getTreeParent();
	if(parent == nullptr)
	{
		QRectF headerBounds;
		headerBounds.setWidth(bounds.width());
		headerBounds.setHeight(headerSize);
		return headerBounds;
	}

	QObjectList parentChildren = parent->getTreeChildren();
	int index = parentChildren.indexOf(this);
	int parentChildCount = parentChildren.length();
	QString parentFlow = parent->property("flow").toString();
	QString parentLayout = parent->property("layout").toString();


	QRectF headerBounds = QRectF(bounds);
	if(parentLayout == "Tabbed")
	{
		if(parentFlow == "Horizontal")
		{
			qreal newWidth = bounds.width() / parentChildCount;

			headerBounds.setWidth(newWidth);

			qreal dx = index * newWidth;
			headerBounds.adjust(dx, 0, dx, 0);
		}
		else if(parentFlow == "Vertical")
		{
			qreal dy = index * headerSize;
			headerBounds.adjust(0, dy, 0, dy);
		}
	}
	headerBounds.setHeight(headerSize);

	return headerBounds;
}

QRectF TreeItem::getNodeBounds()
{
	QRectF bounds = getContentBounds();

	TreeItem* parent = getTreeParent();
	if(parent == nullptr)
	{
		return bounds;
	}

	if(m_layout == "Tabbed")
	{
		if(m_flow == "Vertical")
		{
			bounds.adjust(0, m_children.length() * getHeaderSize(), 0, 0);
		}
		else if(m_flow == "Horizontal")
		{
			bounds.adjust(0, getHeaderSize(), 0, 0);
		}

		bounds.adjust(0, getItemMargin(), 0, 0);
	}

	return bounds;
}

QRectF TreeItem::getBoundsLocal()
{
	QRectF bounds = getBounds();

	TreeItem* treeParent = getTreeParent();
	if(treeParent != nullptr)
	{
		QRectF parentBounds = treeParent->getBounds();
		bounds.adjust(-parentBounds.x(), -parentBounds.y(), -parentBounds.x(), -parentBounds.y());
	}

	return bounds;
}

QRectF TreeItem::getHeaderBoundsLocal()
{
	QRectF headerBounds = getHeaderBounds();

	QRectF bounds = getBounds();
	headerBounds.adjust(-bounds.x(), -bounds.y(), -bounds.x(), -bounds.y());

	return headerBounds;
}

QRectF TreeItem::getNodeBoundsLocal()
{
	QRectF contentBounds = getContentBounds();
	QRectF nodeBounds = getNodeBounds();
	nodeBounds.adjust(-contentBounds.x(), -contentBounds.y(), -contentBounds.x(), -contentBounds.y());

	return nodeBounds;
}

QRectF TreeItem::getContentBoundsLocal()
{
	QRectF contentBounds = getContentBounds();

	TreeItem* treeParent = getTreeParent();
	if(treeParent != nullptr)
	{
		QRectF parentBounds = treeParent->getContentBounds();
		contentBounds.adjust(-parentBounds.x(), -parentBounds.y(), -parentBounds.x(), -parentBounds.y());
	}
	return contentBounds;
}

int TreeItem::getIndex()
{
	TreeItem* parent = getTreeParent();
	if(parent != nullptr)
	{
		QObjectList parentChildren = parent->getTreeChildren();
		return parentChildren.indexOf(this);
	}

	return -1;
}

int TreeItem::getDepth()
{
	int depth = 0;

	TreeItem* candidate = this;
	do
	{
		TreeItem* candidateParent = candidate->getTreeParent();
		if(candidateParent != nullptr)
		{
			candidate = candidateParent;
			depth++;
		}
	} while(candidate->getTreeParent() != nullptr);

	return depth;
}

bool TreeItem::getIsVisible()
{
	if(m_isAnimating) return true;

	TreeItem* parent = getTreeParent();
	if(parent != nullptr)
	{
		if(parent->property("layout").toString() == "Tabbed" && parent->getActiveIndex() != getIndex())
		{
			return false;
		}

		return parent->getWindowVisible();
	}

	return true;
}

bool TreeItem::getWindowVisible()
{
	if(m_isAnimating) return false;

	TreeItem* parent = getTreeParent();
	if(parent != nullptr)
	{
		if(parent->property("layout").toString() == "Tabbed" && parent->getActiveIndex() != getIndex())
		{
			return false;
		}

		return parent->getWindowVisible();
	}

	return true;
}

int TreeItem::getActiveIndex()
{
	return m_children.indexOf(m_activeChild);
}

void TreeItem::setActiveChild(TreeItem* activeChild)
{
	if(m_children.indexOf(activeChild) < 0)
	{
		return;
	}

	m_activeChild = activeChild;
	activeIndexChanged();
}

void TreeItem::scrollActiveIndex(int delta)
{
	int activeIndex = getActiveIndex();
	activeIndex += delta;
	activeIndex = min(activeIndex, m_children.length() - 1);
	activeIndex = max(activeIndex, 0);
	setActiveChild(m_children[activeIndex]);
}

void TreeItem::setActive()
{
	TreeItem* parent = getTreeParent();
	if(parent != nullptr)
	{
		parent->setActiveChild(this);
	}
}

void TreeItem::toggleFlow()
{
	m_flow = m_flow == "Horizontal" ? "Vertical" : "Horizontal";
	flowChanged();
}

void TreeItem::toggleLayout()
{
	m_layout = m_layout == "Split" ? "Tabbed" : "Split";
	layoutChanged();
}

QVariant TreeItem::addChild(QString title, QString flow, QString layout, QScreen* monitor, WindowInfo* windowInfo)
{
	TreeItem* child = new TreeItem(this);

	child->setObjectName(title);
	child->setProperty("title", title);
	child->setProperty("flow", flow);
	child->setProperty("layout", layout);
	child->setProperty("monitor", QVariant::fromValue(monitor));
	child->setProperty("windowInfo", QVariant::fromValue(windowInfo));

	return addChild(child);
}

QVariant TreeItem::addChild(TreeItem* newChild)
{
	if(newChild == nullptr) return QVariant::Invalid;

	m_children.append(newChild);
	childAdded(m_children.length(), newChild);

	if(m_activeChild == nullptr || m_children.length() == 1)
	{
		m_activeChild = newChild;
		activeIndexChanged();
	}

	childrenChanged();

	return QVariant::fromValue<TreeItem*>(newChild);
}

bool TreeItem::removeChild(TreeItem* childToRemove)
{
	if(childToRemove == nullptr) return false;

	int removedIndex = m_children.indexOf(childToRemove);

	if(removedIndex == -1) return false;

	if(m_children.length() > 1)
	{
		if(removedIndex > 0)
		{
			setActiveChild(m_children.at(removedIndex - 1));
		}
		else
		{
			setActiveChild(m_children.at(removedIndex + 1));
		}
	}

	m_children.removeOne(childToRemove);
	childRemoved(removedIndex, childToRemove);

	if(m_children.length() == 0)
	{
		m_activeChild = nullptr;
		activeIndexChanged();
	}

	childrenChanged();

	return true;
}

void TreeItem::moveUp()
{
	TreeItem* parent = getTreeParent();
	if(parent != nullptr)
	{
		parent->moveChild(this, -1);
	}
}

void TreeItem::moveDown()
{
	TreeItem* parent = getTreeParent();
	if(parent != nullptr)
	{
		parent->moveChild(this, 1);
	}
}

void TreeItem::remove()
{
	TreeItem* parent = getTreeParent();
	Q_ASSERT(parent != nullptr);

	parent->removeChild(this);
	deleteLater();
}

QPointF TreeItem::getScreenPosition()
{
	return property("contentBounds").toRect().topLeft();
}

TreeItem* TreeItem::getTreeParent() const
{
	return qobject_cast<TreeItem*>(parent());
}

QObjectList TreeItem::getTreeChildren() const
{
	QList<TreeItem*> children = m_children;
	QObjectList treeChildren;

	for(QObject* o : children)
	{
		treeChildren.append(o);
	}

	return treeChildren;
}

void TreeItem::moveChild(TreeItem* child, int delta)
{
	if(delta == 0)
		return;

	int childIndex = m_children.indexOf(child);

	if(childIndex < 0)
		return;

	int targetIndex = childIndex + delta;

	if(targetIndex < 0)
		return;
	if(targetIndex >= m_children.length())
		return;

	TreeItem* displacedChild = m_children.at(targetIndex);

	m_children.removeAt(childIndex);
	m_children.insert(targetIndex, child);

	child->indexChanged();
	child->boundsChanged();

	displacedChild->indexChanged();
	displacedChild->boundsChanged();

	childrenChanged();
	childMoved(childIndex, targetIndex);
}

void TreeItem::moveChild(int fromIndex, int toIndex)
{
	if(fromIndex < 0 || fromIndex >= m_children.length()) return;
	if(toIndex < 0 || toIndex >= m_children.length()) return;

	TreeItem* child = m_children[fromIndex];
	int delta = toIndex - fromIndex;
	moveChild(child, delta);
}

void TreeItem::setWindowInfo(WindowInfo* newWindowInfo)
{
	if(m_windowInfo != nullptr && m_windowInfo->getHwnd() != nullptr)
	{
		cleanupWindow(m_windowInfo);
		emit commitWindowMove();
	}

	// Assign new window
	m_windowInfo = newWindowInfo;
	windowInfoChanged();

	setupWindow(m_windowInfo);
}

QJsonObject TreeItem::toJsonObject() const
{
	HWND hwnd = nullptr;

	if(m_windowInfo != nullptr)
	{
		hwnd = m_windowInfo->property("hwnd").value<HWND>();
	}

	QJsonObject jo
	{
		{"objectName", objectName()},
		{"flow", m_flow},
		{"layout", m_layout},
		{"hwnd", reinterpret_cast<qlonglong>(hwnd)},
		{"borderless", m_borderless},
		{"launchUri", m_launchUri},
		{"launchParams", m_launchParams},
		{"autoLaunch", m_autoLaunch},
		{"autoGrabTitle", m_autoGrabTitle},
		{"autoGrabClass", m_autoGrabClass}
	};

	QJsonArray children;

	for(TreeItem* child : m_children)
	{
		children.append(child->toJsonObject());
	}

	jo["children"] = children;

	return jo;
}

void TreeItem::loadFromJson(QJsonObject jsonObject)
{
	setObjectName(jsonObject.value("objectName").toString());

	qCInfo(treeItem) << "Loading" << objectName() << "from JSON";

	m_flow = jsonObject.value("flow").toString();
	m_layout = jsonObject.value("layout").toString();

	qlonglong savedHwnd = jsonObject.value("hwnd").toInt();
	m_savedHwnd = reinterpret_cast<HWND>(savedHwnd);

	m_borderless = jsonObject.value("borderless").toBool();

	m_launchUri = jsonObject.value("launchUri").toString();
	m_launchParams = jsonObject.value("launchParams").toString();
	m_autoLaunch = jsonObject.value("autoLaunch").toBool();

	m_autoGrabTitle = jsonObject.value("autoGrabTitle").toString();
	m_autoGrabClass = jsonObject.value("autoGrabClass").toString();

	QJsonArray childArray = jsonObject.value("children").toArray();
	for(int i = 0; i < childArray.count(); ++i)
	{
		TreeItem* item = new TreeItem(this);
		item->loadFromJson(childArray[i].toObject());
		addChild(item);
	}
}

void TreeItem::startup()
{
	qCInfo(treeItem) << objectName() << "startup";

	connect(this, SIGNAL(autoGrabTitleChanged()), this, SLOT(tryAutoGrabWindow()));
	connect(this, SIGNAL(autoGrabClassChanged()), this, SLOT(tryAutoGrabWindow()));

	WindowView* wv = getWindowView();
	connect(wv, &WindowView::windowListChanged, [=](){
		if(m_windowInfo == nullptr && (!m_autoGrabTitle.isEmpty() || !m_autoGrabClass.isEmpty()))
		{
			tryAutoGrabWindow();
		}
	});

	setWindowInfo(wv->getWindowInfo(m_savedHwnd));

	if(m_windowInfo == nullptr)
	{
		tryAutoGrabWindow();
	}

	if(m_autoLaunch && m_windowInfo == nullptr)
	{
		launch();
	}

	for(TreeItem* child : m_children)
	{
		child->startup();
	}

	m_startupComplete = true;
	startupCompleteChanged();
}

void TreeItem::playShutdownAnimation()
{
	m_layout = "Split";
	layoutChanged();

	for(TreeItem* child : m_children)
	{
		child->playShutdownAnimation();
	}
}

void TreeItem::updateWindowPosition()
{
	//qCInfo(treeItem) << objectName() << "updating window position";

	updateWindowPosition_Internal();
	emit commitWindowMove();
}

void TreeItem::launch()
{
	if(!m_launchUri.isEmpty())
	{
		qCInfo(treeItem) << objectName() << "launching";

		QUrl uri = QUrl(m_launchUri + m_launchParams, QUrl::StrictMode);
		if(uri.isValid())
		{
			QDesktopServices::openUrl(uri);
		}
		else
		{
			QProcess::startDetached(m_launchUri, m_launchParams.split(" "));
		}
	}

	for(TreeItem* child : m_children)
	{
		child->launch();
	}
}

void TreeItem::updateWindowPosition_Internal()
{
	if(m_windowInfo != nullptr && m_windowInfo->getHwnd() != nullptr)
	{
		HWND hwnd = m_windowInfo->getHwnd();

		QRectF contentBounds = getContentBounds();

		QScreen* monitor = getMonitor();
		qreal dpr = monitor->devicePixelRatio();

		bool visible = getWindowVisible();

		// Calculate position
		QPointF desktopPosition = monitor->geometry().topLeft();
		QPointF screenPosition = contentBounds.topLeft();
		QPointF globalPosition = desktopPosition + screenPosition;
		globalPosition *= dpr;

		// Calculate size
		QSizeF globalSize = contentBounds.size();
		globalSize *= dpr;

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
		globalPosition -= QPointF(extendedMargins.left(), extendedMargins.top());
		globalSize += QSizeF(extendedMargins.left() + extendedMargins.right(), extendedMargins.top() + extendedMargins.bottom());

		QPoint position = globalPosition.toPoint();
		QSize size = globalSize.toSize();

		emit moveWindow(m_windowInfo->getHwnd(), QRect(position, size), visible);
	}

	for(TreeItem* child : m_children)
	{
		child->updateWindowPosition_Internal();
	}
}

void TreeItem::tryAutoGrabWindow()
{
	if(m_children.length() > 0) return;
	if(m_autoGrabTitle.isEmpty() && m_autoGrabClass.isEmpty()) return;

	//qCInfo(treeItem) << objectName() << "trying auto grab";

	WindowView* wv = getWindowView();
	WindowInfo* foundWindow = wv->getWindowByRegex(m_autoGrabTitle, m_autoGrabClass);

	if(foundWindow != nullptr)
	{
		//qCInfo(treeItem) << objectName() << "auto grab successful";
		setWindowInfo(foundWindow);
		updateWindowPosition();
	}
	else
	{
		//qCInfo(treeItem) << objectName() << "auto grab failed";
	}
	/*
	else if(false && m_windowInfo != nullptr)
	{
		setWindowInfo(nullptr);
	}
	*/
}

SettingsContainer* TreeItem::getSettingsContainer()
{
	AppCore* appCore = AppCore::getInstance(this);
	Q_ASSERT(appCore != nullptr);

	return appCore->getSettingsContainer();
}

WindowView* TreeItem::getWindowView()
{
	AppCore* appCore = AppCore::getInstance(this);
	Q_ASSERT(appCore != nullptr);

	return appCore->getWindowView();
}

qreal TreeItem::getItemMargin()
{
	SettingsContainer* settings = getSettingsContainer();
	Q_ASSERT(settings != nullptr);

	return settings->property("itemMargin").toReal();
}

qreal TreeItem::getHeaderSize()
{
	SettingsContainer* settings = getSettingsContainer();
	Q_ASSERT(settings != nullptr);

	return settings->property("headerSize").toReal();
}
