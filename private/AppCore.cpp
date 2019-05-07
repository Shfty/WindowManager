#include "AppCore.h"

#include <QDebug>
#include <QFileInfo>
#include <QQmlContext>
#include <QQuickStyle>
#include <QQuickView>
#include <QScreen>

#include "ManagedWindow.h"
#include "QMLApplication.h"
#include "Settings.h"
#include "SystemWindow/TaskBarWindow.h"
#include "SystemWindow/TrayIconWindow.h"
#include "TrayIcon.h"
#include "TreeItem.h"
#include "Win.h"
#include "WindowManager.h"

#define WORKING_DIRECTORY "P:/Personal/C++/WindowManager"
#define AUTOSAVE_FILE "autosave.dat"
#define AUTOSAVE_PATH QString(WORKING_DIRECTORY) + QString("/") + QString(AUTOSAVE_FILE)

AppCore::AppCore(QObject* parent)
	: QObject(parent)
	, m_trayIcon(new TrayIcon(this))
	, m_trayIconWindow(new TrayIconWindow(this))
	, m_taskBarWindow(new TaskBarWindow(this))
	, m_rootItem(nullptr)
{
	// Connect save and close handlers
	QObject::connect(&WindowManager::instance(), SIGNAL(windowScanFinished()), this, SLOT(windowManagerReady()));
	elevatePrivileges();
}

AppCore::~AppCore()
{
	
}

void AppCore::windowManagerReady()
{
	// Create nested model and pass a reference to the QML app
	m_rootItem = loadModel(AUTOSAVE_PATH);

	QMLApplication& qmlApp = QMLApplication::instance();

	qmlApp.getRootContext()->setContextProperty("appCore", this);
	qmlApp.getRootContext()->setContextProperty("treeItem", m_rootItem);
	m_rootItem->moveWindowOnscreen();

	qmlApp.showConfigWindow();

	// Disconnect signal
	QObject::disconnect(&WindowManager::instance(), SIGNAL(windowScanFinished()), this, SLOT(windowManagerReady()));

	// Connect QML signals
	QObject::connect(m_trayIcon, SIGNAL(onClicked()), &qmlApp, SLOT(showConfigWindow()));

	/*
	QObject::connect(root, SIGNAL(showTrayIconWindow(QPointF)), this, SLOT(showTrayIconWindow(QPointF)));
	QObject::connect(root, SIGNAL(shutdown()), this, SLOT(shutdown()));
	QObject::connect(root, SIGNAL(restart()), this, SLOT(restart()));
	QObject::connect(root, SIGNAL(sleep()), this, SLOT(sleep()));
	*/
}

void AppCore::showTrayIconWindow(QPointF position)
{
	m_trayIconWindow->setPosition(position.toPoint());
	m_trayIconWindow->toggle();
}

void AppCore::shutdown()
{
	ExitWindowsEx(EWX_SHUTDOWN, 0);
}

void AppCore::restart()
{
	ExitWindowsEx(EWX_REBOOT, 0);
}

void AppCore::sleep()
{
	SetSuspendState(true, false, false);
}

void AppCore::handleAboutToQuit()
{
	this->handleSave();
}

TreeItem* AppCore::loadModel(QString filename)
{
	bool fileExists = QFileInfo::exists(AUTOSAVE_PATH) && QFileInfo(AUTOSAVE_PATH).isFile();
	if(!fileExists)
	{
		saveDefaultModel(AUTOSAVE_PATH);
	}

	TreeItem* model = new TreeItem(this);

	QFile file(filename);
	file.open(QIODevice::ReadOnly);

	QDataStream in(&file);
	in >> model;

	file.close();

	return model;
}

void AppCore::saveModel(QString filename, const TreeItem* model)
{
	QFile file(filename);
	file.open(QIODevice::WriteOnly);

	QDataStream out(&file);
	out << model;

	file.close();
}

void AppCore::saveDefaultModel(QString filename)
{
	TreeItem* model = new TreeItem(nullptr);
	model->setObjectName("Root");

	QList<QScreen*> screens = qApp->screens();
	for(int i = 0; i < screens.length(); ++i)
	{
		TreeItem* monitorItem = qvariant_cast<TreeItem*>(model->addChild("Monitor", "Horizontal", "Tabbed", i));
		monitorItem->addChild("Desktop", "Horizontal", "Split");
	}

	saveModel(filename, model);
}

void AppCore::handleSave()
{
	saveModel(AUTOSAVE_PATH, m_rootItem);
}

void AppCore::handleQuit()
{
	handleSave();
	m_trayIcon->hide();
	qApp->quit();
}

void AppCore::elevatePrivileges()
{
	HANDLE hToken = nullptr;

	int result = OpenProcessToken(GetCurrentProcess(), TOKEN_ADJUST_PRIVILEGES | TOKEN_QUERY, &hToken);

	TOKEN_PRIVILEGES tkp;

	result = LookupPrivilegeValue(nullptr, SE_SHUTDOWN_NAME, &tkp.Privileges[0].Luid);

	tkp.PrivilegeCount = 1;
	tkp.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED;

	result = AdjustTokenPrivileges(hToken, false, &tkp, 0, nullptr, 0);
}
