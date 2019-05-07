#include "WindowManager.h"

#include <QApplication>
#include <QDebug>
#include <QDesktopWidget>
#include <QRect>
#include <QRegularExpression>
#include <QScreen>

#include "EnumWindowsThread.h"
#include "Win.h"

WindowManager::WindowManager(QObject* parent)
	: QObject(parent)
	, m_dwp(nullptr)
	, m_placeholder(new WindowInfo(this))
	, m_thread(new EnumWindowsThread(this))
{
	qRegisterMetaType<HWND>();
	qRegisterMetaType<QScreen*>();

	m_placeholder->setProperty("winTitle", "[Container]");

	connect(m_thread, SIGNAL(windowAdded(HWND, QString)), this, SLOT(onWindowAdded(HWND, QString)));
	connect(m_thread, SIGNAL(windowTitleChanged(HWND, QString)), this, SLOT(onWindowTitleChanged(HWND, QString)));
	connect(m_thread, SIGNAL(windowRemoved(HWND)), this, SLOT(onWindowRemoved(HWND)));
	connect(m_thread, SIGNAL(windowScanFinished()), this, SIGNAL(windowScanFinished()));

	m_thread->startProcess();
}

WindowManager::~WindowManager()
{
	m_thread->stopProcess();
	m_thread->wait();
}

HWND WindowManager::getWindowByRegex(const QString& titlePattern, const QString& classPattern)
{
	QRegularExpression titleRegex(titlePattern);
	QRegularExpression classRegex(classPattern);

	for(WindowInfo* wi : m_windowMap.values())
	{
		if(titleRegex.match(wi->getWinTitle()).hasMatch() && classRegex.match(wi->getWinClass()).hasMatch())
		{
			return wi->getHwnd();
		}
	}

	return nullptr;
}

QObjectList WindowManager::getWindowList()
{
	QObjectList objectList;
	objectList.append(m_placeholder);
	for(WindowInfo* wi : m_windowMap.values())
	{
		objectList.append(wi);
	}
	return objectList;
}

QObjectList WindowManager::getScreenList()
{
	QObjectList objectList;
	for(QScreen* screen : QGuiApplication::screens())
	{
		objectList.append(screen);
	}
	return objectList;
}

QPoint WindowManager::getOffscreenArea()
{
	QScreen* screen = QGuiApplication::screens()[0];
	return screen->virtualGeometry().bottomRight() + QPoint(1, 1);
}

bool WindowManager::hasWindowInfo(HWND hwnd)
{
	for(WindowInfo* wi : m_windowMap.values())
	{
		if(wi->getHwnd() == hwnd)
			return true;
	}

	return false;
}

WindowInfo* WindowManager::getWindowInfo(HWND hwnd)
{
	for(WindowInfo* wi : m_windowMap.values())
	{
		if(wi->getHwnd() == hwnd)
			return wi;
	}

	return nullptr;
}

QString WindowManager::getWindowTitle(HWND hwnd)
{
	WindowInfo* winInfo = getWindowInfo(hwnd);
	
	if(winInfo != nullptr)
	{
		return getWindowInfo(hwnd)->getWinTitle();
	}

	return nullptr;
}

QString WindowManager::getWindowClass(HWND hwnd)
{
	WindowInfo* winInfo = getWindowInfo(hwnd);
	
	if(winInfo != nullptr)
	{
		return getWindowInfo(hwnd)->getWinClass();
	}

	return nullptr;
}

HWND WindowManager::getWindowHwnd(QWindow* window)
{
	if(window != nullptr)
	{
		return reinterpret_cast<HWND>(window->winId());
	}

	return nullptr;
}

QPoint WindowManager::getWindowPosition(HWND hwnd)
{
	RECT rect;
	if(GetWindowRect(hwnd, &rect))
	{
		return QPoint(rect.left, rect.top);
	}

	return QPoint(0, 0);
}

QSize WindowManager::getWindowSize(HWND hwnd)
{
	RECT rect;
	if(GetWindowRect(hwnd, &rect))
	{
		return QSize(rect.right - rect.left, rect.bottom - rect.top);
	}

	return QSize(-1, -1);
}

void WindowManager::beginMoveWindows()
{
	m_dwp = BeginDeferWindowPos(0);
}

void WindowManager::showWindow(HWND hwnd)
{
	Q_ASSERT_X(hwnd != nullptr, "showWindow", "showWindow called with invalid HWND");

	ShowWindow(hwnd, SW_SHOW);
}

void WindowManager::hideWindow(HWND hwnd)
{
	Q_ASSERT_X(hwnd != nullptr, "hideWindow", "hideWindow called with invalid HWND");

	ShowWindow(hwnd, SW_HIDE);
}

bool WindowManager::isWindowVisible(HWND hwnd)
{
	return IsWindowVisible(hwnd);
}

void WindowManager::moveWindow(HWND hwnd, QPoint position, QSize size, int layer)
{
	Q_ASSERT_X(hwnd != nullptr, "moveWindow", "moveWindow called with invalid HWND");
	Q_ASSERT_X(m_dwp != nullptr, "moveWindow", "moveWindow called before beginMoveWindows");

	UINT flags = SWP_NOACTIVATE;
	if(size.isEmpty())
	{
		flags |= SWP_NOSIZE;
	}

	DeferWindowPos(m_dwp, hwnd, reinterpret_cast<HWND>(layer), position.x(), position.y(), size.width(), size.height(), flags);
}

void WindowManager::moveWindow(QWindow* window, QPoint position, QSize size, int layer)
{
	if(window == nullptr)
	{
		return;
	}

	moveWindow(getWindowHwnd(window), position, size, layer);
}

void WindowManager::endMoveWindows()
{
	EndDeferWindowPos(m_dwp);
	m_dwp = nullptr;
}

HWND WindowManager::findWindow(QString winTitle, QString winClass, HWND after, HWND parent)
{
	return FindWindowEx(parent, after, (LPCWSTR)winClass.utf16(), (LPCWSTR)winTitle.utf16());
}

void WindowManager::attachWindowToDesktop(QWindow* window)
{
	HWND windowHwnd = getWindowHwnd(window);
	SetParent(windowHwnd, FindWindowA("Progman", nullptr));
}

void WindowManager::detachWindowFromDesktop(QWindow* window)
{
	HWND windowHwnd = getWindowHwnd(window);
	SetParent(windowHwnd, nullptr);
}

void WindowManager::setBackgroundWindow(QWindow* window)
{
	HWND windowHwnd = getWindowHwnd(window);
	SetWindowLong(windowHwnd, GWL_EXSTYLE, GetWindowLong(windowHwnd, GWL_EXSTYLE) | WS_EX_NOACTIVATE | WS_EX_TOOLWINDOW);
	SetWindowPos(windowHwnd, HWND_BOTTOM, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_NOACTIVATE);
}

void WindowManager::setNonInteractiveWindow(QWindow* window)
{
	HWND windowHwnd = getWindowHwnd(window);
	SetWindowLong(windowHwnd, GWL_EXSTYLE, GetWindowLong(windowHwnd, GWL_EXSTYLE) | WS_EX_TRANSPARENT | WS_EX_LAYERED | WS_EX_TOOLWINDOW);
}

void WindowManager::setTransparentLayeredWindow(QWindow* window)
{
	HWND windowHwnd = getWindowHwnd(window);
	SetWindowLong(windowHwnd, GWL_EXSTYLE, GetWindowLong(windowHwnd, GWL_EXSTYLE) | WS_EX_TRANSPARENT | WS_EX_LAYERED | WS_EX_COMPOSITED);
}

void WindowManager::onWindowAdded(HWND hwnd, QString title)
{
	if(title == QCoreApplication::applicationName())
	{
		return;
	}

	wchar_t* wc = new wchar_t[256];
	GetClassName(hwnd, wc, 256);
	QString winClass = QString::fromStdWString(wc);
	if(winClass == QString("Progman"))
	{
		return;
	}

	WindowInfo* winInfo = new WindowInfo(this);
	winInfo->setProperty("hwnd", QVariant::fromValue<HWND>(hwnd));
	winInfo->setProperty("winTitle", title);
	winInfo->setProperty("winClass", winClass);

	m_windowMap.insert(title.toLower(), winInfo);
	emit windowListChanged();
}

void WindowManager::onWindowTitleChanged(HWND hwnd, QString newTitle)
{
	for(WindowInfo* wi : m_windowMap.values())
	{
		if(wi == m_placeholder) continue;
		
		if(wi->getHwnd() == hwnd)
		{
			m_windowMap.remove(wi->getWinTitle().toLower());
			wi->setProperty("winTitle", newTitle);
			m_windowMap.insert(newTitle.toLower(), wi);
			emit windowListChanged();
			break;
		}
	}
}

void WindowManager::onWindowRemoved(HWND hwnd)
{
	for(WindowInfo* wi : m_windowMap.values())
	{
		if(wi == m_placeholder) continue;

		if(wi->getHwnd() == hwnd)
		{
			m_windowMap.remove(wi->getWinTitle().toLower());
		}
	}
	emit windowListChanged();
}
