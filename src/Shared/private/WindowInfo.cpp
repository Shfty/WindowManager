#include "WindowInfo.h"

#include <QVariant>

#include <QDebug>
Q_LOGGING_CATEGORY(windowInfo, "shared.windowInfo")

WindowObject::WindowObject(QObject* parent)
	: QObject(parent)
{
	setObjectName("Window Info");

	qCInfo(windowInfo, "Startup");
}

WindowObject::WindowObject(WindowInfo wi, QObject* parent)
	: WindowObject(parent)
{
	m_windowInfo = wi;
}

void WindowObject::setHwnd(HWND newHwnd)
{
	m_windowInfo.hwnd = newHwnd;
	hwndChanged();
}

void WindowObject::setWinTitle(QString newTitle)
{
	m_windowInfo.winTitle = newTitle;
	winTitleChanged();
}

void WindowObject::setWinClass(QString newClass)
{
	m_windowInfo.winClass = newClass;
	winClassChanged();
}

void WindowObject::setWinProcess(QString newProcess) {
	m_windowInfo.winProcess = newProcess;
	winProcessChanged();
}

void WindowObject::setWinStyle(qint32 newStyle) {
	m_windowInfo.winStyle = newStyle;
	winStyleChanged();
}

QDataStream& operator <<(QDataStream& ds, const WindowInfo& wi)
{
	ds << wi.hwnd << wi.winTitle << wi.winClass << wi.winProcess << QVariant(wi.winStyle);
	return ds;
}

QDataStream& operator >>(QDataStream& ds, WindowInfo& wi)
{
	HWND hwnd;
	QString winTitle, winClass, winProcess;
	QVariant winStyleVar;

	ds >> hwnd >> winTitle >> winClass >> winProcess >> winStyleVar;

	qint32 winStyle = winStyleVar.value<qint32>();

	wi.hwnd = hwnd;
	wi.winTitle = winTitle;
	wi.winClass = winClass;
	wi.winProcess = winProcess;
	wi.winStyle = winStyle;

	return ds;
}

QDebug operator <<(QDebug dbg, const WindowInfo& wi)
{
	dbg.nospace() << wi.winTitle << " (" << wi.hwnd << ")";
	return dbg.maybeSpace();
}

QTextStream& operator <<(QTextStream& ts, const WindowInfo& wi)
{
	ts << wi.winTitle << " (" << wi.hwnd << ")";
	return ts;
}

bool operator <(const WindowInfo& lhs, const WindowInfo& rhs)
{
	return lhs.hwnd < rhs.hwnd;
}

bool operator ==(const WindowInfo& lhs, const WindowInfo& rhs)
{
	return lhs.hwnd == rhs.hwnd;
}
