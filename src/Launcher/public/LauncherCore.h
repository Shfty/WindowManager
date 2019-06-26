#ifndef LAUNCHERCORE_H
#define LAUNCHERCORE_H

#include <QObject>
#include <QIcon>
#include <QMap>
#include <QThread>

#include <QLoggingCategory>
Q_DECLARE_LOGGING_CATEGORY(launcherCore)
Q_DECLARE_LOGGING_CATEGORY(subprocess)

class TrayIcon;
class SubprocessController;
class WinShellController;
class WindowModel;
class SettingsContainer;
class WindowController;
class WindowView;
class OverlayController;
class IPCServer;

class LauncherCore : public QObject
{
	Q_OBJECT

	Q_PROPERTY(WinShellController* winShellController MEMBER m_winShellController NOTIFY winShellControllerChanged)
	Q_PROPERTY(WindowView* windowView MEMBER m_windowView NOTIFY windowViewChanged)
	Q_PROPERTY(IPCServer* ipcServer MEMBER m_ipcServer NOTIFY ipcServerChanged)
	Q_PROPERTY(SettingsContainer* settingsContainer MEMBER m_settingsContainer NOTIFY settingsContainerChanged)

public:
	explicit LauncherCore(QObject* parent = nullptr);

	static LauncherCore* getInstance(const QObject* child) {
		QObject* candidate = child->parent();
		while(candidate != nullptr)
		{
			LauncherCore* launcherCore = qobject_cast<LauncherCore*>(candidate);
			if(launcherCore != nullptr)
			{
				return launcherCore;
			}

			candidate = candidate->parent();
		}

		return nullptr;
	}

	WindowView* getWindowView() const { return m_windowView; }
	SettingsContainer* getSettingsContainer() const { return m_settingsContainer; }

	void launchAppInstance(QString name, int monitorIndex, QString saveFile);

signals:
	void winShellControllerChanged();
	void windowViewChanged();
	void ipcServerChanged();
	void settingsContainerChanged();

	void startup();

	void sendMessage(QString socketName, QVariantList message);

private:
	void registerMetatypes();
	void makeConnections();
	void registerSynchronizedObject(QObject* object);

	TrayIcon* m_trayIcon;
	SubprocessController* m_subprocessController;
	WinShellController* m_winShellController;
	SettingsContainer* m_settingsContainer;
	WindowModel* m_windowModel;
	WindowView* m_windowView;
	WindowController* m_windowController;
	OverlayController* m_overlayController;

	QThread m_ipcServerThread;
	IPCServer* m_ipcServer;

	QString m_pendingWindowInfoRecipient;
};

#endif // APPCORE_H