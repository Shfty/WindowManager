#include "WindowManager.h"

#include <QApplication>
#include <QDebug>
#include <QDesktopWidget>
#include <QRect>
#include <QRegularExpression>
#include <QScreen>
#include <QTimer>

#include "EnumWindowsThread.h"

WindowManager::WindowManager(QObject* parent)
	: QObject(parent)
	, m_dwp(nullptr)
	, m_thread(new EnumWindowsThread(this))
{
	qRegisterMetaType<HWND>();
	qRegisterMetaType<WindowInfo>();

	connect(m_thread, SIGNAL(windowAdded(WindowInfo)), this, SLOT(onWindowAdded(WindowInfo)));
	connect(m_thread, SIGNAL(windowChanged(WindowInfo)), this, SLOT(onWindowChanged(WindowInfo)));
	connect(m_thread, SIGNAL(windowRemoved(WindowInfo)), this, SLOT(onWindowRemoved(WindowInfo)));
	connect(m_thread, SIGNAL(windowScanFinished()), this, SIGNAL(windowScanFinished()));

	m_thread->startProcess();
}

WindowManager::~WindowManager()
{
}

HWND WindowManager::getWindowByRegex(const QString& titlePattern, const QString& classPattern)
{
	QRegularExpression titleRegex(titlePattern);
	QRegularExpression classRegex(classPattern);

	for(WindowInfo wi : m_windowMap.values())
	{
		if(titleRegex.match(wi.winTitle).hasMatch() && classRegex.match(wi.winClass).hasMatch())
		{
			return wi.hwnd;
		}
	}

	return nullptr;
}

QStringList WindowManager::getWindowStringList()
{
	QStringList stringList;

	for(WindowInfo v : m_windowMap)
	{
		stringList.append(v.winTitle);
	}

	return stringList;
}

QVariantMap WindowManager::getWindowList()
{
	QVariantMap windowList;

	QVariantList hwnds;
	QVariantList titles;

	for(WindowInfo wi : m_windowMap.values())
	{
		hwnds.append(QVariant::fromValue(wi.hwnd));
		titles.append(QVariant::fromValue(wi.winTitle));
	}

	windowList.insert("hwnds", hwnds);
	windowList.insert("titles", titles);

	return windowList;
}

QRect WindowManager::getDesktopRect()
{
	QRect desktopRect;

	QList<QScreen*> screens = QApplication::screens();
	for(int i = 0; i < screens.length(); ++i)
	{
		QScreen* screen = screens.at(i);
		desktopRect |= screen->geometry();
	}

	return desktopRect;
}

QRect WindowManager::getDesktopWorkArea()
{
	QRect desktopRect;

	QList<QScreen*> screens = QApplication::screens();
	for(int i = 0; i < screens.length(); ++i)
	{
		QScreen* screen = screens.at(i);
		desktopRect |= screen->availableGeometry();
	}

	return desktopRect;
}

QPoint WindowManager::getOffscreenArea()
{
	return getDesktopRect().bottomRight() + QPoint(1, 1);
}

bool WindowManager::hasWindowInfo(HWND hwnd)
{
	for(WindowInfo wi : m_windowMap.values())
	{
		if(wi.hwnd == hwnd)
			return true;
	}

	return false;
}

WindowInfo WindowManager::getWindowInfo(HWND hwnd)
{
	for(WindowInfo wi : m_windowMap.values())
	{
		if(wi.hwnd == hwnd)
			return wi;
	}

	return WindowInfo();
}

QString WindowManager::getWindowTitle(HWND hwnd)
{
	return getWindowInfo(hwnd).winTitle;
}

QString WindowManager::getWindowClass(HWND hwnd)
{
	return getWindowInfo(hwnd).winClass;
}

HWND WindowManager::getWindowHwnd(QWindow* window)
{
	return reinterpret_cast<HWND>(window->winId());
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

void WindowManager::moveWindow(HWND hwnd, QPoint position, QSize size)
{
	Q_ASSERT_X(hwnd != nullptr, "moveWindow", "moveWindow called with invalid HWND");
	Q_ASSERT_X(m_dwp != nullptr, "moveWindow", "moveWindow called before beginMoveWindows");

	UINT flags = SWP_NOACTIVATE;
	if(size.isEmpty())
	{
		flags |= SWP_NOSIZE;
	}

	DeferWindowPos(m_dwp, hwnd, HWND_NOTOPMOST, position.x(), position.y(), size.width(), size.height(), flags);
}

void WindowManager::moveWindow(QWindow* window, QPoint position, QSize size)
{
	moveWindow(getWindowHwnd(window), position, size);
}

void WindowManager::endMoveWindows()
{
	EndDeferWindowPos(m_dwp);
	m_dwp = nullptr;
}

HWND WindowManager::findWindow(QString winTitle, QString winClass)
{
	return FindWindowA(winClass.toLocal8Bit().constData(), winTitle.toLocal8Bit().constData());
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
	SetWindowLong(windowHwnd, GWL_EXSTYLE, GetWindowLong(windowHwnd, GWL_EXSTYLE) | WS_EX_TRANSPARENT | WS_EX_NOACTIVATE | WS_EX_TOOLWINDOW);
	SetWindowPos(windowHwnd, HWND_BOTTOM, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_NOACTIVATE);
}

void WindowManager::onWindowAdded(WindowInfo windowInfo)
{
	m_windowMap.insert(windowInfo.winTitle.toLower(), windowInfo);
	emit windowStringListChanged();
	emit windowListChanged();
}

void WindowManager::onWindowChanged(WindowInfo windowInfo)
{
	for(WindowInfo w : m_windowMap.values())
	{
		if(w.hwnd == windowInfo.hwnd)
		{
			m_windowMap.remove(w.winTitle.toLower());
			m_windowMap.insert(windowInfo.winTitle.toLower(), windowInfo);
		}
	}
	emit windowStringListChanged();
	emit windowListChanged();
}

void WindowManager::onWindowRemoved(WindowInfo windowInfo)
{
	for(WindowInfo w : m_windowMap.values())
	{
		if(w.hwnd == windowInfo.hwnd)
		{
			m_windowMap.remove(w.winTitle.toLower());
		}
	}
	emit windowStringListChanged();
	emit windowListChanged();
}
