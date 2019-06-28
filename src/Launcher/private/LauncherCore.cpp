#include "LauncherCore.h"

#include <QApplication>
#include <QMetaMethod>
#include <QQuickWindow>

#include <QDebug>
Q_LOGGING_CATEGORY(launcherCore, "launcher.core")

#include <SettingsContainer.h>
#include <WindowView.h>
#include <WindowInfo.h>

#include "WindowModelThread.h"
#include "WindowController.h"
#include "WinShellController.h"
#include "SubprocessController.h"
#include "OverlayController.h"
#include "TrayIcon.h"

LauncherCore::LauncherCore(QObject* parent)
	: QObject(parent)
	, m_trayIcon(nullptr)
	, m_subprocessController(nullptr)
	, m_winShellController(nullptr)
	, m_settingsContainer(nullptr)
	, m_windowModelThread(nullptr)
	, m_windowView(nullptr)
	, m_windowController(nullptr)
	, m_overlayController(nullptr)
	, m_ipcServer(nullptr)
{
	setObjectName("Construction");

	qCInfo(launcherCore) << "Construction";

	registerMetatypes();

	// Subprocess Controller
	m_subprocessControllerThread.setObjectName("Subprocess Controller Thread");
	m_subprocessController = new SubprocessController(nullptr);
	m_subprocessController->moveToThread(&m_subprocessControllerThread);
	connect(&m_subprocessControllerThread, &QThread::started, m_subprocessController, &SubprocessController::startup);
	connect(&m_subprocessControllerThread, &QThread::finished, m_subprocessController, &QObject::deleteLater);

	// IPC Server
	m_ipcServerThread.setObjectName("IPC Server Thread");
	m_ipcServer = new IPCServer("WindowManager");
	m_ipcServer->moveToThread(&m_ipcServerThread);
	connect(&m_ipcServerThread, &QThread::started, m_ipcServer, &IPCServer::startup);
	connect(&m_ipcServerThread, &QThread::finished, m_ipcServer, &QObject::deleteLater);

	// Window Model Thread
	m_windowModelThread = new WindowModelThread(this);

	// Window Controller
	m_windowControllerThread.setObjectName("Window Controller Thread");
	m_windowController = new WindowController(nullptr);
	m_windowController->moveToThread(&m_windowControllerThread);
	connect(&m_windowControllerThread, &QThread::started, m_windowController, &WindowController::startup);
	connect(&m_windowControllerThread, &QThread::finished, m_windowController, &QObject::deleteLater);

	// Window View
	m_windowView = new WindowView(this);

	// Win Shell Controller & Tray Icon
	m_winShellController = new WinShellController(this);
	m_trayIcon = new TrayIcon(this);

	// Settings Container
	m_settingsContainer = new SettingsContainer(this);

	// Overlay QML
	m_overlayController = new OverlayController(this);

	// Signal Notifies
	winShellControllerChanged();
	windowViewChanged();
	settingsContainerChanged();
	ipcServerChanged();

	makeConnections();

	qCInfo(launcherCore) << "Construction Complete";

	m_subprocessControllerThread.start();
	m_windowModelThread->start();
	m_windowControllerThread.start();

	m_trayIcon->startup();
	m_winShellController->startup();
	m_overlayController->startup();
}

void LauncherCore::registerMetatypes()
{
	qCInfo(launcherCore) << "Registering Metatypes";

	qRegisterMetaType<HWND>();
	qRegisterMetaTypeStreamOperators<HWND>();
	QMetaType::registerDebugStreamOperator<HWND>();
	qRegisterMetaType<IPCServer*>();
	qRegisterMetaType<WindowInfo*>();
	qRegisterMetaType<AppClient>();
}

void LauncherCore::makeConnections()
{
	qCInfo(launcherCore) << "Making Connections";

	// Synchronized Objects
	registerSynchronizedObject(m_settingsContainer);

	// App Core
	{
		// IPC Server
		connect(this, SIGNAL(sendMessage(QString, QVariantList)), m_ipcServer, SLOT(sendMessage(QString, QVariantList)));
	}

	// Window Model
	{
		//App Core
		connect(m_windowModelThread, &WindowModelThread::windowScanFinished, this, &LauncherCore::windowScanFinished);

		// Window Model -> Window View
		connect(m_windowModelThread, &WindowModelThread::windowAdded, m_windowView, &WindowView::windowAdded);
		connect(m_windowModelThread, &WindowModelThread::windowTitleChanged, m_windowView, &WindowView::windowTitleChanged);
		connect(m_windowModelThread, &WindowModelThread::windowRemoved, m_windowView, &WindowView::windowRemoved);

		// Window Model -> Window Controller
		connect(m_windowModelThread, &WindowModelThread::windowAdded, m_windowController, &WindowController::windowAdded);
		connect(m_windowModelThread, &WindowModelThread::windowRemoved, m_windowController, &WindowController::windowRemoved);

		// Window Model -> IPC Server
		connect(m_windowModelThread, &WindowModelThread::windowAdded, m_ipcServer, &IPCServer::windowAdded);
		connect(m_windowModelThread, &WindowModelThread::windowTitleChanged, m_ipcServer, &IPCServer::windowTitleChanged);
		connect(m_windowModelThread, &WindowModelThread::windowRemoved, m_ipcServer, &IPCServer::windowRemoved);
	}

	// IPC Server
	{
		// App Core
		connect(m_ipcServer, &IPCServer::socketReady, this, &LauncherCore::socketReady);
		connect(m_ipcServer, &IPCServer::windowListRequested, this, &LauncherCore::windowListRequested);
		connect(m_ipcServer, &IPCServer::setPendingWindowInfoSocket, this, &LauncherCore::setPendingWindowInfoSocket);

		// Win Shell Controller
		connect(m_ipcServer, &IPCServer::toggleTrayRequested, m_winShellController, &WinShellController::toggleTray);

		// Window Controller
		connect(m_ipcServer, &IPCServer::moveWindow, m_windowController, &WindowController::moveWindow);
		connect(m_ipcServer, &IPCServer::commitWindowMove, m_windowController, &WindowController::commitWindowMove);
		connect(m_ipcServer, &IPCServer::setWindowStyle, m_windowController, &WindowController::setWindowStyle);
		connect(m_ipcServer, &IPCServer::closeWindow, m_windowController, &WindowController::closeWindow);

		// Win Shell Controller
		connect(m_ipcServer, &IPCServer::toggleTrayRequested, m_winShellController, &WinShellController::toggleTray);

		// QML Controller
		connect(m_ipcServer, &IPCServer::moveOverlayRequested, m_overlayController, &OverlayController::moveWindow);
		connect(m_ipcServer, &IPCServer::showOverlayRequested, m_overlayController, &OverlayController::showWindow);
	}

	// Overlay Controller
	{
		// App Core
		connect(m_overlayController, &OverlayController::windowReady, this, &LauncherCore::windowReady);
		connect(m_overlayController, &OverlayController::windowSelected, this, &LauncherCore::windowSelected);
		connect(m_overlayController, &OverlayController::quitRequested, this, &LauncherCore::quitRequested);
	}

	// Cleanup on quit
	connect(QApplication::instance(), &QApplication::aboutToQuit, this, &LauncherCore::cleanup);
	connect(QApplication::instance(), &QApplication::aboutToQuit, m_winShellController, &WinShellController::cleanup);
	connect(QApplication::instance(), &QApplication::aboutToQuit, m_overlayController, &OverlayController::cleanup);
}

void LauncherCore::registerSynchronizedObject(QObject* object)
{
	qCInfo(launcherCore) << "Registering synchronized object" << object;
	const QMetaObject* meta = object->metaObject();
	for(int i = meta->propertyOffset(); i < meta->propertyCount(); ++i)
	{
		QMetaProperty prop = meta->property(i);
		if(prop.hasNotifySignal())
		{
			QMetaMethod signal = prop.notifySignal();

			const QMetaObject* ipcServerMeta = m_ipcServer->metaObject();
			int slotIndex = ipcServerMeta->indexOfSlot("syncObjectPropertyChanged()");
			QMetaMethod slot = ipcServerMeta->method(slotIndex);
			connect(object, signal, m_ipcServer, slot);
		}
	}
}

void LauncherCore::windowReady(QQuickWindow* window)
{
	m_windowController->registerOverlayWindow(reinterpret_cast<HWND>(window->winId()));
}

void LauncherCore::socketReady(AppClient client)
{
	m_windowController->registerUnderlayWindow(client.hwnd);
}

void LauncherCore::windowScanFinished()
{
	disconnect(m_windowModelThread, &WindowModelThread::windowScanFinished, this, &LauncherCore::windowScanFinished);
	m_ipcServerThread.start();
}

void LauncherCore::windowListRequested(QString socketName)
{
	QObjectList windowList = m_windowView->getWindowList();
	QVariantList winListMessage = {"WindowList", windowList.length()};

	for(QObject* object : windowList)
	{
		WindowInfo* windowInfo = qobject_cast<WindowInfo*>(object);
		winListMessage.append(QVariant::fromValue<HWND>(windowInfo->getHwnd()));
		winListMessage.append(windowInfo->getWinTitle());
		winListMessage.append(windowInfo->getWinClass());
		winListMessage.append(windowInfo->getWinProcess());
		winListMessage.append(windowInfo->getWinStyle());
	}

	emit sendMessage(socketName, winListMessage);
}

void LauncherCore::setPendingWindowInfoSocket(QString socketName)
{
	m_pendingWindowInfoSocket = socketName;
}

void LauncherCore::windowSelected(QVariant windowInfoVar)
{
	WindowInfo* wi = windowInfoVar.value<WindowInfo*>();

	qCInfo(launcherCore) << "Window Selected " << wi;

	QVariant hwndVar;
	if(wi != nullptr)
	{
		HWND hwnd = wi->getHwnd();
		hwndVar = QVariant::fromValue<HWND>(hwnd);
	}
	else
	{
		hwndVar = QVariant::fromValue<HWND>(nullptr);
	}

	QVariantList message = {
		"WindowSelected",
		hwndVar
	};

	QMetaObject::invokeMethod(m_ipcServer, "sendMessage", Q_ARG(QString, m_pendingWindowInfoSocket), Q_ARG(QVariantList, message));

	m_pendingWindowInfoSocket = QString();
}

void LauncherCore::quitRequested()
{
	QVariantList message = {"Quit"};
	QMetaObject::invokeMethod(m_ipcServer, "broadcastMessage", Q_ARG(QVariantList, message));
}

void LauncherCore::cleanup()
{
	qCInfo(launcherCore) << "Launcher Quitting";

	m_windowModelThread->cleanup();
	m_windowModelThread->wait();

	m_subprocessControllerThread.quit();
	m_subprocessControllerThread.wait();

	m_ipcServerThread.quit();
	m_ipcServerThread.wait();

	m_windowControllerThread.quit();
	m_windowControllerThread.wait();
}
