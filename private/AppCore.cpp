#include "AppCore.h"

#include <QScreen>
#include <QDebug>
#include <QFileInfo>

#include "TreeItem.h"
#include "WindowManager.h"
#include "SystemWindow/TrayIconWindow.h"
#include "SystemWindow/TaskBarWindow.h"
#include "TrayIcon.h"
#include "QMLApplication.h"

#define WORKING_DIRECTORY "P:/Personal/C++/WindowManager"
#define AUTOSAVE_FILE "autosave.dat"
#define AUTOSAVE_PATH QString(WORKING_DIRECTORY) + QString("/") + QString(AUTOSAVE_FILE)

AppCore::AppCore(QObject *parent)
	: QObject(parent),
	  m_rootItem(nullptr)
{
	m_qmlApp = new QMLApplication(this);

	// Quit if QML application failed to load
	if (m_qmlApp->rootObjects().isEmpty())
	{
		qApp->quit();
	}

	m_trayIcon = new TrayIcon(this);
	m_trayIconWindow = new TrayIconWindow(this);
	m_taskBarWindow = new TaskBarWindow(this);

	auto root = m_qmlApp->rootObjects().first();

	// Connect save and close handlers
	QObject::connect(&WindowManager::instance(), SIGNAL(windowScanFinished()), this, SLOT(windowManagerReady()));

	QObject::connect(m_trayIcon, SIGNAL(onQuitTriggered()), this, SLOT(handleQuit()));
	QObject::connect(m_trayIcon, SIGNAL(onClicked()), root, SIGNAL(showConfigWindow()));

	QObject::connect(root, SIGNAL(showTrayIconWindow(QPointF)), this, SLOT(showTrayIconWindow(QPointF)));
}

AppCore::~AppCore()
{
}

void AppCore::handleTrayIconClicked()
{
	m_qmlApp->showWindow();
}

void AppCore::showTrayIconWindow(QPointF position)
{
	m_trayIconWindow->setPosition(position.toPoint());
	m_trayIconWindow->toggle();
}

void AppCore::windowManagerReady()
{
	// Create nested model and pass a reference to the QML app
	m_rootItem = loadModel(AUTOSAVE_PATH);
	m_qmlApp->setModel(m_rootItem);

	// Disconnect signal
	QObject::disconnect(&WindowManager::instance(), SIGNAL(windowScanFinished()), this, SLOT(windowManagerReady()));
}

void AppCore::handleAboutToQuit()
{
	this->handleSave();
}

TreeItem* AppCore::loadModel(QString filename)
{
	bool fileExists = QFileInfo::exists(AUTOSAVE_PATH) && QFileInfo(AUTOSAVE_PATH).isFile();
	if (!fileExists)
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

	QList<QScreen *> screens = qApp->screens();
	for (int i = 0; i < screens.length(); ++i)
	{
		QScreen* screen = screens.at(i);

		TreeItem* monitorItem = qvariant_cast<TreeItem*>(model->addChild("Monitor", "Horizontal", "Tabbed", screen->geometry(), screen->refreshRate()));
		TreeItem* desktopItem = qvariant_cast<TreeItem*>(monitorItem->addChild("Desktop", "Horizontal", "Split"));
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