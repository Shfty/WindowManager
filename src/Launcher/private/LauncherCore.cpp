#include "LauncherCore.h"

#include <QApplication>
#include <QProcess>
#include <QDebug>
#include <QIcon>
#include <QMenu>
#include <QSystemTrayIcon>
#include <QAction>
#include <QLocalServer>
#include <QLocalSocket>

#define APP_EXE "App.exe"

LauncherCore::LauncherCore(QObject* parent)
	: QObject(parent)
	, m_trayIcon(nullptr)
	, m_trayMenu(nullptr)
	, m_localServer(new QLocalServer(this))
{
	setObjectName("Launcher Core");

	qInfo("Startup");

	// Setup tray icon
	QAction* exitAction = new QAction("Exit", this);
	connect(exitAction, &QAction::triggered, this, [=](){
		QApplication::quit();
	});

	m_trayMenu = new QMenu(nullptr);
	m_trayMenu->addAction(exitAction);

	m_trayIcon = new QSystemTrayIcon(QIcon(":/graphics/logo.png"), this);
	m_trayIcon->setContextMenu(m_trayMenu);

	m_trayIcon->show();

	// Setup local socket server
	qInfo() << "Creating m_localServer";
	m_localServer = new QLocalServer(this);
	m_localServer->listen("WindowManager");
	connect(m_localServer, &QLocalServer::newConnection, [=](){
		QLocalSocket* localSocket = m_localServer->nextPendingConnection();
		qInfo() << "m_localServer newConnection" << localSocket;

		QDataStream stream(localSocket);
		stream << "Identify";

		connect(localSocket, &QLocalSocket::readyRead, [=](){
			QString key = m_localSockets.key(localSocket);

			QDataStream stream(localSocket);
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
						QByteArray id;
						stream >> id;
						QString idStr = QString::fromUtf8(id);

						qInfo() << "ID from" << idStr;
						m_localSockets.insert(id, localSocket);

						stream << "Chatter";
					}
					else if(mStr == "Quit")
					{
						QApplication::quit();
					}
					else
					{
						qInfo() << "Unknown message from" << key << ":" << mStr;
					}
				}
				else
				{
					break;
				}
			}
		});
	});

	// Launch app instances
	launchAppInstance("Main", "../../main.json");
	launchAppInstance("Video", "../../video.json");
	launchAppInstance("Chat", "../../chat.json");

	// Connect cleanup code
	connect(QApplication::instance(), &QApplication::aboutToQuit, [=](){
		qInfo() << "Launcher Quitting";
		for(QProcess* inst : m_appInstances)
		{
			inst->terminate();
		}
	});
}

void LauncherCore::launchAppInstance(QString name, QString saveFile)
{
	qInfo() << "Launching process for " << name;

	QProcess* process = new QProcess(this);
	QStringList mainArgs;
	mainArgs << name << saveFile;
	process->start(APP_EXE, mainArgs);
	connect(process, &QProcess::readyReadStandardOutput, [=](){
		qInfo() << process->readAllStandardOutput();
	});
	m_appInstances.insert(name, process);
}
