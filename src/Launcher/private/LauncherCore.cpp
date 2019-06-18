#include "LauncherCore.h"

#include <QApplication>
#include <QProcess>
#include <QDebug>
#include <QIcon>
#include <QSystemTrayIcon>
#include <QLocalServer>
#include <QLocalSocket>
#include <QFile>
#include <QJsonDocument>
#include <QJsonArray>
#include <QJsonObject>

#define APP_EXE "App.exe"

LauncherCore::LauncherCore(QObject* parent)
	: QObject(parent)
	, m_trayIcon(nullptr)
	, m_localServer(new QLocalServer(this))
{
	setObjectName("Launcher Core");

	qInfo("Startup");

	// Setup tray icon
	m_trayIcon = new QSystemTrayIcon(QIcon(":/graphics/logo.png"), this);
	connect(m_trayIcon, &QSystemTrayIcon::activated, [=](QSystemTrayIcon::ActivationReason reason){
		QApplication::quit();
	});

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
						for(QLocalSocket* quitSocket : m_localSockets)
						{
							QDataStream quitStream(quitSocket);
							quitStream.setVersion(QDataStream::Qt_5_12);
							quitStream << "Quit";
						}
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


	QFile file("../../launcher.json");
	file.open(QIODevice::ReadOnly);
	{
		QByteArray loadData = file.readAll();
		QJsonDocument loadDocument = QJsonDocument::fromJson(loadData);

		QJsonArray monitorArray = loadDocument.array();
		for(QJsonValue v : monitorArray)
		{
			QJsonObject monitorObject = v.toObject();

			QString monitorName = monitorObject["monitorName"].toString();
			int monitorIndex = monitorObject["monitorIndex"].toInt();
			QString monitorFile = monitorObject["monitorFile"].toString();

			qInfo() << "Launching app instance for monitor" << monitorName << " with index " << monitorIndex << " using save file " << monitorFile;
			launchAppInstance(monitorName, monitorIndex, "../../" + monitorFile);
		}
	}
	file.close();

	// Connect cleanup code
	connect(QApplication::instance(), &QApplication::aboutToQuit, [=](){
		qInfo() << "Launcher Quitting";
		for(QProcess* inst : m_appInstances)
		{
			inst->terminate();
		}
	});
}

void LauncherCore::launchAppInstance(QString name, int monitorIndex, QString saveFile)
{
	qInfo() << "Launching process for " << name;

	QProcess* process = new QProcess(this);
	QStringList mainArgs;
	mainArgs << name << QString::number(monitorIndex) << saveFile;
	process->start(APP_EXE, mainArgs);
	connect(process, &QProcess::readyReadStandardOutput, [=](){
		qInfo() << process->readAllStandardOutput();
	});
	connect(process, &QProcess::readyReadStandardError, [=](){
		qWarning() << process->readAllStandardError();
	});
	connect(process, QOverload<int, QProcess::ExitStatus>::of(&QProcess::finished), [=](){
		qInfo() << "Process finished" << process;
		m_appInstances.remove(m_appInstances.key(process));
		if(m_appInstances.empty())
		{
			qInfo() << "All subprocesses finished, quitting";
			QApplication::quit();
		}
	});
	m_appInstances.insert(name, process);
}
