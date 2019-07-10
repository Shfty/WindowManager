#include "ClientCore.h"

#include <QScreen>
#include <QGuiApplication>
#include <QQuickWindow>
#include <QQuickItem>
#include <QLocalSocket>
#include <QMetaObject>

#include <QDebug>
Q_LOGGING_CATEGORY(clientCore, "app.core")

#include <Shared.h>
#include <WindowView.h>
#include <SettingsContainer.h>
#include <Win.h>

#include "QMLController.h"
#include "TreeModel.h"
#include "TreeItem.h"
#include "DWMThumbnail.h"
#include "IPCClient.h"

ClientCore::ClientCore(QObject* parent)
	: QObject(parent)
	, m_qmlController(nullptr)
	, m_windowView(nullptr)
	, m_settingsContainer(nullptr)
	, m_treeModel(nullptr)
	, m_exitExpected(false)
	, m_ipcClient(nullptr)
	, m_pendingWindowRecipient(nullptr)
{
	setObjectName("Client Core");

	qCInfo(clientCore) << "Construction";

	registerMetatypes();

	m_qmlController = new QMLController(this);
	m_qmlController->startup();

	m_windowView = new WindowView(this);

	m_ipcClientThread.setObjectName("IPC Client Thread");
	m_ipcClient = new IPCClient(QString(QCoreApplication::arguments().at(1)));
	m_ipcClient->moveToThread(&m_ipcClientThread);
	connect(&m_ipcClientThread, &QThread::started, m_ipcClient, &IPCClient::startup);
	connect(&m_ipcClientThread, &QThread::finished, m_ipcClient, &QObject::deleteLater);

	m_settingsContainer = new SettingsContainer(this);
	m_treeModel = new TreeModel(this);

	windowViewChanged();
	settingsContainerChanged();
	treeModelChanged();

	makeConnections();

	qCInfo(clientCore) << "Construction Complete";

	m_ipcClientThread.start();
}

void ClientCore::registerMetatypes()
{
	// Register Metatypes
	qCInfo(clientCore) << "Registering Metatypes";

	Shared::registerMetatypes();

	qRegisterMetaType<QScreen*>();
	qRegisterMetaType<TreeModel*>();
	qRegisterMetaType<TreeItem*>();
	qRegisterMetaType<IPCClient*>();

	// Register QML types
	qCInfo(clientCore) << "Registering QML Types";
	qmlRegisterType<DWMThumbnail>("DWMThumbnail", 1, 0, "DWMThumbnail");
}

void ClientCore::makeConnections()
{
	// QML Controller
	{
		// IPC Client
		connect(m_qmlController, &QMLController::sendMessage, m_ipcClient, &IPCClient::sendMessage);
	}

	// IPC Client
	{
		// App Core
		connect(m_ipcClient, &IPCClient::ipcReady, this, &ClientCore::ipcReady);
		connect(m_ipcClient, &IPCClient::syncObjectPropertyChanged, this, &ClientCore::syncObjectPropertyChanged);
		connect(m_ipcClient, &IPCClient::windowSelected, this, &ClientCore::windowSelected);
		connect(m_ipcClient, &IPCClient::windowSelectionCanceled, this, &ClientCore::windowSelectionCanceled);
		connect(m_ipcClient, &IPCClient::receivedQuitMessage, this, &ClientCore::exitRequested);

		// QML Controller
		connect(m_ipcClient, &IPCClient::ipcReady, m_qmlController, &QMLController::ipcReady);
		connect(m_ipcClient, &IPCClient::reloadQml, m_qmlController, &QMLController::reloadQml);
		connect(m_ipcClient, &IPCClient::disconnected, m_qmlController, &QMLController::closeWindow);

		// Window View
		connect(m_ipcClient, &IPCClient::windowCreated, m_windowView, &WindowView::windowCreated);
		connect(m_ipcClient, &IPCClient::windowTitleChanged, m_windowView, &WindowView::windowTitleChanged);
		connect(m_ipcClient, &IPCClient::windowDestroyed, m_windowView, &WindowView::windowDestroyed);

		// Tree Model
		connect(m_ipcClient, &IPCClient::receivedWindowList, m_treeModel, &TreeModel::startup);
	}

	// Tree Model
	{
		// IPC Client
		connect(m_treeModel, &TreeModel::sendMessage, m_ipcClient, &IPCClient::sendMessage);
	}

	// Cleanup on exit
	QGuiApplication* app = static_cast<QGuiApplication*>(QGuiApplication::instance());
	connect(app, &QGuiApplication::lastWindowClosed, this, &ClientCore::lastWindowClosed);
	connect(app, &QGuiApplication::aboutToQuit, this, &ClientCore::cleanup);
}

void ClientCore::windowReady(HWND hwnd)
{
	sendMessage({
		"WindowChanged",
		QVariant::fromValue<HWND>(hwnd)
	});
}

void ClientCore::windowDestroyed()
{
	sendMessage({
		"WindowChanged",
		QVariant::fromValue<HWND>(nullptr)
	});
}

void ClientCore::ipcReady()
{
	sendMessage({
		"WindowList"
	});
}

void ClientCore::syncObjectPropertyChanged(QString object, QString property, QVariant value)
{
	QObject* syncObject = findChild<QObject*>(object);
	Q_ASSERT(syncObject != nullptr);
	syncObject->setProperty(property.toStdString().c_str(), value);
}

void ClientCore::setPendingWindowRecipient(TreeItem* treeItem)
{
	m_pendingWindowRecipient = treeItem;
}

void ClientCore::sendMessage(QVariantList message)
{
	QMetaObject::invokeMethod(m_ipcClient, "sendMessage", Q_ARG(QVariantList, message));
}

void ClientCore::windowSelected(HWND hwnd)
{
	WindowObject* wi = m_windowView->getWindowInfo(hwnd);

	m_pendingWindowRecipient->setWindowInfo(wi);
	m_pendingWindowRecipient = nullptr;
}

void ClientCore::windowSelectionCanceled()
{
	m_pendingWindowRecipient = nullptr;
}

void ClientCore::exitRequested()
{
	m_exitExpected = true;

	m_treeModel->save();

	m_qmlController->closeWindow();
	/*
	connect(m_treeModel->getRootItem(), &TreeItem::animationFinished, [=](){
		m_qmlController->closeWindow();
	});
	*/

}

void ClientCore::lastWindowClosed()
{
	/*
	qCInfo(clientCore) << "All windows closed, shutting down";

	// Save if this is an unexpected force-quit
	if(m_exitExpected == false)
	{
		qCInfo(clientCore) << "Unexpected exit";
		m_treeModel->save();
		m_treeModel->getRootItem()->cleanup();
	}
	else {
		qCInfo(clientCore) << "Expected exit";
	}

	qCInfo(clientCore) << "Quitting";
	QGuiApplication::quit();
	*/
}

void ClientCore::cleanup()
{
	m_ipcClientThread.quit();
	m_ipcClientThread.wait();
}
