#ifndef SUBPROCESSCONTROLLER_H
#define SUBPROCESSCONTROLLER_H

#include <QObject>

#include <QLoggingCategory>
Q_DECLARE_LOGGING_CATEGORY(subprocessController)

class QProcess;

class SubprocessController : public QObject
{
	Q_OBJECT
public:
	explicit SubprocessController(QObject *parent = nullptr);

public slots:
	void cleanup();

protected:
	void launchAppInstance(QString name, int monitorIndex, QString saveFile);

private:
	QMap<QString, QProcess*> m_appInstances;
};

#endif // SUBPROCESSCONTROLLER_H
