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
	, m_placeholder(new WindowInfo(this))
{
	qRegisterMetaType<WindowView*>();
	setObjectName("Window View");

	qCInfo(windowView) << "Startup";
}

WindowInfo* WindowView::getWindowByRegex(const QString& titlePattern, const QString& classPattern)
{
	QRegularExpression titleRegex(titlePattern);
	QRegularExpression classRegex(classPattern);

	for(WindowInfo* wi : m_windowMap.values())
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
	QMap<QString, WindowInfo*> sortedList;
	for(WindowInfo* wi : m_windowMap.values())
	{
		QString hwndString;
		QTextStream addressStream (&hwndString);
		addressStream << wi->getHwnd();

		sortedList.insert(wi->property("winTitle").toString().toLower() + "-" + hwndString, wi);
	}

	QObjectList objectList;
	objectList.append(m_placeholder);
	for(WindowInfo* wi : sortedList.values())
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

WindowInfo* WindowView::objectToWindowInfo(QObject* obj)
{
	qCInfo(windowView) << qobject_cast<WindowInfo*>(obj);
	return qobject_cast<WindowInfo*>(obj);
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

void WindowView::windowAdded(HWND hwnd, QString winTitle, QString winClass, QString winProcess, qint32 winStyle)
{
	qCInfo(windowView) << "WindowView windowAdded" << hwnd << winTitle << winClass << winProcess << winStyle;

	WindowInfo* winInfo = new WindowInfo(this);
	winInfo->setProperty("hwnd", QVariant::fromValue<HWND>(hwnd));
	winInfo->setProperty("winTitle", winTitle);
	winInfo->setProperty("winClass", winClass);
	winInfo->setProperty("winProcess", winProcess);
	winInfo->setProperty("winStyle", winStyle);

	m_windowMap.insert(hwnd, winInfo);
	emit windowListChanged();
}

void WindowView::windowTitleChanged(HWND hwnd, QString newTitle)
{
	WindowInfo* target = m_windowMap.value(hwnd);
	if(target == nullptr)
	{
		qFatal("onWindowTitleChanged: target nullptr");
	}

	target->setProperty("winTitle", newTitle);
	emit windowListChanged();
}

void WindowView::windowRemoved(HWND hwnd)
{
	WindowInfo* target = m_windowMap.value(hwnd);
	if(target == nullptr)
	{
		qFatal("onWindowRemoved: target nullptr");
	}

	target->windowClosed();
	m_windowMap.remove(hwnd);
	emit windowListChanged();
}
