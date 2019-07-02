#include "WindowView.h"

#include <QApplication>
#include <QDesktopWidget>
#include <QRect>
#include <QRegularExpression>
#include <QScreen>
#include <QTextStream>

#include <QDebug>
Q_LOGGING_CATEGORY(windowView, "shared.windowView")

#include "Win.h"

WindowView::WindowView(QObject* parent)
	: QObject(parent)
	, m_dwp(nullptr)
	, m_placeholder(new WindowObject(this))
{
	qRegisterMetaType<WindowView*>();
	setObjectName("Window View");

	m_placeholder->setHwnd(HWND(-1));
	m_placeholder->setWinTitle("[ Container ]");
}

WindowObject* WindowView::getWindowByRegex(const QString& titlePattern, const QString& classPattern)
{
	QRegularExpression titleRegex(titlePattern);
	QRegularExpression classRegex(classPattern);

	for(WindowObject* wi : m_windowMap.values())
	{
		if(titleRegex.match(wi->getWinTitle()).hasMatch() && classRegex.match(wi->getWinClass()).hasMatch())
		{
			return wi;
		}
	}

	return nullptr;
}

#include <QDebug>
QObjectList WindowView::getWindowList()
{
	QMap<QString, WindowObject*> sortedList;
	for(WindowObject* wi : m_windowMap.values())
	{
		QString hwndString;
		QTextStream addressStream (&hwndString);
		addressStream << wi->getHwnd();

		sortedList.insert(wi->property("winTitle").toString().toLower() + "-" + hwndString, wi);
	}

	QObjectList objectList;
	objectList.append(m_placeholder);
	for(WindowObject* wi : sortedList.values())
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

bool WindowView::hasWindowInfo(HWND hwnd)
{
	for(WindowObject* wi : m_windowMap.values())
	{
		if(wi->getHwnd() == hwnd)
			return true;
	}

	return false;
}

WindowObject* WindowView::getWindowInfo(HWND hwnd)
{
	for(WindowObject* wi : m_windowMap.values())
	{
		if(wi->getHwnd() == hwnd)
			return wi;
	}

	return nullptr;
}

WindowObject* WindowView::objectToWindowInfo(QObject* obj)
{
	qCInfo(windowView) << qobject_cast<WindowObject*>(obj);
	return qobject_cast<WindowObject*>(obj);
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

void WindowView::windowCreated(WindowInfo wi)
{
	qCInfo(windowView) << "WindowView windowCreated" << wi;

	WindowObject* winInfo = new WindowObject(wi, this);

	m_windowMap.insert(wi.hwnd, winInfo);
	emit windowListChanged();
}

void WindowView::windowTitleChanged(HWND hwnd, QString newTitle)
{
	WindowObject* target = m_windowMap.value(hwnd);
	if(target == nullptr)
	{
		qFatal("onWindowTitleChanged: target nullptr");
	}

	target->setProperty("winTitle", newTitle);
	emit windowListChanged();
}

void WindowView::windowDestroyed(HWND hwnd)
{
	WindowObject* target = m_windowMap.value(hwnd);
	if(target == nullptr)
	{
		qFatal("onWindowRemoved: target nullptr");
	}

	target->windowClosed();
	m_windowMap.remove(hwnd);
	emit windowListChanged();
}
