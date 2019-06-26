#include "LauncherCore.h"

#include <QApplication>
#include <QMetaMethod>

#include <QDebug>
Q_LOGGING_CATEGORY(launcherCore, "launcher.core")

#include <SettingsContainer.h>
#include <WindowView.h>
#include <WindowInfo.h>

#include "WindowModel.h"
#include "WindowController.h"
#include "WinShellController.h"
#include "SubprocessController.h"
#include "IPCServer.h"
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

	qCInfo(launcherCore) << "Startup";

	registerMetatypes();

	m_subprocessController = new SubprocessController(this);

	m_winShellController = new WinShellController(this);
	m_trayIcon = new TrayIcon(this);

	m_windowModel = new WindowModel(this);
	m_windowView = new WindowView(this);
	m_windowController = new WindowController(this);

	m_ipcServerThread.setObjectName("IPC Server Thread");
	m_ipcServer = new IPCServer("WindowManager");
	m_ipcServer->moveToThread(&m_ipcServerThread);
	connect(&m_ipcServerThread, &QThread::started, m_ipcServer, &IPCServer::listen);
	connect(&m_ipcServerThread, &QThread::finished, m_ipcServer, &QObject::deleteLater);

	m_settingsContainer = new SettingsContainer(this);
	registerSynchronizedObject(m_settingsContainer);

	m_overlayController = new OverlayController(this);

	winShellControllerChanged();
	windowViewChanged();
	settingsContainerChanged();
	ipcServerChanged();

	qCInfo(launcherCore) << "Construction Complete";

	makeConnections();

	emit startup();
}

void LauncherCore::registerMetatypes()
{
	qCInfo(launcherCore) << "Registering Metatypes";

	qRegisterMetaType<HWND>();
	qRegisterMetaTypeStreamOperators<HWND>();
	QMetaType::registerDebugStreamOperator<HWND>();
	qRegisterMetaType<IPCServer*>();
	qRegisterMetaType<WindowInfo*>();
}

void LauncherCore::makeConnections()
{
	qCInfo(launcherCore) << "Making Connections";

	// Window Model -> Window View
	connect(m_windowModel, &WindowModel::windowAdded, m_windowView, &WindowView::windowAdded);
	connect(m_windowModel, &WindowModel::windowTitleChanged, m_windowView, &WindowView::windowTitleChanged);
	connect(m_windowModel, &WindowModel::windowRemoved, m_windowView, &WindowView::windowRemoved);

	// Window Model -> IPC Server
	connect(m_windowModel, &WindowModel::windowAdded, m_ipcServer, &IPCServer::windowAdded);
	connect(m_windowModel, &WindowModel::windowTitleChanged, m_ipcServer, &IPCServer::windowTitleChanged);
	connect(m_windowModel, &WindowModel::windowRemoved, m_ipcServer, &IPCServer::windowRemoved);

	// IPC Server -> QML Controller
	connect(m_ipcServer, &IPCServer::moveOverlayRequested, m_overlayController, &OverlayController::moveWindow);
	connect(m_ipcServer, &IPCServer::showOverlayRequested, m_overlayController, &OverlayController::showWindow);

	// IPC Server -> Win Shell Controller
	connect(m_ipcServer, &IPCServer::toggleTrayRequested, m_winShellController, &WinShellController::toggleTray);

	// IPC Server -> Window Controller
	connect(m_ipcServer, &IPCServer::beginMoveWindows, m_windowController, &WindowController::beginMoveWindows);
	connect(m_ipcServer, &IPCServer::moveWindow, m_windowController, &WindowController::moveWindow);
	connect(m_ipcServer, &IPCServer::endMoveWindows, m_windowController, &WindowController::endMoveWindows);
	connect(m_ipcServer, &IPCServer::setWindowStyle, m_windowController, &WindowController::setWindowStyle);

	// Overlay Controller -> App Core -> IPC Server
	connect(m_overlayController, &OverlayController::quitRequested, [=]() {
		QVariantList message = {"Quit"};
		QMetaObject::invokeMethod(m_ipcServer, "broadcastMessage", Q_ARG(QVariantList, message));
	});

	// Overlay Controller -> App Core -> IPC Server
	connect(m_overlayController, &OverlayController::windowSelected, [=](QVariant wiVar) {
		WindowInfo* wi = wiVar.value<WindowInfo*>();

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

		QMetaObject::invokeMethod(m_ipcServer, "sendMessage", Q_ARG(QString, m_pendingWindowInfoRecipient), Q_ARG(QVariantList, message));

		m_pendingWindowInfoRecipient = QString();
	});

	// App Core -> IPC Server
	connect(this, SIGNAL(sendMessage(QString, QVariantList)), m_ipcServer, SLOT(sendMessage(QString, QVariantList)));

	// IPC Server -> App Core
	connect(m_ipcServer, &IPCServer::windowListRequested, [=](QString socket) {
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
		emit sendMessage(socket, winListMessage);
	});

	connect(m_ipcServer, &IPCServer::setPendingWindowInfoSocket, [=](QString socketName) {
		m_pendingWindowInfoRecipient = socketName;
	});

	// Startup
	connect(this, SIGNAL(startup()), &m_ipcServerThread, SLOT(start()));
	connect(this, &LauncherCore::startup, m_windowModel, &WindowModel::startProcess);

	// Cleanup on quit
	connect(QApplication::instance(), &QApplication::aboutToQuit, m_subprocessController, &SubprocessController::cleanup);
	connect(QApplication::instance(), &QApplication::aboutToQuit, m_winShellController, &WinShellController::cleanup);
	connect(QApplication::instance(), &QApplication::aboutToQuit, m_overlayController, &OverlayController::cleanup);
	connect(QApplication::instance(), &QApplication::aboutToQuit, [=](){
		qCInfo(launcherCore) << "Launcher Quitting";

		m_windowModel->stopProcess();
		m_windowModel->wait();

		m_ipcServerThread.quit();
		m_ipcServerThread.wait();
	});
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
