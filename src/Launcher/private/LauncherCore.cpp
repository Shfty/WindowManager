#include "LauncherCore.h"

#include <QApplication>
#include <QMetaMethod>
#include <QQuickWindow>
#include <QTimer>

#include <QDebug>
Q_LOGGING_CATEGORY(launcherCore, "launcher.core")

#include <Shared.h>
#include <SettingsContainer.h>
#include <WindowView.h>
#include <WindowInfo.h>

#include "WindowModel.h"
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
	, m_windowModel(nullptr)
	, m_windowView(nullptr)
	, m_windowController(nullptr)
	, m_overlayController(nullptr)
	, m_ipcServer(nullptr)
{
	setObjectName("Construction");

	qCInfo(launcherCore) << "Construction";

	/*
	QLoggingCategory::setFilterRules("*=false\n"
									 "launcher.windowEventModel=true\n"
									 "shared.windowView=true");
	*/

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

	// Window Event Model
	m_windowEventModelThread.setObjectName("Window Event Model Thread");
	m_windowModel = new WindowModel(nullptr);
	m_windowModel->moveToThread(&m_windowEventModelThread);
	connect(&m_windowEventModelThread, &QThread::started, m_windowModel, &WindowModel::startup);
	connect(&m_windowEventModelThread, &QThread::finished, m_windowModel, &QObject::deleteLater);

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
	m_windowEventModelThread.start();
	m_windowControllerThread.start();

	m_trayIcon->startup();
	m_winShellController->startup();
	m_overlayController->startup();
}

void LauncherCore::registerMetatypes()
{
	qCInfo(launcherCore) << "Registering Metatypes";

	Shared::registerMetatypes();

	qRegisterMetaType<IPCServer*>();
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
		// Window Model -> IPC Server Thread
		connect(m_windowModel, SIGNAL(startupComplete()), &m_ipcServerThread, SLOT(start()));

		// Window Model -> Window Controller
		connect(m_windowModel, &WindowModel::windowCreated, m_windowController, &WindowController::windowCreated);
		connect(m_windowModel, &WindowModel::windowDestroyed, m_windowController, &WindowController::windowDestroyed);
		connect(m_windowModel, &WindowModel::activeWindowChanged, m_windowController, &WindowController::updateWindowLayout);

		// Window Model -> Window View
		connect(m_windowModel, &WindowModel::windowCreated, m_windowView, &WindowView::windowCreated);
		connect(m_windowModel, &WindowModel::windowRenamed, m_windowView, &WindowView::windowTitleChanged);
		connect(m_windowModel, &WindowModel::windowDestroyed, m_windowView, &WindowView::windowDestroyed);

		// Window Model -> IPC Server
		connect(m_windowModel, &WindowModel::windowCreated, m_ipcServer, &IPCServer::windowCreated);
		connect(m_windowModel, &WindowModel::windowRenamed, m_ipcServer, &IPCServer::windowTitleChanged);
		connect(m_windowModel, &WindowModel::windowDestroyed, m_ipcServer, &IPCServer::windowDestroyed);
	}

	// IPC Server
	{
		// App Core
		connect(m_ipcServer, &IPCServer::clientReady, this, &LauncherCore::clientReady);
		connect(m_ipcServer, &IPCServer::windowListRequested, this, &LauncherCore::windowListRequested);
		connect(m_ipcServer, &IPCServer::setPendingWindowInfoSocket, this, &LauncherCore::setPendingWindowInfoSocket);

		// Win Shell Controller
		connect(m_ipcServer, &IPCServer::toggleTrayRequested, m_winShellController, &WinShellController::toggleTray);

		// Window Controller
		connect(m_ipcServer, &IPCServer::moveWindow, m_windowController, &WindowController::moveWindow);
		connect(m_ipcServer, &IPCServer::commitWindowMove, m_windowController, &WindowController::updateWindowLayout);
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
		connect(m_overlayController, &OverlayController::reloadRequested, this, &LauncherCore::reloadRequested);
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
	m_windowController->registerOverlayWindow(WindowInfo(reinterpret_cast<HWND>(window->winId()), "Launcher Overlay"));
}

void LauncherCore::clientReady(AppClient client)
{
	m_windowController->registerUnderlayWindow(WindowInfo(client.hwnd, client.name));
}

void LauncherCore::windowListRequested(QString socketName)
{
	QObjectList windowList = m_windowView->getWindowList();
	QVariantList winListMessage = {"WindowList", windowList.length()};

	for(QObject* object : windowList)
	{
		WindowObject* wo = qobject_cast<WindowObject*>(object);
		winListMessage.append(QVariant::fromValue<WindowInfo>(wo->getWindowInfo()));
	}

	emit sendMessage(socketName, winListMessage);
}

void LauncherCore::setPendingWindowInfoSocket(QString socketName)
{
	m_pendingWindowInfoSocket = socketName;
}

void LauncherCore::windowSelected(QVariant windowInfoVar)
{
	WindowObject* wi = windowInfoVar.value<WindowObject*>();

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

void LauncherCore::reloadRequested()
{
	QVariantList message = {"ReloadQML"};
	QMetaObject::invokeMethod(m_ipcServer, "broadcastMessage", Q_ARG(QVariantList, message));
}

void LauncherCore::quitRequested()
{
	QVariantList message = {"Quit"};
	QMetaObject::invokeMethod(m_ipcServer, "broadcastMessage", Q_ARG(QVariantList, message));
}

void LauncherCore::cleanup()
{
	qCInfo(launcherCore) << "Launcher Quitting";

	m_windowEventModelThread.quit();
	m_windowEventModelThread.wait();

	m_subprocessControllerThread.quit();
	m_subprocessControllerThread.wait();

	m_ipcServerThread.quit();
	m_ipcServerThread.wait();

	m_windowControllerThread.quit();
	m_windowControllerThread.wait();
}
