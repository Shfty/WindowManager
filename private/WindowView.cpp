#include "WindowView.h"

#include <QApplication>
#include <QDebug>
#include <QDesktopWidget>
#include <QRect>
#include <QRegularExpression>
#include <QScreen>

#include "EnumWindowsThread.h"
#include "Win.h"

WindowView::WindowView(QObject* parent)
	: QObject(parent)
	, m_dwp(nullptr)
	, m_placeholder(new WindowInfo(this))
	, m_thread(new EnumWindowsThread(this))
{
	qRegisterMetaType<WindowView*>();
	setObjectName("Window Controller");

	m_placeholder->setProperty("winTitle", "[Container]");

	connect(m_thread, SIGNAL(windowAdded(HWND, QString)), this, SLOT(onWindowAdded(HWND, QString)));
	connect(m_thread, SIGNAL(windowTitleChanged(HWND, QString)), this, SLOT(onWindowTitleChanged(HWND, QString)));
	connect(m_thread, SIGNAL(windowRemoved(HWND)), this, SLOT(onWindowRemoved(HWND)));
	connect(m_thread, SIGNAL(windowScanFinished()), this, SIGNAL(windowScanFinished()));

	m_thread->startProcess();
}

WindowView::~WindowView()
{
	m_thread->stopProcess();
	m_thread->wait();
}

HWND WindowView::getWindowByRegex(const QString& titlePattern, const QString& classPattern)
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

QObjectList WindowView::getWindowList()
{
	QObjectList objectList;
	objectList.append(m_placeholder);
	for(WindowInfo* wi : m_windowMap.values())
	{
		objectList.append(wi);
	}
	return objectList;
}

QObjectList WindowView::getScreenList()
{
	QObjectList objectList;
	for(QScreen* screen : QGuiApplication::screens())
	{
		objectList.append(screen);
	}
	return objectList;
}

QPoint WindowView::getOffscreenArea()
{
	QScreen* screen = QGuiApplication::screens()[0];
	return screen->virtualGeometry().bottomRight() + QPoint(1, 1);
}

bool WindowView::hasWindowInfo(HWND hwnd)
{
	for(WindowInfo* wi : m_windowMap.values())
	{
		if(wi->getHwnd() == hwnd)
			return true;
	}

	return false;
}

WindowInfo* WindowView::getWindowInfo(HWND hwnd)
{
	for(WindowInfo* wi : m_windowMap.values())
	{
		if(wi->getHwnd() == hwnd)
			return wi;
	}

	return nullptr;
}

HWND WindowView::getWindowHwnd(QWindow* window)
{
	if(window != nullptr)
	{
		return reinterpret_cast<HWND>(window->winId());
	}

	return nullptr;
}

bool WindowView::isWindowVisible(HWND hwnd)
{
	return IsWindowVisible(hwnd);
}

HWND WindowView::findWindow(QString winTitle, QString winClass, HWND after, HWND parent)
{
	return FindWindowEx(parent, after, (LPCWSTR)winClass.utf16(), (LPCWSTR)winTitle.utf16());
}

void WindowView::onWindowAdded(HWND hwnd, QString title)
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

void WindowView::onWindowTitleChanged(HWND hwnd, QString newTitle)
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

void WindowView::onWindowRemoved(HWND hwnd)
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
