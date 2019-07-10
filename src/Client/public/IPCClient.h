#ifndef IPCCLIENT_H
#define IPCCLIENT_H

#include <QObject>
#include <QVariant>

#include <QLoggingCategory>
Q_DECLARE_LOGGING_CATEGORY(ipcClient);

#include <Win.h>
#include <WindowInfo.h>

class QLocalSocket;
class TreeItem;

class IPCClient : public QObject
{
	Q_OBJECT
public:
	explicit IPCClient(QString socketName, QObject *parent = nullptr);

signals:
	void ipcReady();

	void syncObjectPropertyChanged(QString objectName, QString objectProperty, QVariant propertyValue);

	void windowCreated(WindowInfo wi);
	void windowTitleChanged(HWND hwnd, QString title);
	void windowDestroyed(HWND hwnd);

	void windowSelected(HWND hwnd);
	void windowSelectionCanceled();

	void receivedWindowList();
	void receivedQuitMessage();

	void reloadQml();

	void disconnected();

public slots:
	void startup();
	void sendMessage(QVariantList message);

private:
	void handleMessage(QDataStream& stream, QString message);

	QString m_socketName;

	QLocalSocket* m_localSocket;
	QVariantList m_windowMoveBuffer;
};

#endif // IPCCLIENT_H
