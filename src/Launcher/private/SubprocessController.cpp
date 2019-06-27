#include "SubprocessController.h"

#include <QFile>
#include <QJsonDocument>
#include <QJsonArray>
#include <QJsonObject>
#include <QApplication>
#include <QProcess>

#include <QDebug>
Q_LOGGING_CATEGORY(subprocessController, "launcher.subprocess")

#define APP_EXE "App.exe"

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

			launchAppProcess(monitorName, monitorIndex, "../../" + monitorFile);
		}
	}
	file.close();
}

void SubprocessController::cleanup()
{
	for(QProcess* inst : m_appProcesses.keys())
	{
		inst->terminate();
	}
}

void SubprocessController::launchAppProcess(QString name, int monitorIndex, QString saveFile)
{
	qCInfo(subprocessController) << "Launching app instance for monitor" << name << "with index" << monitorIndex << "using save file" << saveFile;

	QProcess* process = new QProcess(this);
	process->setProcessChannelMode(QProcess::ForwardedChannels);
	process->closeWriteChannel();

	QStringList mainArgs;
	mainArgs << name << QString::number(monitorIndex) << saveFile;
	process->start(APP_EXE, mainArgs);

	connect(process, &QProcess::started, [=](){
		AppSubprocess* newAppSubprocess = new AppSubprocess(name, monitorIndex, saveFile);
		m_appProcesses.insert(process, newAppSubprocess);
		emit processStarted(*newAppSubprocess);
	});

	connect(process, QOverload<int, QProcess::ExitStatus>::of(&QProcess::finished), [=](){
		qCInfo(subprocessController) << "Process finished" << process;
		m_appProcesses.remove(process);
		if(m_appProcesses.isEmpty())
		{
			qCInfo(subprocessController) << "All subprocesses finished, quitting";
			QApplication::quit();
		}
	});
}
