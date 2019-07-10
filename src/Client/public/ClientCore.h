#ifndef CLIENTCORE_H
#define CLIENTCORE_H

#include <QObject>
#include <QThread>

#include <QLoggingCategory>
Q_DECLARE_LOGGING_CATEGORY(clientCore);

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

class ClientCore : public QObject
{
	Q_OBJECT

	Q_PROPERTY(WindowView* windowView MEMBER m_windowView NOTIFY windowViewChanged)
	Q_PROPERTY(SettingsContainer* settingsContainer MEMBER m_settingsContainer NOTIFY settingsContainerChanged)
	Q_PROPERTY(TreeModel* treeModel MEMBER m_treeModel NOTIFY treeModelChanged)

public:
	explicit ClientCore(QObject* parent = nullptr);

	static ClientCore* getInstance(const QObject* child) {
		QObject* candidate = child->parent();
		while(candidate != nullptr)
		{
			ClientCore* clientCore = qobject_cast<ClientCore*>(candidate);
			if(clientCore != nullptr)
			{
				return clientCore;
			}

			candidate = candidate->parent();
		}

		return nullptr;
	}

	WindowView* getWindowView() const { return m_windowView; }

signals:
	void windowViewChanged();
	void settingsContainerChanged();
	void treeModelChanged();

public slots:
	void setPendingWindowRecipient(TreeItem* treeItem);
	void sendMessage(QVariantList message);

private:
	void registerMetatypes();
	void makeConnections();

private slots:
	void windowReady(HWND hwnd);
	void windowDestroyed();

	void ipcReady();

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

#endif // CLIENTCORE_H