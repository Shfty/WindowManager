#include "EnumWindowsThread.h"

#include "Win.h"
#include <QCoreApplication>
#include <QDebug>

EnumWindowsThread::EnumWindowsThread(QObject* parent)
	: QThread(parent)
{
}

void EnumWindowsThread::startProcess()
{
	m_abort = false;
	start();
}

void EnumWindowsThread::stopProcess()
{
	m_abort = true;
}

BOOL CALLBACK enumWindowsProc(__in HWND hwnd, __in LPARAM lParam)
{
	int titleLength = GetWindowTextLength(hwnd);

	if(!IsWindowVisible(hwnd) || titleLength == 0)
	{
		return TRUE;
	}

	wchar_t* windowTitle = new wchar_t[titleLength + 1];
	GetWindowText(hwnd, windowTitle, titleLength + 1);

	if(windowTitle == QCoreApplication::applicationName())
	{
		return TRUE;
	}

	wchar_t* windowClass = new wchar_t[256];
	GetClassName(hwnd, windowClass, 256);

	if(windowClass == QString("Progman"))
	{
		return TRUE;
	}

	// Retrieve the pointer passed into this callback, and re-'type' it.
	// The only way for a C API to pass arbitrary data is by means of a void*.
	QMap<HWND, WindowInfo>& windowMap = *reinterpret_cast<QMap<HWND, WindowInfo>*>(lParam);
	windowMap.insert(hwnd, WindowInfo(hwnd, QString::fromStdWString(windowTitle), QString::fromStdWString(windowClass)));

	delete[] windowTitle;

	return TRUE;
}

void EnumWindowsThread::run()
{
	while(!m_abort)
	{
		QMap<HWND, WindowInfo> windows;
		EnumWindows(enumWindowsProc, reinterpret_cast<LPARAM>(&windows));

		// Prune closed windows
		for(HWND k : m_windowMap.keys())
		{
			if(!windows.contains(k))
			{
				WindowInfo v = m_windowMap.value(k);
				m_windowMap.remove(k);
				emit windowRemoved(v);
			}
		}

		// Add or update new windows
		for(HWND k : windows.keys())
		{
			WindowInfo v = windows.value(k);
			if(m_windowMap.contains(k))
			{
				WindowInfo existing = m_windowMap.value(k);
				if(existing.winTitle != v.winTitle || existing.winClass != v.winClass)
				{
					m_windowMap.insert(k, v);
					emit windowChanged(v);
				}
			}
			else
			{
				m_windowMap.insert(k, v);
				emit windowAdded(v);
			}
		}

		emit windowScanFinished();

		msleep(10);
	}
}
