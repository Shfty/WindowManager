#include "SubprocessController.h"

#include <QFile>
#include <QJsonDocument>
#include <QJsonArray>
#include <QJsonObject>
#include <QApplication>
#include <QProcess>

#include <QDebug>
Q_LOGGING_CATEGORY(subprocessController, "launcher.subprocess")

#ifdef QT_DEBUG
#define APP_EXE "./build/src/App/debug/App.exe"
#else
#define APP_EXE "./build/src/App/release/App.exe"
#endif

SubprocessController::SubprocessController(QObject *parent)
	: QObject(parent)
{

}

SubprocessController::~SubprocessController()
{
	qDeleteAll(m_appProcesses.keys());
	m_appProcesses.clear();
}

void SubprocessController::startup()
{
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

}

void SubprocessController::cleanup()
{
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
