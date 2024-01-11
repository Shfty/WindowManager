#include "SubprocessController.h"

#include <QDir>
#include <QFile>
#include <QJsonDocument>
#include <QJsonArray>
#include <QJsonObject>
#include <QApplication>
#include <QProcess>

#include <QDebug>
Q_LOGGING_CATEGORY(subprocessController, "launcher.subprocess")

#ifdef QT_DEBUG
#define APP_EXE "./build/src/Client/debug/Client.exe"
#define DEBUG_EXE "./build/src/WindowList/debug/WindowList.exe"
#else
#define APP_EXE "./build/src/Client/release/Client.exe"
#define DEBUG_EXE "./build/src/WindowList/release/WindowList.exe"
#endif

SubprocessController::SubprocessController(QObject *parent)
	: QObject(parent)
	, m_debugProcess(nullptr)
{
	setObjectName("Subprocess Controller");
	qCInfo(subprocessController) << "Construction";
}

SubprocessController::~SubprocessController()
{
	qDeleteAll(m_appProcesses.keys());
	m_appProcesses.clear();
}

void SubprocessController::startup()
{
	qCInfo(subprocessController) << "Startup";

	// Load save file and launch app instances
	QFile file("launcher.json");

	if(file.open(QIODevice::ReadOnly))
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

			launchAppProcess(monitorName, monitorIndex, monitorFile);
		}

		file.close();
	}
	else
	{
		qFatal("Could not open Launcher save file");
	}

	// Launch debug helper
	m_debugProcess = new QProcess(this);
	m_debugProcess->setWorkingDirectory(QDir::currentPath());
	m_debugProcess->setProcessChannelMode(QProcess::ForwardedChannels);
	m_debugProcess->closeWriteChannel();

	connect(m_debugProcess, &QProcess::errorOccurred, [=](QProcess::ProcessError error){
		Q_UNUSED(error)

		QString errorString = "Debug process error: " + m_debugProcess->errorString();
		qFatal(errorString.toStdString().c_str());
	});

	connect(m_debugProcess, QOverload<int, QProcess::ExitStatus>::of(&QProcess::finished), [=](){
		qCInfo(subprocessController) << "Debug process finished" << m_debugProcess;
		m_debugProcess->deleteLater();
	});

	m_debugProcess->start(DEBUG_EXE);
}

void SubprocessController::cleanup()
{
	qCInfo(subprocessController) << "Cleanup";

	m_debugProcess->terminate();
	m_debugProcess->waitForFinished();

	for(QProcess* inst : m_appProcesses.keys())
	{
		inst->terminate();
		inst->waitForFinished(-1);
	}
}

void SubprocessController::launchAppProcess(QString name, int monitorIndex, QString saveFile)
{
	qCInfo(subprocessController) << "Launching app instance for monitor" << name << "with index" << monitorIndex << "using save file" << saveFile;

	QProcess* process = new QProcess(this);
	process->setWorkingDirectory(QDir::currentPath());
	process->setProcessChannelMode(QProcess::ForwardedChannels);
	process->closeWriteChannel();

	connect(process, &QProcess::started, [=](){
		qCInfo(subprocessController) << "Process started" << process;
		AppSubprocess* newAppSubprocess = new AppSubprocess(name, monitorIndex, saveFile);
		m_appProcesses.insert(process, newAppSubprocess);
		emit processStarted(*newAppSubprocess);
	});

	connect(process, &QProcess::errorOccurred, [=](QProcess::ProcessError error){
		Q_UNUSED(error)

		QString errorString = "Subprocess error: " + process->errorString();
		qFatal(errorString.toStdString().c_str());
	});

	connect(process, QOverload<int, QProcess::ExitStatus>::of(&QProcess::finished), [=](){
		qCInfo(subprocessController) << "Process finished" << process;
		process->deleteLater();
		m_appProcesses.remove(process);
		if(m_appProcesses.isEmpty())
		{
			qCInfo(subprocessController) << "All subprocesses finished, quitting";
			QApplication::quit();
		}
	});

	QStringList mainArgs;
	mainArgs << name << QString::number(monitorIndex) << saveFile;
	process->start(APP_EXE, mainArgs);
}
