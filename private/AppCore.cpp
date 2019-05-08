#include "AppCore.h"

#include <QFileInfo>
#include <QScreen>
#include <QGuiApplication>
#include <QQuickWindow>
#include <QQmlContext>

#include "WindowController.h"
#include "WindowView.h"
#include "WinShellController.h"
#include "TreeItem.h"
#include "QmlController.h"
#include "SettingsContainer.h"

#define WORKING_DIRECTORY "P:/Personal/C++/WindowManager"
#define AUTOSAVE_FILE "autosave.dat"
#define AUTOSAVE_PATH QString(WORKING_DIRECTORY) + QString("/") + QString(AUTOSAVE_FILE)

AppCore::AppCore(QObject* parent)
	: QObject(parent)
	, m_windowController(new WindowController(this))
	, m_windowView(new WindowView(this))
	, m_winShellController(new WinShellController(this))
	, m_settingsContainer(new SettingsContainer(this))
	, m_qmlController(new QmlController(this))
	, m_rootItem(nullptr)
	, m_configWindow(nullptr)
{
	elevatePrivileges();

	qRegisterMetaType<TreeItem*>();
	setObjectName("App Core");

	m_qmlController->getRootContext()->setContextProperty("appCore", this);
	m_configWindow = m_qmlController->createWindow(QUrl("qrc:/qml/config/ConfigWindow.qml"), QRect(0, 0, 1280, 720));

	connect(QGuiApplication::instance(), SIGNAL(aboutToQuit()), this, SLOT(save()));
	connect(m_windowView, SIGNAL(windowScanFinished()), this, SLOT(windowManagerReady()));
}

AppCore::~AppCore()
{

}

void AppCore::windowManagerReady()
{
	// Create nested model and pass a reference to the QML app
	m_rootItem = loadModel(AUTOSAVE_PATH);
	m_rootItem->moveWindowOnscreen();
	treeModelChanged();

	// Disconnect signal
	QObject::disconnect(m_windowView, SIGNAL(windowScanFinished()), this, SLOT(windowManagerReady()));
}

void AppCore::save()
{
	saveModel(AUTOSAVE_PATH, m_rootItem);
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
