#include "AppCore.h"

#include <QFileInfo>
#include <QScreen>
#include <QGuiApplication>
#include <QQuickWindow>
#include <QQuickItem>
#include <QQmlContext>
#include <QJsonDocument>

#include "WindowController.h"
#include "WindowView.h"
#include "WinShellController.h"
#include "TreeItem.h"
#include "QmlController.h"
#include "SettingsContainer.h"
#include "Win.h"

#define WORKING_DIRECTORY "P:/Personal/C++/WindowManager"
#define AUTOSAVE_FILE "autosave.dat"
#define AUTOSAVE_JSON_FILE "autosave.json"
#define AUTOSAVE_JSON_PATH QString(WORKING_DIRECTORY) + QString("/") + QString(AUTOSAVE_JSON_FILE)

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
{
	setObjectName("App Core");

	qInfo() << "Startup";

	elevatePrivileges();

	qInfo() << "Registering Metatypes";
	qRegisterMetaType<WindowInfo*>();
	qRegisterMetaType<TreeItem*>();

	qInfo() << "Initializing WM objects";
	// Window Controller
	m_windowController = new WindowController();
	m_windowController->moveToThread(&m_windowControllerThread);
	connect(&m_windowControllerThread, SIGNAL(finished()), m_windowController, SLOT(deleteLater()));
	m_windowControllerThread.start();
	windowControllerChanged();

	// Window View
	m_windowView = new WindowView(this);
	windowViewChanged();

	// Windows Shell Controller
	m_winShellController = new WinShellController(this);
	winShellControllerChanged();

	// Settings Container
	m_settingsContainer = new SettingsContainer(this);
	settingsContainerChanged();

	// QML Controller
	m_qmlController = new QmlController(this);
	m_qmlController->getRootContext()->setContextProperty("appCore", this);
	qmlControllerChanged();

	qInfo() << "Creating Overlay Windows";
	// Config Window
	m_configOverlay = m_qmlController->createWindow(QUrl("qrc:/qml/overlays/ConfigOverlay.qml"), QRect(m_windowView->getOffscreenArea(), QSize(1280, 720)));
	m_configOverlay->setColor(Qt::transparent);
	m_configOverlay->setFlags(m_configOverlay->flags() | static_cast<Qt::WindowFlags>(
		Qt::WA_TranslucentBackground |
		Qt::FramelessWindowHint |
		Qt::WindowStaysOnTopHint
	));
	m_configOverlay->show();
	configOverlayChanged();

	// Window List Overlay
	m_windowListOverlay = m_qmlController->createWindow(QUrl("qrc:/qml/overlays/WindowListOverlay.qml"), QRect(m_windowView->getOffscreenArea(), QSize(640, 480)));
	m_windowListOverlay->setColor(Qt::transparent);
	m_windowListOverlay->setFlags(m_windowListOverlay->flags() | static_cast<Qt::WindowFlags>(
		Qt::WA_TranslucentBackground |
		Qt::FramelessWindowHint |
		Qt::WindowStaysOnTopHint
	));
	m_windowListOverlay->show();
	windowListOverlayChanged();

	// Power Menu Overlay
	m_powerMenuOverlay = m_qmlController->createWindow(QUrl("qrc:/qml/overlays/PowerMenuOverlay.qml"), QRect(m_windowView->getOffscreenArea(), QSize(120, 150)));
	m_powerMenuOverlay->setColor(Qt::transparent);
	m_powerMenuOverlay->setFlags(m_powerMenuOverlay->flags() | static_cast<Qt::WindowFlags>(
		Qt::WA_TranslucentBackground |
		Qt::FramelessWindowHint |
		Qt::WindowStaysOnTopHint
	));
	m_powerMenuOverlay->show();
	powerMenuOverlayChanged();

	// Item Settings Overlay
	m_itemSettingsOverlay = m_qmlController->createWindow(QUrl("qrc:/qml/overlays/ItemSettingsOverlay.qml"), QRect(m_windowView->getOffscreenArea(), QSize(640, 480)));
	m_itemSettingsOverlay->setColor(Qt::transparent);
	m_itemSettingsOverlay->setFlags(m_itemSettingsOverlay->flags() | static_cast<Qt::WindowFlags>(
		Qt::WA_TranslucentBackground |
		Qt::FramelessWindowHint |
		Qt::WindowStaysOnTopHint
	));
	m_itemSettingsOverlay->show();
	itemSettingsOverlayChanged();

	qInfo() << "Setup Connections";
	// Connections
	connect(m_windowView, SIGNAL(windowScanFinished()), this, SLOT(windowManagerReady()));

	QGuiApplication* app = static_cast<QGuiApplication*>(QGuiApplication::instance());
	connect(app, &QGuiApplication::lastWindowClosed, [=](){
		qInfo() << "All windows closed, shutting down";

		save();
		m_winShellController->cleanup();
		m_rootItem->cleanup();

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

	qInfo() << "Loading Tree Model";

	// Create nested model and pass a reference to the QML app
	m_rootItem = loadModel(AUTOSAVE_JSON_PATH);
	treeModelChanged();

	m_rootItem->startup();
	m_rootItem->updateWindowPosition();

	qInfo() << "Startup Complete";
}

void AppCore::save()
{
	qInfo() << "Saving Tree Model";
	saveModel(AUTOSAVE_JSON_PATH, m_rootItem);
}

void AppCore::shutdown()
{
	qInfo() << "Triggering Shutdown";
	m_qmlController->cleanup();
	ExitWindowsEx(EWX_SHUTDOWN, 0);
}

void AppCore::reboot()
{
	qInfo() << "Triggering Reboot";
	m_qmlController->cleanup();
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
	m_qmlController->cleanup();
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
	model->setObjectName("Root");

	QList<QScreen*> screens = qApp->screens();
	for(int i = 0; i < screens.length(); ++i)
	{
		TreeItem* monitorItem = qvariant_cast<TreeItem*>(model->addChild("Monitor", "Horizontal", "Tabbed", screens[i]));
		monitorItem->addChild("Desktop", "Horizontal", "Split");
	}

	saveModel(filename, model);
}
