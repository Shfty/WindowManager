#include "WindowModel.h"

#include <QGuiApplication>

#include <QDebug>
Q_LOGGING_CATEGORY(windowModel, "launcher.windowModel")

#include "Win.h"

WindowModel::WindowModel(QObject* parent)
	: QThread(parent)
	, m_abort(false)
{
	setObjectName("Window Model");

	qCInfo(windowModel, "Startup");
}

void WindowModel::cleanup()
{
	m_abort = true;
}

BOOL CALLBACK enumWindowsProc(__in HWND hwnd, __in LPARAM lParam)
{
	if(hwnd == GetShellWindow()) return TRUE;
	if(!IsWindowVisible(hwnd)) return TRUE;

	int titleLength = GetWindowTextLength(hwnd);
	if(titleLength == 0) return TRUE;

	// Retrieve the pointer passed into this callback, and re-'type' it.
	// The only way for a C API to pass arbitrary data is by means of a void*.
	QList<HWND>& windowList = *reinterpret_cast<QList<HWND>*>(lParam);
	if(windowList.indexOf(hwnd) != -1) return TRUE;

	windowList.append(hwnd);
	return TRUE;
}

void WindowModel::run()
{
	qCInfo(windowModel) << "Startup";

	while(!m_abort)
	{
		QList<HWND> windows;
		EnumWindows(enumWindowsProc, reinterpret_cast<LPARAM>(&windows));

		// Prune closed windows
		for(HWND hwnd : m_windowMap.keys())
		{
			if(!windows.contains(hwnd))
			{
				m_windowMap.remove(hwnd);
				emit windowRemoved(hwnd);
			}
		}

		// Add or update new windows
		for(HWND hwnd : windows)
		{
			int titleLength = GetWindowTextLength(hwnd);
			wchar_t* wt = new wchar_t[titleLength + 1];
			GetWindowText(hwnd, wt, titleLength + 1);
			QString winTitle = QString::fromStdWString(wt);
			delete[] wt;

			if(winTitle == QGuiApplication::applicationName()) continue;

			wchar_t* wc = new wchar_t[256];
			GetClassName(hwnd, wc, 256);
			QString winClass = QString::fromStdWString(wc);
			delete[] wc;

			if(winClass == "Qt5QWindowOwnDCIcon") continue;

			if(m_windowMap.contains(hwnd))
			{
				QString existingTitle = m_windowMap.value(hwnd);
				if(existingTitle != winTitle)
				{
					m_windowMap.insert(hwnd, winTitle);
					emit windowTitleChanged(hwnd, winTitle);
				}
			}
			else
			{

				DWORD dwProcId = 0;
				GetWindowThreadProcessId(hwnd, &dwProcId);

				HANDLE hProc = OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ , FALSE, dwProcId);
				wchar_t buffer[MAX_PATH];
				GetModuleFileNameEx((HMODULE)hProc, NULL, buffer, MAX_PATH);
				CloseHandle(hProc);

				QString winProcess = QString::fromStdWString(buffer);
				winProcess.replace("\\", "/");

				qint32 winStyle = GetWindowLong(hwnd, GWL_STYLE);

				m_windowMap.insert(hwnd, winTitle);
				qCInfo(windowModel) << "windowAdded" << hwnd << winTitle << winClass << winProcess << winStyle;
				emit windowAdded(hwnd, winTitle, winClass, winProcess, winStyle);
			}
		}

		emit windowScanFinished();

		msleep(33);
	}

	qCInfo(windowModel) << "Shutdown";

	quit();
}
