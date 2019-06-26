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

			qCInfo(subprocessController) << "Launching app instance for monitor" << monitorName << " with index " << monitorIndex << " using save file " << monitorFile;
			launchAppInstance(monitorName, monitorIndex, "../../" + monitorFile);
		}
	}
	file.close();
}

void SubprocessController::cleanup()
{
	for(QProcess* inst : m_appInstances)
	{
		inst->terminate();
	}
}

void SubprocessController::launchAppInstance(QString name, int monitorIndex, QString saveFile)
{
	qCInfo(subprocessController) << "Launching process for " << name;

	QProcess* process = new QProcess(this);
	process->closeReadChannel(QProcess::ProcessChannel::StandardOutput);
	process->closeReadChannel(QProcess::ProcessChannel::StandardError);
	process->closeWriteChannel();

	QStringList mainArgs;
	mainArgs << name << QString::number(monitorIndex) << saveFile;
	process->start(APP_EXE, mainArgs);

	connect(process, QOverload<int, QProcess::ExitStatus>::of(&QProcess::finished), [=](){
		qCInfo(subprocessController) << "Process finished" << process;
		m_appInstances.remove(m_appInstances.key(process));
		if(m_appInstances.isEmpty())
		{
			qCInfo(subprocessController) << "All subprocesses finished, quitting";
			QApplication::quit();
		}
	});

	m_appInstances.insert(name, process);
}
