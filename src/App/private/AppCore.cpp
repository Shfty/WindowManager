#include "AppCore.h"

#include <QFileInfo>
#include <QScreen>
#include <QGuiApplication>
#include <QQuickWindow>
#include <QQuickItem>
#include <QQmlContext>
#include <QJsonDocument>
#include <QLocalSocket>
#include <QQmlApplicationEngine>
#include <QQuickWindow>

#include "WindowController.h"
#include "WindowView.h"
#include "WinShellController.h"
#include "TreeItem.h"
#include "QmlController.h"
#include "SettingsContainer.h"
#include "DWMThumbnail.h"
#include "TreeIconImageProvider.h"
#include "Win.h"

AppCore::AppCore(QObject* parent)
	: QObject(parent)
	, m_windowController(nullptr)
	, m_windowView(nullptr)
	, m_winShellController(nullptr)
	, m_settingsContainer(nullptr)
	, m_qmlController(nullptr)
	, m_rootItem(nullptr)
	, m_configOverlay(nullptr)
	, m_windowListOverlay(nullptr)
	, m_powerMenuOverlay(nullptr)
	, m_itemSettingsOverlay(nullptr)
	, m_exitExpected(false)
	, m_socketName(QString(QCoreApplication::arguments().at(1)))
	, m_monitorIndex(QCoreApplication::arguments().at(2).toInt())
	, m_autosavePath(QString(QCoreApplication::arguments().at(3)))
	, m_qmlEngine(nullptr)
	, m_qmlWindow(nullptr)
	, m_localSocket(nullptr)
{
	setObjectName("App Core");

	qInfo() << "Startup";

	// Elevate Privileges
	qInfo() << "Elevating Privileges";
	elevatePrivileges();

	// Register Metatypes
	qInfo() << "Registering Metatypes";
	qRegisterMetaType<WindowInfo*>();
	qRegisterMetaType<TreeItem*>();

	// Register QML types
	qInfo() << "Registering QML Types";
	qmlRegisterType<DWMThumbnail>("DWMThumbnail", 1, 0, "DWMThumbnail");
	qmlRegisterInterface<HWND>("HWND");

	// Load QML application
	qInfo() << "Loading main.qml";

	m_qmlEngine = new QQmlApplicationEngine("qrc:/qml/main.qml", this);
	m_qmlEngine->rootContext()->setContextProperty("appCore", this);
	m_qmlEngine->addImageProvider(QLatin1String("treeIcon"), new TreeIconImageProvider);

	m_qmlWindow = qobject_cast<QQuickWindow*>(m_qmlEngine->rootObjects()[0]);
	m_qmlWindow->setScreen(QGuiApplication::screens()[m_monitorIndex]);
	m_qmlWindow->setProperty("_q_showWithoutActivating", QVariant(true));
	m_qmlWindow->setVisibility(QWindow::Maximized);

	// Connect to launcher via local socket
	qInfo() << "Creating m_localSocket";
	m_localSocket = new QLocalSocket(this);
	connect(m_localSocket, &QLocalSocket::connected, [=](){
		qInfo() << "m_localSocket connected";
		connect(m_localSocket, &QLocalSocket::readyRead, [=](){
			QDataStream stream(m_localSocket);
			stream.setVersion(QDataStream::Qt_5_12);

			while(true)
			{
				QByteArray message;

				stream.startTransaction();
				stream >> message;
				QString mStr = QString::fromUtf8(message);

				if(stream.commitTransaction())
				{
					if(mStr == "Identify")
					{
						qInfo() << "ID request from server";
						stream << "Identify" << m_socketName.toUtf8();
					}
					else if(mStr == "Chatter")
					{
						qInfo() << "Chatter request from server";
						stream << "Foo" << "Bar" << "Baz" << "DecafIsBad";
					}
					else if(mStr == "Quit")
					{
						qInfo() << "Quit request from server";
						exitRequested();
					}
					else
					{
						qInfo() << "Unknown message from server:" << mStr;
					}
				}
				else
				{
					break;
				}
			}
		});
	});

	connect(m_localSocket, &QLocalSocket::disconnected, [=](){
		qInfo() << "m_localSocket disconnected";
		m_qmlController->cleanup();
	});

	m_localSocket->connectToServer("WindowManager");

	// Normal Startup
	qInfo() << "Initializing WM objects";
	// Window Controller
	m_windowController = new WindowController();
	//m_windowController->moveToThread(&m_windowControllerThread);
	//connect(&m_windowControllerThread, SIGNAL(finished()), m_windowController, SLOT(deleteLater()));
	//m_windowControllerThread.start();
	windowControllerChanged();

	// Window View
	m_windowView = new WindowView(this);
	windowViewChanged();

	// Settings Container
	m_settingsContainer = new SettingsContainer(this);
	settingsContainerChanged();

	// Windows Shell Controller
	m_winShellController = new WinShellController(this);
	winShellControllerChanged();

	/*
	qInfo() << "Creating Overlay Windows";
	// Config Window
	m_configOverlay = m_qmlController->createWindow(QUrl("qrc:/qml/overlays/ConfigOverlay.qml"), QRect(m_windowView->getOffscreenArea(), QSize(1280, 720)));
	m_configOverlay->setFlags(m_configOverlay->flags() | static_cast<Qt::WindowFlags>(
		Qt::FramelessWindowHint |
		Qt::WindowStaysOnTopHint
	));
	m_configOverlay->show();
	configOverlayChanged();

	// Window List Overlay
	m_windowListOverlay = m_qmlController->createWindow(QUrl("qrc:/qml/overlays/WindowListOverlay.qml"), QRect(m_windowView->getOffscreenArea(), QSize(640, 480)));
	m_windowListOverlay->setFlags(m_windowListOverlay->flags() | static_cast<Qt::WindowFlags>(
		Qt::FramelessWindowHint |
		Qt::WindowStaysOnTopHint
	));
	m_windowListOverlay->show();
	windowListOverlayChanged();

	// Power Menu Overlay
	m_powerMenuOverlay = m_qmlController->createWindow(QUrl("qrc:/qml/overlays/PowerMenuOverlay.qml"), QRect(m_windowView->getOffscreenArea(), QSize(120, 150)));
	m_powerMenuOverlay->setFlags(m_powerMenuOverlay->flags() | static_cast<Qt::WindowFlags>(
		Qt::FramelessWindowHint |
		Qt::WindowStaysOnTopHint
	));
	m_powerMenuOverlay->show();
	powerMenuOverlayChanged();

	// Item Settings Overlay
	m_itemSettingsOverlay = m_qmlController->createWindow(QUrl("qrc:/qml/overlays/ItemSettingsOverlay.qml"), QRect(m_windowView->getOffscreenArea(), QSize(640, 480)));
	m_itemSettingsOverlay->setFlags(m_itemSettingsOverlay->flags() | static_cast<Qt::WindowFlags>(
		Qt::FramelessWindowHint |
		Qt::WindowStaysOnTopHint
	));
	m_itemSettingsOverlay->show();
	itemSettingsOverlayChanged();
*/
	qInfo() << "Setup Connections";
	// Connections
	connect(m_windowView, SIGNAL(windowScanFinished()), this, SLOT(windowManagerReady()));
/*
	connect(m_qmlController, &QmlController::exitRequested, [=](){
		QDataStream stream(m_localSocket);
		stream.setVersion(QDataStream::Qt_5_12);
		stream << "Quit";
	});
*/
	QGuiApplication* app = static_cast<QGuiApplication*>(QGuiApplication::instance());
	connect(app, &QGuiApplication::lastWindowClosed, [=](){
		qInfo() << "All windows closed, shutting down";

		// Save if this is an unexpected force-quit
		if(m_exitExpected == false)
		{
			qInfo() << "Unexpected exit";
			save();
			m_rootItem->cleanup();
		}
		else {
			qInfo() << "Expected exit";
		}

		m_winShellController->cleanup();

		qInfo() << "Quitting";
		QGuiApplication::quit();
	});

	connect(app, &QGuiApplication::aboutToQuit, [=](){
		m_windowControllerThread.quit();
		m_windowControllerThread.wait();
	});
}

void AppCore::windowManagerReady()
{
	// Disconnect signal
	QObject::disconnect(m_windowView, SIGNAL(windowScanFinished()), this, SLOT(windowManagerReady()));

	// Tree model
	qInfo() << "Loading Tree Model from " << m_autosavePath;
	m_rootItem = loadModel(m_autosavePath);
	treeModelChanged();
	m_rootItem->startup();
	m_rootItem->updateWindowPosition();

	qInfo() << "Startup Complete";
}

void AppCore::exitRequested()
{
	m_exitExpected = true;

	save();

	m_settingsContainer->setProperty("headerSize", 0);
	m_rootItem->playShutdownAnimation();
	connect(m_rootItem, &TreeItem::animationFinished, [=](){
		m_qmlController->cleanup();
	});
}

void AppCore::save()
{
	qInfo() << "Saving Tree Model";
	saveModel(m_autosavePath, m_rootItem);
}

void AppCore::shutdown()
{
	qInfo() << "Triggering Shutdown";
	ExitWindowsEx(EWX_SHUTDOWN, 0);
}

void AppCore::reboot()
{
	qInfo() << "Triggering Reboot";
	ExitWindowsEx(EWX_REBOOT, 0);
}

void AppCore::sleep()
{
	qInfo() << "Triggering Sleep";
	SetSuspendState(true, false, false);
}

void AppCore::logout()
{
	qInfo() << "Logging out";
	ExitWindowsEx(EWX_LOGOFF, 0);
}

void AppCore::elevatePrivileges()
{
	qInfo() << "Elevating privileges";

	HANDLE hToken = nullptr;
	if(OpenProcessToken(GetCurrentProcess(), TOKEN_ADJUST_PRIVILEGES | TOKEN_QUERY, &hToken))
	{
		TOKEN_PRIVILEGES tkp;
		if(LookupPrivilegeValue(nullptr, SE_SHUTDOWN_NAME, &tkp.Privileges[0].Luid))
		{
			tkp.PrivilegeCount = 1;
			tkp.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED;

			if(AdjustTokenPrivileges(hToken, false, &tkp, 0, nullptr, nullptr))
			{
				qInfo() << "Privileges elevated";
				return;
			}
		}
	}

	qWarning() << "Privilege elevation failed";
}

QQuickItem* AppCore::getWindowListOverlay()
{
	return m_windowListOverlay->findChild<QQuickItem*>("WindowListOverlay");
}

QQuickItem* AppCore::getConfigOverlay()
{
	return m_configOverlay->findChild<QQuickItem*>("ConfigOverlay");
}

QQuickItem* AppCore::getPowerMenuOverlay()
{
	return m_powerMenuOverlay->findChild<QQuickItem*>("PowerMenuOverlay");
}

QQuickItem* AppCore::getItemSettingsOverlay()
{
	return m_itemSettingsOverlay->findChild<QQuickItem*>("ItemSettingsOverlay");
}

TreeItem* AppCore::loadModel(QString filename)
{
	bool fileExists = QFileInfo::exists(filename) && QFileInfo(filename).isFile();
	if(!fileExists)
	{
		saveDefaultModel(filename);
	}

	TreeItem* model = new TreeItem(this);

	QFile file(filename);
	file.open(QIODevice::ReadOnly);

	QByteArray loadData = file.readAll();
	QJsonDocument loadDocument = QJsonDocument::fromJson(loadData);
	model->loadFromJson(loadDocument.object());

	file.close();

	return model;
}

void AppCore::saveModel(QString filename, const TreeItem* model)
{
	QJsonDocument jsonDoc(model->toJsonObject());
	QFile jsonFile(filename);
	jsonFile.open(QIODevice::WriteOnly);
	jsonFile.write(jsonDoc.toJson());
	jsonFile.close();
}

void AppCore::saveDefaultModel(QString filename)
{
	TreeItem* model = new TreeItem(this);
	model->setObjectName("Monitor");
	model->setProperty("flow", "Horizontal");
	model->setProperty("layout", "Split");

	model->addChild("Desktop", "Horizontal", "Split");

	saveModel(filename, model);
}
