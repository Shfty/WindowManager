#ifndef LAUNCHERCORE_H
#define LAUNCHERCORE_H

#include <QObject>
#include <QIcon>
#include <QMap>

class QProcess;
class QSystemTrayIcon;
class QMenu;
class QLocalServer;
class QLocalSocket;

class LauncherCore : public QObject
{
	Q_OBJECT

public:
	explicit LauncherCore(QObject* parent = nullptr);

	void launchAppInstance(QString name, QString saveFile);

private:
	QSystemTrayIcon* m_trayIcon;
	QMenu* m_trayMenu;
	QMap<QString, QProcess*> m_appInstances;

	QLocalServer* m_localServer;
	QMap<QString, QLocalSocket*> m_localSockets;
};

#endif // APPCORE_H