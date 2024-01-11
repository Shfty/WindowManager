#include "WindowEnumerator.h"

#include <QTimer>
#include <QTextStream>

BOOL CALLBACK enumWindows(__in HWND hwnd, __in LPARAM lParam)
{
	// Windows with no title
	int windowTitleLength = GetWindowTextLength(hwnd);
	if(windowTitleLength == 0) return true;

	// Invisible Windows
	if(!IsWindowVisible(hwnd)) return true;

	// Non-root Windows
	if(hwnd != GetAncestor(hwnd, GA_ROOT)) return true;

	WindowEnumerator& enumerator = *reinterpret_cast<WindowEnumerator*>(lParam);
	enumerator.windowAdded(hwnd);

	return true;
}

WindowEnumerator::WindowEnumerator(QObject *parent)
	: QObject(parent)
	, m_timer(nullptr)
{

}

void WindowEnumerator::startup()
{
	m_timer = new QTimer(this);
	connect(m_timer, &QTimer::timeout, this, &WindowEnumerator::enumerateWindows);
	m_timer->start(5);
}

void WindowEnumerator::cleanup()
{
	m_timer->stop();
}

void WindowEnumerator::enumerateWindows()
{
	m_windowList.clear();
	EnumWindows(enumWindows, reinterpret_cast<LPARAM>(this));
	emit enumComplete(QVariant::fromValue<QStringList>(m_windowList));
}

void WindowEnumerator::windowAdded(HWND hwnd)
{
	wchar_t winTitle[255];
	GetWindowText(hwnd, winTitle, 255);
	QString winTitleString = QString::fromStdWString(winTitle);

	RECT rect;
	GetWindowRect(hwnd, &rect);
	long x = rect.left;
	long y = rect.top;

	qint32 style = GetWindowLong(hwnd, GWL_STYLE);
	bool visible = style & WS_VISIBLE;

	qint32 exStyle = GetWindowLong(hwnd, GWL_EXSTYLE);
	bool topmost = exStyle & WS_EX_TOPMOST;

	QString windowString;
	QTextStream ts(&windowString);
	ts << m_windowList.length() << ": " << hwnd << "\t" << (topmost ? "TOPMOST " : "") << "\t" << (visible ? "VISIBLE" : "") << "\t" << x << ", " << y << "\t" << winTitleString;
	ts.flush();
	m_windowList.append(windowString);
}
