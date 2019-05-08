#include "TreeItem.h"

#include "AppCore.h"
#include "SettingsContainer.h"
#include "QmlController.h"
#include "WindowController.h"
#include "WindowView.h"

#include <QDebug>
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

TreeItem::TreeItem(QObject* parent)
	: WMObject(parent)
	, m_monitorIndex(-1)
	, m_hwnd(nullptr)
	, m_activeChild(nullptr)
	, m_isAnimating(false)
	, m_itemWindow(nullptr)
	, m_headerWindow(nullptr)
	, m_overlayWindow(nullptr)
{
	setObjectName("Tree Item");

	Q_ASSERT(parent != nullptr);

	if(parent->inherits("TreeItem"))
	{
		connect(parent, SIGNAL(contentBoundsChanged()), this, SIGNAL(boundsChanged()));
	}
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

QRectF TreeItem::calculateBounds()
{
	if(m_monitorIndex != -1)
	{
		QScreen* monitor = getMonitor();
		return monitor->geometry();
	}

	TreeItem* treeParent = getTreeParent();
	if(treeParent == nullptr)
	{
		return QRectF(0, 0, -1, -1);
	}

	QRectF parentContentBounds = treeParent->calculateContentBounds();
	QRectF newBounds;
	newBounds.setWidth(parentContentBounds.width());
	newBounds.setHeight(parentContentBounds.height());

	qreal margin = getItemMargin();
	return newBounds.marginsRemoved(QMarginsF(margin, margin, margin, margin));
}

QRectF TreeItem::calculateHeaderBounds()
{
	QRectF bounds = calculateBounds();

	if(m_monitorIndex != -1)
	{
		qreal headerSize = getHeaderSize();

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

	QRectF headerBounds;
	if(parentFlow == "Horizontal")
	{
		qreal newWidth = bounds.width() / parentChildCount;
		qreal headerSize = getHeaderSize();

		headerBounds.setX(index * newWidth);
		headerBounds.setWidth(newWidth);
		headerBounds.setHeight(headerSize);

		if(parentLayout == "Split")
		{
			qreal itemMargin = getItemMargin();
			QMarginsF margins = QMarginsF(index > 0 ? itemMargin / 2 : 0, 0, index < parentChildCount - 1 ? itemMargin / 2 : 0, 0);
			headerBounds = headerBounds.marginsRemoved(margins);
		}
	}
	else
	{
		if(parentLayout == "Split")
		{
			qreal newHeight = bounds.height() / parentChildCount;
			qreal headerSize = getHeaderSize();
			qreal itemMargin = getItemMargin();

			headerBounds.setY(index * newHeight);
			if(index > 0)
			{
				headerBounds.setY(headerBounds.y() + itemMargin / 2);
			}
			headerBounds.setWidth(bounds.width());
			headerBounds.setHeight(headerSize);
		}
		else
		{
			qreal headerSize = getHeaderSize();

			headerBounds.setY(index * headerSize);
			headerBounds.setWidth(bounds.width());
			headerBounds.setHeight(headerSize);
		}
	}

	return headerBounds;
}

QRectF TreeItem::calculateContentBounds()
{
	QRectF bounds = calculateBounds();

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

	QRectF newBounds;
		qreal headerSize = getHeaderSize();
		qreal itemMargin = getItemMargin();
	if(parentLayout == "Split")
	{
		if(parentFlow == "Horizontal")
		{
			qreal newWidth = bounds.width() / parentChildCount;

			newBounds.setX(index * newWidth);
			newBounds.setY(headerSize);
			newBounds.setWidth(newWidth);
			newBounds.setHeight(bounds.height() - headerSize);

			QMarginsF margins = QMarginsF(index > 0 ? itemMargin / 2 : 0, 0, index < parentChildCount - 1 ? itemMargin / 2 : 0, 0);
			newBounds = newBounds.marginsRemoved(margins);
		}
		else
		{
			qreal newHeight = bounds.height() / parentChildCount;
			newBounds.setY(index * newHeight + headerSize);
			newBounds.setWidth(bounds.width());
			newBounds.setHeight(newHeight - headerSize);

			QMarginsF margins = QMarginsF(0, index > 0 ? itemMargin / 2 : 0, 0, index < parentChildCount - 1 ? itemMargin / 2 : 0);
			newBounds = newBounds.marginsRemoved(margins);
		}
	}
	else
	{
		int deltaIndex = parentActiveIndex - index;
		if(parentFlow == "Horizontal")
		{
			qreal itemWidth = bounds.width() + itemMargin;
			newBounds.setX(itemWidth * -deltaIndex);
			newBounds.setY(headerSize);

			newBounds.setWidth(bounds.width());
			newBounds.setHeight(bounds.height() - headerSize);
		}
		else
		{
			qreal itemHeight = bounds.height() + itemMargin;
			newBounds.setX(0);

			if(index == parentActiveIndex)
			{
				newBounds.setY(headerSize * parentChildCount);
			}
			else
			{
				newBounds.setY(itemHeight * -deltaIndex);
			}

			newBounds.setWidth(bounds.width());
			newBounds.setHeight(bounds.height() - headerSize * parentChildCount);
		}
	}

	return newBounds;
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
	if(m_isAnimating)
	{
		return true;
	}

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
			if(parent->property("activeIndex").toInt() != property("index").toInt())
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

void TreeItem::setActive()
{
	TreeItem* parent = getTreeParent();
	if(parent != nullptr)
	{
		parent->setActiveChild(this);
	}
}

bool TreeItem::isHwndValid()
{
	return m_hwnd != nullptr;
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

QVariant TreeItem::addChild(QString title, QString flow, QString layout, int monitorIndex, HWND hwnd)
{
	TreeItem* child = new TreeItem(this);

	child->setObjectName(title);
	child->setProperty("title", title);
	child->setProperty("flow", flow);
	child->setProperty("layout", layout);
	child->setProperty("monitorIndex", monitorIndex);
	child->setProperty("hwnd", QVariant::fromValue<HWND>(hwnd));

	addChild(child);

	return QVariant::fromValue<TreeItem*>(child);
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

void TreeItem::resetHwnd()
{
	m_hwnd = nullptr;
	hwndChanged();
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
	QPointF topLeft;

	TreeItem* candidate = this;
	do
	{
		topLeft += candidate->property("bounds").toRect().topLeft();
		topLeft += candidate->property("contentBounds").toRect().topLeft();
		candidate = candidate->getTreeParent();
	} while(candidate != nullptr);

	return topLeft;
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

void TreeItem::moveWindowOnscreen()
{
	WindowController* wc = getWindowController();
	wc->beginMoveWindows();
	moveWindowOnscreen_Internal();
	wc->endMoveWindows();
}

void TreeItem::moveWindowOffscreen()
{
	WindowController* wc = getWindowController();
	wc->beginMoveWindows();
	moveWindowOffscreen_Internal();
	wc->endMoveWindows();
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
	if(isHwndValid())
	{
		WindowView* wv = getWindowView();
		QPoint position = getIsVisible() ? getScreenPosition().toPoint() : wv->getOffscreenArea();

		WindowController* wc = getWindowController();
		wc->moveWindow(m_hwnd, position, property("contentBounds").toRect().size());
	}

	for(TreeItem* child : m_children)
	{
		child->moveWindowOnscreen_Internal();
	}
}

void TreeItem::moveWindowOffscreen_Internal()
{
	if(isHwndValid())
	{
		WindowController* wc = getWindowController();
		WindowView* wv = getWindowView();
		wc->moveWindow(m_hwnd, wv->getOffscreenArea());
	}

	for(TreeItem* child : m_children)
	{
		child->moveWindowOffscreen_Internal();
	}
}

void TreeItem::handleMonitorBoundsChanged()
{
	QScreen* monitor = getMonitor();
	if(monitor != nullptr)
	{
		QRect geo = monitor->geometry();
		if(m_itemWindow != nullptr)
		{
			m_itemWindow->setGeometry(geo);
		}

		if(m_headerWindow != nullptr)
		{
			m_headerWindow->setGeometry(geo);
		}
	}
}

void TreeItem::handleAnimatingChanged()
{
	if(m_isAnimating)
	{
		moveWindowOffscreen();
	}
	else
	{
		moveWindowOnscreen();
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

QDataStream& TreeItem::serialize(QDataStream& out) const
{
	out << objectName();
	out << m_flow;
	out << m_layout;
	out << m_monitorIndex;
	out << QVariant(qulonglong(m_hwnd));
	out << m_launchUri;
	out << m_launchParams;
	out << m_children.length();

	for(TreeItem* child : m_children)
	{
		out << child;
	}

	return out;
}

QDataStream& TreeItem::deserialize(QDataStream& in)
{
	QString title;
	QString flow;
	QString layout;
	int monitorIndex;
	QVariant hwnd;
	QString launchUri;
	QString launchParams;

	in >> title;
	in >> flow;
	in >> layout;
	in >> monitorIndex;
	in >> hwnd;
	in >> launchUri;
	in >> launchParams;

	setObjectName(title);
	m_flow = flow;
	m_layout = layout;

	m_monitorIndex = monitorIndex;
	if(m_monitorIndex != -1)
	{
		QScreen* monitor = QGuiApplication::screens()[m_monitorIndex];
		if(monitor != nullptr)
		{
			connect(monitor, SIGNAL(geometryChanged(const QRect&)), this, SLOT(handleMonitorBoundsChanged()));
		}

		AppCore* appCore = getAppCore();
		QmlController* qmlController = appCore->property("qmlController").value<QmlController*>();

		m_overlayWindow = qmlController->createWindow(QUrl("qrc:/qml/tree/OverlayWindow.qml"), QRect());
		m_overlayWindow->setColor(Qt::transparent);
		m_overlayWindow->setFlags(m_overlayWindow->flags() | static_cast<Qt::WindowFlags>(
			Qt::WA_TranslucentBackground |
			Qt::FramelessWindowHint |
			Qt::WindowStaysOnTopHint
		));
		m_overlayWindow->show();

		QQmlContext* rootContext = qmlController->getRootContext();
		QQmlContext* newContext = new QQmlContext(rootContext, this);
		newContext->setContextProperty("treeItem", this);
		newContext->setContextProperty("overlayWindow", m_overlayWindow);

		m_headerWindow = qmlController->createWindow(QUrl("qrc:/qml/tree/HeaderWindow.qml"), monitor->geometry(), newContext);
		m_headerWindow->setColor(Qt::transparent);
		m_headerWindow->setFlags(m_headerWindow->flags() | static_cast<Qt::WindowFlags>(
			Qt::WA_TranslucentBackground |
			Qt::FramelessWindowHint |
			Qt::WindowStaysOnBottomHint |
			Qt::WindowDoesNotAcceptFocus
		));
		m_headerWindow->show();

		m_itemWindow = qmlController->createWindow(QUrl("qrc:/qml/tree/ItemWindow.qml"), monitor->geometry(), newContext);
		m_itemWindow->setColor(Qt::transparent);
		m_itemWindow->setFlags(m_itemWindow->flags() | static_cast<Qt::WindowFlags>(
			Qt::WA_TranslucentBackground |
			Qt::FramelessWindowHint |
			Qt::WindowStaysOnBottomHint |
			Qt::WindowDoesNotAcceptFocus
		));
		m_itemWindow->show();
	}

	HWND loadedHwnd = HWND(hwnd.toULongLong());
	WindowView* wv = getWindowView();
	if(wv->hasWindowInfo(loadedHwnd))
	{
		m_hwnd = loadedHwnd;
	}

	m_launchUri = launchUri;
	m_launchParams = launchParams;

	int childCount;
	in >> childCount;
	for(int i = 0; i < childCount; ++i)
	{
		TreeItem* item = new TreeItem(this);
		in >> item;
		addChild(item);
	}

	connect(this, SIGNAL(boundsChanged()), this, SIGNAL(contentBoundsChanged()));
	connect(this, SIGNAL(boundsChanged()), this, SIGNAL(headerBoundsChanged()));

	connect(this, SIGNAL(activeIndexChanged()), this, SIGNAL(contentBoundsChanged()));
	connect(this, SIGNAL(flowChanged()), this, SIGNAL(contentBoundsChanged()));
	connect(this, SIGNAL(layoutChanged()), this, SIGNAL(contentBoundsChanged()));
	connect(this, SIGNAL(childrenChanged()), this, SIGNAL(boundsChanged()));

	connect(this, SIGNAL(hwndChanged()), this, SLOT(moveWindowOnscreen()));

	connect(this, SIGNAL(isAnimatingChanged()), this, SLOT(handleAnimatingChanged()));

	return in;
}

QDataStream& operator<<(QDataStream& out, const TreeItem* itemParams)
{
	return itemParams->serialize(out);
}

QDataStream& operator>>(QDataStream& in, TreeItem* itemParams)
{
	return itemParams->deserialize(in);
}
