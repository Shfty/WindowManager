#ifndef APPCORE_H
#define APPCORE_H

#include <QObject>
#include <QThread>

#include <QLoggingCategory>
Q_DECLARE_LOGGING_CATEGORY(appCore);

#include <Win.h>

class WindowView;
class TreeModel;
class SettingsContainer;
class QQuickWindow;
class QQuickItem;
class QWindow;
class QMLController;
class QLocalSocket;
class IPCClient;
class TreeItem;

class AppCore : public QObject
{
	Q_OBJECT

	Q_PROPERTY(WindowView* windowView MEMBER m_windowView NOTIFY windowViewChanged)
	Q_PROPERTY(SettingsContainer* settingsContainer MEMBER m_settingsContainer NOTIFY settingsContainerChanged)
	Q_PROPERTY(TreeModel* treeModel MEMBER m_treeModel NOTIFY treeModelChanged)
	Q_PROPERTY(IPCClient* ipcClient MEMBER m_ipcClient NOTIFY ipcClientChanged)

public:
	explicit AppCore(QObject* parent = nullptr);

	static AppCore* getInstance(const QObject* child) {
		QObject* candidate = child->parent();
		while(candidate != nullptr)
		{
			AppCore* launcherCore = qobject_cast<AppCore*>(candidate);
			if(launcherCore != nullptr)
			{
				return launcherCore;
			}

			candidate = candidate->parent();
		}

		return nullptr;
	}

	void logHandler(QtMsgType type, const QMessageLogContext& ctx, const QString& msg);

	WindowView* getWindowView() const { return m_windowView; }
	QMLController* getQmlController() const { return m_qmlController; }
	SettingsContainer* getSettingsContainer() const { return m_settingsContainer; }

signals:
	void windowViewChanged();
	void settingsContainerChanged();
	void treeModelChanged();
	void ipcClientChanged();

public slots:
	void setPendingWindowRecipient(TreeItem* treeItem);

private:
	void registerMetatypes();
	void makeConnections();

private slots:
	void syncObjectPropertyChanged(QString object, QString property, QVariant value);
	void windowSelected(HWND hwnd);
	void windowSelectionCanceled();
	void exitRequested();
	void lastWindowClosed();
	void cleanup();

private:
	QMLController* m_qmlController;
	WindowView* m_windowView;
	SettingsContainer* m_settingsContainer;
	TreeModel* m_treeModel;

	bool m_exitExpected;

	QThread m_ipcClientThread;
	IPCClient* m_ipcClient;

	TreeItem* m_pendingWindowRecipient;
};

#endif // APPCORE_H