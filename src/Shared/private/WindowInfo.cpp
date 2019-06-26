#include "WindowInfo.h"

#include <QVariant>

#include <QDebug>
Q_LOGGING_CATEGORY(windowInfo, "shared.windowInfo")

WindowInfo::WindowInfo(QObject* parent)
	: QObject(parent)
	, m_hwnd(HWND(-1))
	, m_winTitle("[Container]")
	, m_winClass("")
	, m_winProcess("")
	, m_winStyle(0)
{
	setObjectName("Window Info");

	qCInfo(windowInfo, "Startup");

}

void WindowInfo::setHwnd(HWND newHwnd)
{
	m_hwnd = newHwnd;
	hwndChanged();
}

void WindowInfo::setWinTitle(QString newTitle)
{
	m_winTitle = newTitle;
	winTitleChanged();
}

void WindowInfo::setWinClass(QString newClass)
{
	m_winClass = newClass;
	winClassChanged();
}

void WindowInfo::setWinProcess(QString newProcess) {
	m_winProcess = newProcess;
	winProcessChanged();
}

void WindowInfo::setWinStyle(qint32 newStyle) {
	m_winStyle = newStyle;
	winStyleChanged();
}
