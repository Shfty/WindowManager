#include "TreeItem.h"

#include "AppCore.h"
#include "SettingsContainer.h"
#include "QmlController.h"
#include "WindowController.h"
#include "WindowView.h"

#include <QApplication>
#include <QDesktopServices>
#include <QDir>
#include <QGuiApplication>
#include <QProcess>
#include <QScreen>
#include <QUrl>
#include <QQuickView>
#include <QQmlContext>
#include <QQmlEngine>
#include <QQuickItem>
#include <QJsonArray>

TreeItem::TreeItem(QObject* parent)
: WMObject(parent)
, m_monitorIndex(-1)
, m_windowInfo(nullptr)
, m_activeChild(nullptr)
, m_isAnimating(false)
, m_itemWindow(nullptr)
, m_headerWindow(nullptr)
{
	setObjectName("Tree Item");

	Q_ASSERT(parent != nullptr);

	if(parent->inherits("TreeItem"))
	{
		connect(parent, SIGNAL(contentBoundsChanged()), this, SIGNAL(boundsChanged()));
	}

	connect(this, SIGNAL(boundsChanged()), this, SIGNAL(contentBoundsChanged()));
	connect(this, SIGNAL(boundsChanged()), this, SIGNAL(headerBoundsChanged()));

	connect(this, SIGNAL(activeIndexChanged()), this, SIGNAL(contentBoundsChanged()));
	connect(this, SIGNAL(flowChanged()), this, SIGNAL(contentBoundsChanged()));
	connect(this, SIGNAL(layoutChanged()), this, SIGNAL(contentBoundsChanged()));
	connect(this, SIGNAL(childrenChanged()), this, SIGNAL(contentBoundsChanged()));

	connect(this, &TreeItem::windowInfoChanged, [=]() {
		if(m_windowInfo != nullptr)
		{
			connect(m_windowInfo, &QObject::destroyed, [=]() {
				m_windowInfo = nullptr;
				windowInfoChanged();
			});
		}
	});
	connect(this, SIGNAL(windowInfoChanged()), this, SLOT(moveWindowOnscreen()));

	connect(this, &TreeItem::isAnimatingChanged, [=]() {
		if(m_isAnimating)
		{
			moveWindowOffscreen();
		}
		else
		{
			moveWindowOnscreen();
		}

		isVisibleChanged();
	});

	// Window management signals
	WindowController* wc = getWindowController();
	connect(this, SIGNAL(beginMoveWindows()), wc, SLOT(beginMoveWindows()));
	connect(this, SIGNAL(moveWindow(HWND, QPoint)), wc, SLOT(moveWindow(HWND, QPoint)));
	connect(this, SIGNAL(moveWindow(HWND, QPoint, QSize, qlonglong)), wc, SLOT(moveWindow(HWND, QPoint, QSize, qlonglong)));
	connect(this, SIGNAL(endMoveWindows()), wc, SLOT(endMoveWindows()));
}

TreeItem::~TreeItem()
{

}

QScreen* TreeItem::getMonitor()
{
	if(m_monitorIndex != -1)
	{
		return QGuiApplication::screens()[m_monitorIndex];
	}

	TreeItem* parent = getTreeParent();
	if(parent == nullptr)
	{
		return nullptr;
	}

	return parent->getMonitor();
}

void TreeItem::setMonitor(QScreen* newMonitor)
{
	QList<QScreen*> screens = qApp->screens();
	m_monitorIndex = screens.indexOf(newMonitor);
	monitorChanged();
}

QRectF TreeItem::getBounds()
{
	if(m_monitorIndex != -1)
	{
		QScreen* monitor = getMonitor();
		return QRectF(QPoint(), monitor->size());
	}

	TreeItem* treeParent = getTreeParent();
	if(treeParent == nullptr)
	{
		return QRectF(0, 0, -1, -1);
	}

	QRectF bounds = treeParent->getContentBounds();

	// ---------
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
			qreal newWidth = bounds.width() / parentChildCount;
			qreal dx = index * newWidth;

			newBounds.setWidth(newWidth);
			newBounds.adjust(dx, 0, dx, 0);

			if(index > 0)
			{
				newBounds.adjust(-itemMargin * 0.5, 0, 0, 0);
			}

			if(index < parentChildCount - 1)
			{
				newBounds.adjust(0, 0, itemMargin * 0.5, 0);
			}
		}
		else
		{
			qreal newHeight = bounds.height() / parentChildCount;
			newBounds.setY(bounds.y() + (index * newHeight));
			newBounds.setWidth(bounds.width());
			newBounds.setHeight(newHeight);

			if(index > 0)
			{
				newBounds.adjust(0, -itemMargin * 0.5, 0, 0);
			}

			if(index < parentChildCount - 1)
			{
				newBounds.adjust(0, 0, 0, itemMargin * 0.5);
			}
		}
	}
	// ---------

	return newBounds.marginsRemoved(QMarginsF(itemMargin, itemMargin, itemMargin, itemMargin));
}

QRectF TreeItem::getHeaderBounds()
{
	QRectF bounds = getBounds();

	qreal headerSize = getHeaderSize();

	if(m_monitorIndex != -1)
	{
		QRectF headerBounds;
		headerBounds.setWidth(bounds.width());
		headerBounds.setHeight(headerSize);
		return headerBounds;
	}

	TreeItem* parent = getTreeParent();
	if(parent == nullptr)
	{
		return QRectF(0, 0, -1, -1);
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

QRectF TreeItem::getContentBounds()
{
	QRectF bounds = getBounds();

	if(m_monitorIndex != -1)
	{
		qreal headerSize = getHeaderSize();

		QRectF newBounds;
		newBounds.setWidth(bounds.width());
		newBounds.setHeight(bounds.height());
		newBounds.setY(headerSize);
		return newBounds;
	}

	TreeItem* parent = getTreeParent();
	if(parent == nullptr)
	{
		return QRectF(0, 0, -1, -1);
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

QRectF TreeItem::getContentBoundsLocal()
{
	QRectF contentBounds = getContentBounds();

	QRectF bounds = getBounds();
	contentBounds.adjust(-bounds.x(), -bounds.y(), -bounds.x(), -bounds.y());

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
	if(parent == nullptr)
	{
		return true;
	}
	else
	{
		if(parent->getIsVisible() == false)
		{
			return false;
		}

		if(parent->property("layout").toString() == "Tabbed")
		{
			if(parent->getActiveIndex() != getIndex())
			{
				return false;
			}
		}
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
		{"monitorIndex", m_monitorIndex},
		{"hwnd", reinterpret_cast<qlonglong>(hwnd)},
		{"launchUri", m_launchUri},
		{"launchParams", m_launchParams},
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

	m_flow = jsonObject.value("flow").toString();
	m_layout = jsonObject.value("layout").toString();

	m_monitorIndex = jsonObject.value("monitorIndex").toInt();
	if(m_monitorIndex != -1)
	{
		QScreen* monitor = QGuiApplication::screens()[m_monitorIndex];

		AppCore* appCore = getAppCore();
		QmlController* qmlController = appCore->property("qmlController").value<QmlController*>();

		QQmlContext* rootContext = qmlController->getRootContext();
		QQmlContext* newContext = new QQmlContext(rootContext, this);
		newContext->setContextProperty("treeItem", this);

		m_headerWindow = qmlController->createWindow(QUrl("qrc:/qml/tree/HeaderWindow.qml"), monitor->geometry(), newContext);
		m_headerWindow->setColor(Qt::transparent);
		m_headerWindow->setFlags(m_headerWindow->flags() | static_cast<Qt::WindowFlags>(
		Qt::WA_TranslucentBackground |
		Qt::FramelessWindowHint |
		Qt::WindowStaysOnBottomHint |
		Qt::WindowDoesNotAcceptFocus
		));
		m_headerWindow->setScreen(monitor);
		m_headerWindow->setVisibility(QWindow::Maximized);

		m_itemWindow = qmlController->createWindow(QUrl("qrc:/qml/tree/NodeWindow.qml"), monitor->geometry(), newContext);
		m_itemWindow->setColor(Qt::transparent);
		m_itemWindow->setFlags(m_itemWindow->flags() | static_cast<Qt::WindowFlags>(
		Qt::WA_TranslucentBackground |
		Qt::FramelessWindowHint |
		Qt::WindowStaysOnBottomHint |
		Qt::WindowDoesNotAcceptFocus
		));
		m_itemWindow->setScreen(monitor);
		m_itemWindow->setVisibility(QWindow::Maximized);


		m_headerWindow->showMaximized();
		m_itemWindow->showMaximized();

		m_headerWindow->lower();
		m_itemWindow->lower();

		if(monitor != nullptr)
		{
			connect(monitor, SIGNAL(geometryChanged(const QRect&)), m_itemWindow, SLOT(setGeometry(const QRect&)));
			connect(monitor, SIGNAL(geometryChanged(const QRect&)), m_headerWindow, SLOT(setGeometry(const QRect&)));
			connect(monitor, SIGNAL(geometryChanged(const QRect&)), this, SIGNAL(boundsChanged()));
		}
	}

	WindowView* wv = getWindowView();
	HWND loadedHwnd = reinterpret_cast<HWND>(jsonObject.value("hwnd").toInt());
	m_windowInfo = wv->getWindowInfo(loadedHwnd);

	connect(this, SIGNAL(autoGrabTitleChanged()), this, SLOT(tryAutoGrabWindow()));
	connect(this, SIGNAL(autoGrabClassChanged()), this, SLOT(tryAutoGrabWindow()));
	connect(wv, SIGNAL(windowListChanged()), this, SLOT(tryAutoGrabWindow()));

	m_launchUri = jsonObject.value("launchUri").toString();
	m_launchParams = jsonObject.value("launchParams").toString();

	m_autoGrabTitle = jsonObject.value("autoGrabTitle").toString();
	m_autoGrabClass = jsonObject.value("autoGrabClass").toString();

	tryAutoGrabWindow();

	QJsonArray childArray = jsonObject.value("children").toArray();
	for(int i = 0; i < childArray.count(); ++i)
	{
		TreeItem* item = new TreeItem(this);
		item->loadFromJson(childArray[i].toObject());
		addChild(item);
	}
}

void TreeItem::moveWindowOnscreen()
{
	emit beginMoveWindows();
	moveWindowOnscreen_Internal();
	emit endMoveWindows();
}

void TreeItem::moveWindowOffscreen()
{
	emit beginMoveWindows();
	moveWindowOffscreen_Internal();
	emit endMoveWindows();
}

void TreeItem::launch()
{
	if(!m_launchUri.isEmpty())
	{
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

void TreeItem::moveWindowOnscreen_Internal()
{
	if(!getIsVisible()) return;

	if(m_windowInfo != nullptr)
	{
		QRectF contentBounds = getContentBounds();

		QPointF desktopPosition = getMonitor()->geometry().topLeft();
		QPointF screenPosition = contentBounds.topLeft();
		QPointF globalPosition = desktopPosition + screenPosition;

		QPoint position = globalPosition.toPoint();
		QSize size = contentBounds.size().toSize();

		emit moveWindow(m_windowInfo->getHwnd(), position, size, -2LL);
	}

	for(TreeItem* child : m_children)
	{
		child->moveWindowOnscreen_Internal();
	}
}

void TreeItem::moveWindowOffscreen_Internal()
{
	if(m_windowInfo != nullptr)
	{
		HWND hwnd = m_windowInfo->getHwnd();

		WindowView* wv = getWindowView();
		QPoint offscreenArea = wv->getOffscreenArea();

		emit moveWindow(hwnd, offscreenArea);
	}

	for(TreeItem* child : m_children)
	{
		child->moveWindowOffscreen_Internal();
	}
}

void TreeItem::tryAutoGrabWindow()
{
	if(m_children.length() > 0) return;
	if(m_autoGrabTitle.isEmpty() && m_autoGrabClass.isEmpty()) return;

	WindowView* wv = getAppCore()->property("windowView").value<WindowView*>();
	WindowInfo* foundWindow = wv->getWindowByRegex(m_autoGrabTitle, m_autoGrabClass);

	if(foundWindow != nullptr)
	{
		m_windowInfo = foundWindow;
		windowInfoChanged();
	}
	else if(false && m_windowInfo != nullptr)
	{
		m_windowInfo = nullptr;
		windowInfoChanged();
	}
}

SettingsContainer* TreeItem::getSettingsContainer()
{
	AppCore* appCore = getAppCore();
	Q_ASSERT(appCore != nullptr);

	return appCore->property("settingsContainer").value<SettingsContainer*>();

}

WindowController* TreeItem::getWindowController()
{
	AppCore* appCore = getAppCore();
	Q_ASSERT(appCore != nullptr);

	return appCore->property("windowController").value<WindowController*>();
}

WindowView* TreeItem::getWindowView()
{
	AppCore* appCore = getAppCore();
	Q_ASSERT(appCore != nullptr);

	return appCore->property("windowView").value<WindowView*>();
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
