#include "TreeItem.h"

#include "Settings.h"
#include "QMLApplication.h"
#include "WindowManager.h"

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
	: QObject(parent)
	, m_hwnd(nullptr)
	, m_activeChild(nullptr)
	, m_monitorIndex(-1)
	, m_itemWindow(nullptr)
	, m_headerWindow(nullptr)
	, m_overlayWindow(nullptr)
	, m_isAnimating(false)
{
	setObjectName("Tree Item");
}

TreeItem::~TreeItem()
{
	if(m_itemWindow != nullptr)
	{
		m_itemWindow->deleteLater();
	}

	if(m_headerWindow != nullptr)
	{
		m_headerWindow->deleteLater();
	}

	if(m_overlayWindow != nullptr)
	{
		m_overlayWindow->deleteLater();
	}
}

QScreen* TreeItem::getMonitor()
{
	if(m_monitorIndex != -1)
	{
		return QGuiApplication::screens()[m_monitorIndex];
	}

	TreeItem* parent = this->getTreeParent();
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

	TreeItem* parent = this->getTreeParent();
	if(parent == nullptr)
	{
		return QRectF(0, 0, -1, -1);
	}

	QRectF parentContentBounds = parent->calculateContentBounds();
	QRectF newBounds;
	newBounds.setWidth(parentContentBounds.width());
	newBounds.setHeight(parentContentBounds.height());
	return newBounds.marginsRemoved(QMargins(Settings::instance().getItemMargin(), Settings::instance().getItemMargin(), Settings::instance().getItemMargin(), Settings::instance().getItemMargin()));
}

QRectF TreeItem::calculateHeaderBounds()
{
	QRectF bounds = this->calculateBounds();

	if(m_monitorIndex != -1)
	{
		QRectF headerBounds;
		headerBounds.setWidth(bounds.width());
		headerBounds.setHeight(Settings::instance().getHeaderSize());
		return headerBounds;
	}

	TreeItem* parent = this->getTreeParent();
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
		headerBounds.setX(index * newWidth);
		headerBounds.setWidth(newWidth);
		headerBounds.setHeight(Settings::instance().getHeaderSize());

		if(parentLayout == "Split")
		{
			QMargins margins = QMargins(index > 0 ? Settings::instance().getItemMargin() / 2 : 0, 0, index < parentChildCount - 1 ? Settings::instance().getItemMargin() / 2 : 0, 0);
			headerBounds = headerBounds.marginsRemoved(margins);
		}
	}
	else
	{
		if(parentLayout == "Split")
		{
			qreal newHeight = bounds.height() / parentChildCount;
			headerBounds.setY(index * newHeight);
			if(index > 0)
			{
				headerBounds.setY(headerBounds.y() + Settings::instance().getItemMargin() / 2);
			}
			headerBounds.setWidth(bounds.width());
			headerBounds.setHeight(Settings::instance().getHeaderSize());
		}
		else
		{
			headerBounds.setY(index * Settings::instance().getHeaderSize());
			headerBounds.setWidth(bounds.width());
			headerBounds.setHeight(Settings::instance().getHeaderSize());
		}
	}

	return headerBounds;
}

QRectF TreeItem::calculateContentBounds()
{
	QRectF bounds = this->calculateBounds();

	if(m_monitorIndex != -1)
	{
		QRectF newBounds;
		newBounds.setWidth(bounds.width());
		newBounds.setHeight(bounds.height());
		newBounds.setY(Settings::instance().getHeaderSize());
		return newBounds;
	}

	TreeItem* parent = this->getTreeParent();
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
	if(parentLayout == "Split")
	{
		if(parentFlow == "Horizontal")
		{
			qreal newWidth = bounds.width() / parentChildCount;
			newBounds.setX(index * newWidth);
			newBounds.setY(Settings::instance().getHeaderSize());
			newBounds.setWidth(newWidth);
			newBounds.setHeight(bounds.height() - Settings::instance().getHeaderSize());

			QMargins margins = QMargins(index > 0 ? Settings::instance().getItemMargin() / 2 : 0, 0, index < parentChildCount - 1 ? Settings::instance().getItemMargin() / 2 : 0, 0);
			newBounds = newBounds.marginsRemoved(margins);
		}
		else
		{
			qreal newHeight = bounds.height() / parentChildCount;
			newBounds.setY(index * newHeight + Settings::instance().getHeaderSize());
			newBounds.setWidth(bounds.width());
			newBounds.setHeight(newHeight - Settings::instance().getHeaderSize());

			QMargins margins = QMargins(0, index > 0 ? Settings::instance().getItemMargin() / 2 : 0, 0, index < parentChildCount - 1 ? Settings::instance().getItemMargin() / 2 : 0);
			newBounds = newBounds.marginsRemoved(margins);
		}
	}
	else
	{
		int deltaIndex = parentActiveIndex - index;
		if(parentFlow == "Horizontal")
		{
			qreal itemWidth = bounds.width() + Settings::instance().getItemMargin();
			newBounds.setX(itemWidth * -deltaIndex);
			newBounds.setY(Settings::instance().getHeaderSize());

			newBounds.setWidth(bounds.width());
			newBounds.setHeight(bounds.height() - Settings::instance().getHeaderSize());
		}
		else
		{
			qreal itemHeight = bounds.height() + Settings::instance().getItemMargin();
			newBounds.setX(0);

			if(index == parentActiveIndex)
			{
				newBounds.setY(Settings::instance().getHeaderSize() * parentChildCount);
			}
			else
			{
				newBounds.setY(itemHeight * -deltaIndex);
			}

			newBounds.setWidth(bounds.width());
			newBounds.setHeight(bounds.height() - Settings::instance().getHeaderSize() * parentChildCount);
		}
	}

	return newBounds;
}

int TreeItem::getIndex()
{
	TreeItem* parent = this->getTreeParent();
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
	TreeItem* parent = this->getTreeParent();
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
	TreeItem* child = new TreeItem();

	child->setObjectName(title);
	child->setProperty("title", title);
	child->setProperty("flow", flow);
	child->setProperty("layout", layout);
	child->setProperty("monitorIndex", monitorIndex);
	child->setProperty("hwnd", QVariant::fromValue<HWND>(hwnd));
	child->setParent(this);

	if(m_activeChild == nullptr)
	{
		m_activeChild = child;
	}

	return QVariant::fromValue<TreeItem*>(child);
}

void TreeItem::resetHwnd()
{
	m_hwnd = nullptr;
	hwndChanged();
}

void TreeItem::moveUp()
{
	TreeItem* parent = this->getTreeParent();
	if(parent != nullptr)
	{
		parent->moveChild(this, -1);
	}
}

void TreeItem::moveDown()
{
	TreeItem* parent = this->getTreeParent();
	if(parent != nullptr)
	{
		parent->moveChild(this, 1);
	}
}

void TreeItem::remove()
{
	this->setParent(nullptr);
	this->deleteLater();
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

void TreeItem::childEvent(QChildEvent* event)
{
	QObject::childEvent(event);

	TreeItem* treeChild = qobject_cast<TreeItem*>(event->child());
	int removedIndex = -1;

	if(treeChild != nullptr)
	{
		switch(event->type())
		{
			case QChildEvent::ChildAdded:
				m_children.append(treeChild);
				childAdded(m_children.length(), treeChild);

				if(m_children.length() == 1)
				{
					m_activeChild = treeChild;
					activeIndexChanged();
				}

				childrenChanged();

				break;
			case QChildEvent::ChildRemoved:
				if(m_children.length() > 1)
				{
					int removedIndex = m_children.indexOf(treeChild);
					if(removedIndex > 0)
					{
						this->setActiveChild(m_children.at(removedIndex - 1));
					}
					else
					{
						this->setActiveChild(m_children.at(removedIndex + 1));
					}
				}

				removedIndex = m_children.indexOf(treeChild);
				m_children.removeOne(treeChild);
				childRemoved(removedIndex, treeChild);

				if(m_children.length() == 0)
				{
					m_activeChild = nullptr;
					activeIndexChanged();
				}

				childrenChanged();
				break;
			default:
				break;
		}
	}
}

void TreeItem::moveWindowOnscreen()
{
	WindowManager& wm = WindowManager::instance();
	wm.beginMoveWindows();
	moveWindowOnscreen_Internal();
	wm.endMoveWindows();
}

void TreeItem::moveWindowOffscreen()
{
	WindowManager& wm = WindowManager::instance();
	wm.beginMoveWindows();
	moveWindowOffscreen_Internal();
	wm.endMoveWindows();
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
	if(this->isHwndValid())
	{
		WindowManager& wm = WindowManager::instance();
		QPoint position = getIsVisible() ? getScreenPosition().toPoint() : wm.getOffscreenArea();
		wm.moveWindow(m_hwnd, position, property("contentBounds").toRect().size());
	}

	for(TreeItem* child : m_children)
	{
		child->moveWindowOnscreen_Internal();
	}
}

void TreeItem::moveWindowOffscreen_Internal()
{
	if(this->isHwndValid())
	{
		WindowManager& wm = WindowManager::instance();
		wm.moveWindow(m_hwnd, wm.getOffscreenArea());
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
		
		QMLApplication& qmlApp = QMLApplication::instance();

		m_overlayWindow = qmlApp.createWindow(QUrl("qrc:/qml/tree/OverlayWindow.qml"), QRect());
		m_overlayWindow->setColor(Qt::transparent);
		m_overlayWindow->setFlags(m_overlayWindow->flags() | static_cast<Qt::WindowFlags>(
			Qt::WA_TranslucentBackground |
			Qt::FramelessWindowHint |
			Qt::WindowStaysOnTopHint
		));
		m_overlayWindow->show();

		QQmlContext* newContext = new QQmlContext(qmlApp.getRootContext(), qmlApp.getRootContext());
		newContext->setContextProperty("treeItem", this);
		newContext->setContextProperty("overlayWindow", m_overlayWindow);

		m_headerWindow = qmlApp.createWindow(QUrl("qrc:/qml/tree/HeaderWindow.qml"), monitor->geometry(), newContext);
		m_headerWindow->setColor(Qt::transparent);
		m_headerWindow->setFlags(m_headerWindow->flags() | static_cast<Qt::WindowFlags>(
			Qt::WA_TranslucentBackground |
			Qt::FramelessWindowHint |
			Qt::WindowStaysOnBottomHint |
			Qt::WindowDoesNotAcceptFocus
		));
		m_headerWindow->show();

		m_itemWindow = qmlApp.createWindow(QUrl("qrc:/qml/tree/ItemWindow.qml"), monitor->geometry(), newContext);
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
	if(WindowManager::instance().hasWindowInfo(loadedHwnd))
	{
		m_hwnd = loadedHwnd;
	}

	m_launchUri = launchUri;
	m_launchParams = launchParams;

	int childCount;
	in >> childCount;
	for(int i = 0; i < childCount; ++i)
	{
		TreeItem* item = new TreeItem(nullptr);
		in >> item;
		item->setParent(this);
		connect(this, SIGNAL(contentBoundsChanged()), item, SIGNAL(boundsChanged()));
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
