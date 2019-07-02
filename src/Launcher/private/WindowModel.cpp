#include "WindowModel.h"

#include <QDebug>
Q_LOGGING_CATEGORY(windowModel, "launcher.windowModel")

WindowModel* WindowModel::instance = nullptr;

WindowModel::WindowModel(QObject* parent)
	: QObject(parent)
{
	setObjectName("Window Event Model");

	qCInfo(windowModel) << "Construction";

	WindowModel::instance = this;
}

WindowModel::~WindowModel()
{
	UnhookWinEvent(m_createHook);
	UnhookWinEvent(m_renameHook);
	UnhookWinEvent(m_destroyHook);
}

bool filterObject(LONG idObject, LONG idChild)
{
	if(idObject != OBJID_WINDOW) return true;
	if(idChild != CHILDID_SELF) return true;
	return false;
}

bool filterWindow(HWND hwnd)
{
	if(hwnd == GetShellWindow()) return true;
	if(!IsWindowVisible(hwnd)) return true;

	int titleLength = GetWindowTextLength(hwnd);
	if(titleLength == 0) return true;

	return false;
}

BOOL CALLBACK enumWindows(__in HWND hwnd, __in LPARAM lParam)
{
	if(filterWindow(hwnd)) return true;

	WindowModel& windowModel = *reinterpret_cast<WindowModel*>(lParam);
	windowModel.handleWindowCreated(hwnd);

	return true;
}

void objectCreated(HWINEVENTHOOK hWinEventHook,
				   DWORD event,
				   HWND hwnd,
				   LONG idObject,
				   LONG idChild,
				   DWORD idEventThread,
				   DWORD dwmsEventTime)
{
	Q_UNUSED(hWinEventHook);
	Q_UNUSED(event);
	Q_UNUSED(idEventThread);
	Q_UNUSED(dwmsEventTime);

	if(filterObject(idObject, idChild)) return;

	WindowModel::instance->handleWindowCreated(hwnd);
}

void objectRenamed(HWINEVENTHOOK hWinEventHook,
				   DWORD event,
				   HWND hwnd,
				   LONG idObject,
				   LONG idChild,
				   DWORD idEventThread,
				   DWORD dwmsEventTime)
{
	Q_UNUSED(hWinEventHook);
	Q_UNUSED(event);
	Q_UNUSED(idEventThread);
	Q_UNUSED(dwmsEventTime);

	if(filterObject(idObject, idChild)) return;

	WindowModel::instance->handleWindowRenamed(hwnd);
}

void objectDestroyed(HWINEVENTHOOK hWinEventHook,
					 DWORD event,
					 HWND hwnd,
					 LONG idObject,
					 LONG idChild,
					 DWORD idEventThread,
					 DWORD dwmsEventTime)
{
	Q_UNUSED(hWinEventHook);
	Q_UNUSED(event);
	Q_UNUSED(idEventThread);
	Q_UNUSED(dwmsEventTime);

	if(filterObject(idObject, idChild)) return;

	WindowModel::instance->handleWindowDestroyed(hwnd);
}

void objectShown(HWINEVENTHOOK hWinEventHook,
					 DWORD event,
					 HWND hwnd,
					 LONG idObject,
					 LONG idChild,
					 DWORD idEventThread,
					 DWORD dwmsEventTime)
{
	Q_UNUSED(hWinEventHook);
	Q_UNUSED(event);
	Q_UNUSED(idEventThread);
	Q_UNUSED(dwmsEventTime);

	if(filterObject(idObject, idChild)) return;

	qCInfo(windowModel) << "Object Shown" << hwnd;

	WindowModel::instance->handleWindowCreated(hwnd);
}

void objectHidden(HWINEVENTHOOK hWinEventHook,
					 DWORD event,
					 HWND hwnd,
					 LONG idObject,
					 LONG idChild,
					 DWORD idEventThread,
					 DWORD dwmsEventTime)
{
	Q_UNUSED(hWinEventHook);
	Q_UNUSED(event);
	Q_UNUSED(idEventThread);
	Q_UNUSED(dwmsEventTime);

	if(filterObject(idObject, idChild)) return;

	qCInfo(windowModel) << "Object Hidden" << hwnd;

	WindowModel::instance->handleWindowDestroyed(hwnd);
}

void activeWindowChanged(HWINEVENTHOOK hWinEventHook,
					 DWORD event,
					 HWND hwnd,
					 LONG idObject,
					 LONG idChild,
					 DWORD idEventThread,
					 DWORD dwmsEventTime)
{
	Q_UNUSED(hWinEventHook);
	Q_UNUSED(event);
	Q_UNUSED(hwnd);
	Q_UNUSED(idObject);
	Q_UNUSED(idChild);
	Q_UNUSED(idEventThread);
	Q_UNUSED(dwmsEventTime);

	qCInfo(windowModel) << "Active Window Changed";
}

void WindowModel::startup()
{
	qCInfo(windowModel) << "Startup";

	EnumWindows(enumWindows, reinterpret_cast<LPARAM>(this));

	m_createHook = hookEvent(EVENT_OBJECT_CREATE, &objectCreated);
	m_renameHook = hookEvent(EVENT_OBJECT_NAMECHANGE, &objectRenamed);
	m_destroyHook = hookEvent(EVENT_OBJECT_DESTROY, &objectDestroyed);
	m_destroyHook = hookEvent(EVENT_OBJECT_SHOW, &objectShown);
	m_destroyHook = hookEvent(EVENT_OBJECT_HIDE, &objectHidden);
	m_destroyHook = hookEvent(EVENT_SYSTEM_FOREGROUND, &activeWindowChanged);

	emit startupComplete();
}

void WindowModel::handleWindowCreated(HWND hwnd)
{
	if(filterWindow(hwnd)) return;

	qCInfo(windowModel) << "windowCreated" << hwnd << getWinTitle(hwnd) << getWinClass(hwnd) << getWinProcess(hwnd) << getWinStyle(hwnd);
	emit windowCreated(hwnd, getWinTitle(hwnd), getWinClass(hwnd), getWinProcess(hwnd), getWinStyle(hwnd));
	m_windowList.insert(hwnd, getWinTitle(hwnd));
}

void WindowModel::handleWindowRenamed(HWND hwnd)
{
	if(filterWindow(hwnd)) return;

	if(!m_windowList.contains(hwnd))
	{
		qCInfo(windowModel) << "windowCreated" << hwnd << getWinTitle(hwnd) << getWinClass(hwnd) << getWinProcess(hwnd) << getWinStyle(hwnd);
		emit windowCreated(hwnd, getWinTitle(hwnd), getWinClass(hwnd), getWinProcess(hwnd), getWinStyle(hwnd));
	}
	else
	{
		qCInfo(windowModel) << "windowRenamed" << hwnd << getWinTitle(hwnd);
		emit windowRenamed(hwnd, getWinTitle(hwnd));
	}

	m_windowList.insert(hwnd, getWinTitle(hwnd));
}

void WindowModel::handleWindowDestroyed(HWND hwnd)
{
	if(!m_windowList.contains(hwnd)) return;

	qCInfo(windowModel) << "windowDestroyed" << hwnd;
	emit windowDestroyed(hwnd);

	m_windowList.remove(hwnd);
}

HWINEVENTHOOK WindowModel::hookEvent(DWORD event, WINEVENTPROC callback)
{
	return SetWinEventHook(
		event,
		event,
		nullptr,
		callback,
		0,
		0,
		WINEVENT_OUTOFCONTEXT | WINEVENT_SKIPOWNPROCESS
	);
}

QString WindowModel::getWinTitle(HWND hwnd)
{
	int titleLength = GetWindowTextLength(hwnd);
	wchar_t* wt = new wchar_t[qulonglong(titleLength + 1)];
	GetWindowText(hwnd, wt, titleLength + 1);
	QString winTitle = QString::fromStdWString(wt);
	delete[] wt;

	return winTitle;
}

QString WindowModel::getWinClass(HWND hwnd)
{
	wchar_t* wc = new wchar_t[256];
	GetClassName(hwnd, wc, 256);
	QString winClass = QString::fromStdWString(wc);
	delete[] wc;

	return winClass;
}

QString WindowModel::getWinProcess(HWND hwnd)
{
	DWORD dwProcId = 0;
	GetWindowThreadProcessId(hwnd, &dwProcId);

	HANDLE hProc = OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ , FALSE, dwProcId);
	wchar_t buffer[MAX_PATH];
	GetModuleFileNameEx(HMODULE(hProc), nullptr, buffer, MAX_PATH);
	CloseHandle(hProc);

	QString winProcess = QString::fromStdWString(buffer);
	winProcess.replace("\\", "/");

	return winProcess;
}

qint32 WindowModel::getWinStyle(HWND hwnd)
{
	return GetWindowLong(hwnd, GWL_STYLE);
}
