#ifndef SERVERCORE_H
#define SERVERCORE_H

#include <QObject>
#include <QIcon>
#include <QMap>
#include <QThread>

#include <QLoggingCategory>
Q_DECLARE_LOGGING_CATEGORY(serverCore)
Q_DECLARE_LOGGING_CATEGORY(subprocess)

#include "IPCServer.h"

class TrayIcon;
class WinShellController;
class WindowModelThread;
class WindowModel;
class SettingsContainer;
class WindowController;
class WindowView;
class OverlayController;
class QQuickWindow;
class SubprocessController;

class ServerCore : public QObject
{
	Q_OBJECT

	Q_PROPERTY(WinShellController* winShellController MEMBER m_winShellController NOTIFY winShellControllerChanged)
	Q_PROPERTY(WindowView* windowView MEMBER m_windowView NOTIFY windowViewChanged)
	Q_PROPERTY(IPCServer* ipcServer MEMBER m_ipcServer NOTIFY ipcServerChanged)
	Q_PROPERTY(SettingsContainer* settingsContainer MEMBER m_settingsContainer NOTIFY settingsContainerChanged)

public:
	explicit ServerCore(QObject* parent = nullptr);

	static ServerCore* getInstance(const QObject* child) {
		QObject* candidate = child->parent();
		while(candidate != nullptr)
		{
			ServerCore* serverCore = qobject_cast<ServerCore*>(candidate);
			if(serverCore != nullptr)
			{
				return serverCore;
			}

			candidate = candidate->parent();
		}

		return nullptr;
	}

signals:
	void winShellControllerChanged();
	void windowViewChanged();
	void ipcServerChanged();
	void settingsContainerChanged();

	void sendMessage(QString socketName, QVariantList message);

private:
	void registerMetatypes();
	void makeConnections();
	void registerSynchronizedObject(QObject* object);

private slots:
	void clientWindowChanged(AppClient client, HWND oldHwnd);

	void windowListRequested(QString socketName);
	void setPendingWindowInfoSocket(QString socketName);
	void windowSelected(QVariant windowInfoVar);
	void reloadRequested();
	void quitRequested();
	void cleanup();

private:
	TrayIcon* m_trayIcon;
	SubprocessController* m_subprocessController;
	WinShellController* m_winShellController;
	SettingsContainer* m_settingsContainer;
	WindowModel* m_windowModel;
	WindowView* m_windowView;
	WindowController* m_windowController;
	OverlayController* m_overlayController;

	QThread m_subprocessControllerThread;
	QThread m_windowModelThread;
	QThread m_ipcServerThread;
	QThread m_windowControllerThread;
	IPCServer* m_ipcServer;

	QString m_pendingWindowInfoSocket;
};

#endif // SERVERCORE_H