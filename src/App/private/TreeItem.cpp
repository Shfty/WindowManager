#include "TreeItem.h"

#include <QDesktopServices>
#include <QProcess>
#include <QUrl>
#include <QJsonArray>

#include <QDebug>
Q_LOGGING_CATEGORY(treeItem, "app.treeItem")

#include <WindowInfo.h>
#include <WindowView.h>

#include "TreeModel.h"
#include "AppCore.h"
#include "Win.h"

TreeItem::TreeItem(QObject* parent)
	: QObject(parent)
	, m_windowInfo(nullptr)
	, m_borderless(false)
	, m_autoLaunch(false)
	, m_startupComplete(false)
	, m_activeChild(nullptr)
	, m_wantsAutoLaunch(false)
{
	setObjectName("Tree Item");

	qCInfo(treeItem) << "Startup";

	Q_ASSERT(parent != nullptr);

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
		}
	});

	TreeModel* tm = TreeModel::getInstance(this);
	connect(this, SIGNAL(setWindowStyle(HWND, qint32)), tm, SLOT(setWindowStyle(HWND, qint32)));
}

void TreeItem::cleanup()
{
	qCInfo(treeItem) << objectName() << "cleaning up";

	if(m_windowInfo != nullptr)
	{
		disconnect(m_windowInfo, SIGNAL(windowClosed()));
		restoreWindowStyle();
	}

	for(TreeItem* child : m_children)
	{
		child->cleanup();
	}
}

void TreeItem::setupWindow(WindowObject* wi)
{
	if(m_windowInfo)
	{
		connect(wi, &WindowObject::windowClosed, [=](){
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

QVariant TreeItem::addChild(QString title, QString flow, QString layout, WindowObject* windowInfo)
{
	TreeItem* child = new TreeItem(this);

	child->setObjectName(title);
	child->setProperty("title", title);
	child->setProperty("flow", flow);
	child->setProperty("layout", layout);
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
	displacedChild->indexChanged();

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

void TreeItem::setWindowInfo(WindowObject* newWindowInfo)
{
	if(m_windowInfo != nullptr && m_windowInfo->getHwnd() != nullptr)
	{
		restoreWindowStyle();
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

void TreeItem::restoreWindowStyle()
{
	emit setWindowStyle(m_windowInfo->getHwnd(), m_windowInfo->getWinStyle());
}

void TreeItem::tryAutoGrabWindow()
{
	if(m_children.length() > 0) return;
	if(m_autoGrabTitle.isEmpty() && m_autoGrabClass.isEmpty()) return;

	//qCInfo(treeItem) << objectName() << "trying auto grab";

	WindowView* wv = getWindowView();
	WindowObject* foundWindow = wv->getWindowByRegex(m_autoGrabTitle, m_autoGrabClass);

	if(foundWindow != nullptr)
	{
		//qCInfo(treeItem) << objectName() << "auto grab successful";
		setWindowInfo(foundWindow);
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

WindowView* TreeItem::getWindowView()
{
	AppCore* appCore = AppCore::getInstance(this);
	Q_ASSERT(appCore != nullptr);

	return appCore->getWindowView();
}
