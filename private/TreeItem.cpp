#include "TreeItem.h"

#include "WindowManager.h"

#include <QDir>
#include <QDebug>
#include <QUrl>
#include <QProcess>
#include <QDesktopServices>

// TODO: Convert into member properties
#define HEADER_SIZE 30
#define ITEM_MARGIN 10

TreeItem::TreeItem(QObject *parent)
	: QObject(parent),
	  m_hwnd(nullptr),
	  m_activeChild(nullptr),
	  m_refreshRate(-1),
	  m_process(new QProcess())
{
	connect(this, SIGNAL(boundsChanged()), this, SLOT(updateChildBounds()));
	connect(this, SIGNAL(activeIndexChanged()), this, SLOT(updateChildBounds()));
	connect(this, SIGNAL(flowChanged()), this, SLOT(updateChildBounds()));
	connect(this, SIGNAL(layoutChanged()), this, SLOT(updateChildBounds()));

	connect(this, SIGNAL(hwndChanged()), this, SLOT(moveWindowOnscreen()));

	connect(this, SIGNAL(isAnimatingChanged()), this, SLOT(handleAnimatingChanged()));
}

TreeItem::~TreeItem()
{
}

QRectF TreeItem::calculateBounds()
{
	if (this->isBoundsValid())
	{
		return m_bounds;
	}

	TreeItem *parent = this->getTreeParent();
	if (parent == nullptr)
	{
		return QRectF(0, 0, -1, -1);
	}

	QRectF parentContentBounds = parent->calculateContentBounds();
	QRectF newBounds;
	newBounds.setWidth(parentContentBounds.width());
	newBounds.setHeight(parentContentBounds.height());
	return newBounds.marginsRemoved(QMargins(ITEM_MARGIN, ITEM_MARGIN, ITEM_MARGIN, ITEM_MARGIN));
}

QRectF TreeItem::calculateHeaderBounds()
{
	QRectF bounds = this->calculateBounds();

	if (this->isBoundsValid())
	{
		QRectF headerBounds;
		headerBounds.setWidth(bounds.width());
		headerBounds.setHeight(HEADER_SIZE);
		return headerBounds;
	}

	TreeItem *parent = this->getTreeParent();
	if (parent == nullptr)
	{
		return QRectF(0, 0, -1, -1);
	}

	QObjectList parentChildren = parent->getTreeChildren();
	int index = parentChildren.indexOf(this);
	int parentChildCount = parentChildren.length();
	QString parentFlow = parent->property("flow").toString();
	QString parentLayout = parent->property("layout").toString();

	QRectF headerBounds;
	if (parentFlow == "Horizontal")
	{
		qreal newWidth = bounds.width() / parentChildCount;
		headerBounds.setX(index * newWidth);
		headerBounds.setWidth(newWidth);
		headerBounds.setHeight(HEADER_SIZE);

		if (parentLayout == "Split")
		{
			QMargins margins = QMargins(index > 0 ? ITEM_MARGIN / 2 : 0, 0, index < parentChildCount - 1 ? ITEM_MARGIN / 2 : 0, 0);
			headerBounds = headerBounds.marginsRemoved(margins);
		}
	}
	else
	{
		if (parentLayout == "Split")
		{
			qreal newHeight = bounds.height() / parentChildCount;
			headerBounds.setY(index * newHeight);
			if(index > 0)
			{
				headerBounds.setY(headerBounds.y() + ITEM_MARGIN / 2);
			}
			headerBounds.setWidth(bounds.width());
			headerBounds.setHeight(HEADER_SIZE);
		}
		else
		{
			headerBounds.setY(index * HEADER_SIZE);
			headerBounds.setWidth(bounds.width());
			headerBounds.setHeight(HEADER_SIZE);
		}
	}

	return headerBounds;
}

QRectF TreeItem::calculateContentBounds()
{
	QRectF bounds = this->calculateBounds();

	if (this->isBoundsValid())
	{
		QRectF newBounds;
		newBounds.setWidth(bounds.width());
		newBounds.setHeight(bounds.height());
		newBounds.setY(HEADER_SIZE);
		return newBounds;
	}

	TreeItem *parent = this->getTreeParent();
	if (parent == nullptr)
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
	if (parentLayout == "Split")
	{
		if (parentFlow == "Horizontal")
		{
			qreal newWidth = bounds.width() / parentChildCount;
			newBounds.setX(index * newWidth);
			newBounds.setY(HEADER_SIZE);
			newBounds.setWidth(newWidth);
			newBounds.setHeight(bounds.height() - HEADER_SIZE);

			QMargins margins = QMargins(index > 0 ? ITEM_MARGIN / 2 : 0, 0, index < parentChildCount - 1 ? ITEM_MARGIN / 2 : 0, 0);
			newBounds = newBounds.marginsRemoved(margins);
		}
		else
		{
			qreal newHeight = bounds.height() / parentChildCount;
			newBounds.setY(index * newHeight + HEADER_SIZE);
			newBounds.setWidth(bounds.width());
			newBounds.setHeight(newHeight - HEADER_SIZE);

			QMargins margins = QMargins(0, index > 0 ? ITEM_MARGIN / 2 : 0, 0, index < parentChildCount - 1 ? ITEM_MARGIN / 2 : 0);
			newBounds = newBounds.marginsRemoved(margins);
		}
	}
	else
	{
		if (parentFlow == "Horizontal")
		{
			if(index > parentActiveIndex)
			{
				newBounds.setX(bounds.width() + ITEM_MARGIN);
			}
			else if(index == parentActiveIndex)
			{
				newBounds.setX(0);
			}
			else
			{
				newBounds.setX(-(bounds.width() + ITEM_MARGIN));
			}
			newBounds.setY(HEADER_SIZE);

			newBounds.setWidth(bounds.width());
			newBounds.setHeight(bounds.height() - HEADER_SIZE);
		}
		else
		{
			newBounds.setX(0);
			if(index > parentActiveIndex)
			{
				newBounds.setY(bounds.height() + ITEM_MARGIN);
			}
			else if(index == parentActiveIndex)
			{
				newBounds.setY(HEADER_SIZE * parentChildCount);
			}
			else
			{
				newBounds.setY(-(bounds.height() + ITEM_MARGIN));
			}

			newBounds.setWidth(bounds.width());
			newBounds.setHeight(bounds.height() - HEADER_SIZE * parentChildCount);
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
	TreeItem* parent = getTreeParent();
	if(parent != nullptr && !parent->getIsVisible())
	{
		return false;
	}

	if(m_isAnimating)
	{
		return true;
	}

	if(getTreeParent()->property("layout").toString() == "Split")
	{
		return true;
	}

	if(property("index").toInt() == getTreeParent()->property("activeIndex").toInt())
	{
		return true;
	}

	return false;
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

bool TreeItem::isBoundsValid()
{
	return m_bounds.isValid();
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

QVariant TreeItem::addChild(QString title, QString flow, QString layout, QRectF bounds, qreal refreshRate, HWND hwnd)
{
	TreeItem *child = new TreeItem();

	child->setProperty("title", title);
	child->setProperty("flow", flow);
	child->setProperty("layout", layout);
	child->setProperty("bounds", bounds);
	child->setProperty("refreshRate", refreshRate);
	child->setProperty("hwnd", QVariant::fromValue<HWND>(hwnd));
	child->setParent(this);

	if(m_activeChild == nullptr)
	{
		m_activeChild = child;
	}

	return QVariant::fromValue<TreeItem *>(child);
}

void TreeItem::resetHwnd()
{
	m_hwnd = nullptr;
	hwndChanged();
}

void TreeItem::moveUp()
{
	TreeItem *parent = this->getTreeParent();
	if (parent != nullptr)
	{
		parent->moveChild(this, -1);
	}
}

void TreeItem::moveDown()
{
	TreeItem *parent = this->getTreeParent();
	if (parent != nullptr)
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

TreeItem *TreeItem::getTreeParent() const
{
	return qobject_cast<TreeItem *>(parent());
}

QObjectList TreeItem::getTreeChildren() const
{
	QList<TreeItem *> children = m_children;
	QObjectList treeChildren;

	for (QObject *o : children)
	{
		treeChildren.append(o);
	}

	return treeChildren;
}

void TreeItem::moveChild(TreeItem *child, int delta)
{
	if (delta == 0)
		return;

	int childIndex = m_children.indexOf(child);

	if (childIndex < 0)
		return;

	int targetIndex = childIndex + delta;

	if (targetIndex < 0)
		return;
	if (targetIndex >= m_children.length())
		return;

	TreeItem* displacedChild = m_children.at(targetIndex);

	m_children.removeAt(childIndex);
	m_children.insert(targetIndex, child);

	child->indexChanged();
	displacedChild->indexChanged();
	childrenChanged();

	this->updateChildBounds();
}

void TreeItem::childEvent(QChildEvent *event)
{
	QObject::childEvent(event);

	TreeItem *treeChild = qobject_cast<TreeItem *>(event->child());
	int removedIndex = -1;

	if (treeChild != nullptr)
	{
		switch (event->type())
		{
		case QChildEvent::ChildAdded:
			m_children.append(treeChild);
			childAdded(m_children.length(), treeChild);
			childrenChanged();

			if(m_children.length() == 1)
			{
				m_activeChild = treeChild;
				activeIndexChanged();
			}

			updateChildBounds();

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
			childrenChanged();

			if(m_children.length() == 0)
			{
				m_activeChild = nullptr;
				activeIndexChanged();
			}

			updateChildBounds();
			break;
		default:
			break;
		}
	}
}

void TreeItem::updateChildBounds()
{
	this->headerBoundsChanged();
	this->contentBoundsChanged();

	for (TreeItem *child : m_children)
	{
		child->boundsChanged();
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

QDataStream &TreeItem::serialize(QDataStream &out) const
{
	out << m_title;
	out << m_flow;
	out << m_layout;
	out << m_bounds;
	out << m_refreshRate;
	out << QVariant(qulonglong(m_hwnd));
	out << m_launchUri;
	out << m_launchParams;
	out << m_children.length();

	for (TreeItem* child : m_children)
	{
		out << child;
	}

	return out;
}

QDataStream &TreeItem::deserialize(QDataStream &in)
{
	QString title;
	QString flow;
	QString layout;
	QRectF bounds;
	qreal refreshRate;
	QVariant hwnd;
	QString launchUri;
	QString launchParams;

	in >> title;
	in >> flow;
	in >> layout;
	in >> bounds;
	in >> refreshRate;
	in >> hwnd;
	in >> launchUri;
	in >> launchParams;

	m_title = title;
	m_flow = flow;
	m_layout = layout;
	m_bounds = bounds;
	m_refreshRate = refreshRate;

	HWND loadedHwnd = HWND(hwnd.toULongLong());
	if (WindowManager::instance().hasWindowInfo(loadedHwnd))
	{
		m_hwnd = loadedHwnd;
	}

	m_launchUri = launchUri;
	m_launchParams = launchParams;
	
	int childCount;
	in >> childCount;
	for (int i = 0; i < childCount; ++i)
	{
		TreeItem* item = new TreeItem(nullptr);
		in >> item;
		item->setParent(this);
	}

	return in;
}

QDataStream &operator<<(QDataStream &out, const TreeItem* itemParams)
{
	return itemParams->serialize(out);
}

QDataStream &operator>>(QDataStream &in, TreeItem* itemParams)
{
	return itemParams->deserialize(in);
}
