#ifndef SUBPROCESSCONTROLLER_H
#define SUBPROCESSCONTROLLER_H

#include <QObject>

#include <QLoggingCategory>
Q_DECLARE_LOGGING_CATEGORY(subprocessController)

class QProcess;

struct AppSubprocess {
	AppSubprocess(QString name = "", int monitorIndex = -1, QString saveFile = "")
		: name(name)
		, monitorIndex(monitorIndex)
		, saveFile(saveFile)
	{}

	AppSubprocess(const AppSubprocess& as)
		: name(as.name)
		, monitorIndex(as.monitorIndex)
		, saveFile(as.saveFile)
	{}

	QString name;
	int monitorIndex;
	QString saveFile;
};

class SubprocessController : public QObject
{
	Q_OBJECT
public:
	explicit SubprocessController(QObject *parent = nullptr);
	~SubprocessController();

signals:
	void processStarted(AppSubprocess process);

public slots:
	void startup();
	void cleanup();

protected:
	void launchAppProcess(QString name, int monitorIndex, QString saveFile);

private:
	QProcess* m_debugProcess;

	QMap<QProcess*, AppSubprocess*> m_appProcesses;
};

#endif // SUBPROCESSCONTROLLER_H
