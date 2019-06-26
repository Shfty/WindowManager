#ifndef IPCCLIENT_H
#define IPCCLIENT_H

#include <QObject>
#include <QVariant>
#include <QSize>

#include <QLoggingCategory>
Q_DECLARE_LOGGING_CATEGORY(ipcClient);

#include <Win.h>

class QLocalSocket;
class TreeItem;

class IPCClient : public QObject
{
	Q_OBJECT
public:
	explicit IPCClient(QObject *parent = nullptr);

signals:
	void syncObjectPropertyChanged(QString objectName, QString objectProperty, QVariant propertyValue);

	void windowAdded(HWND hwnd, QString title, QString winClass, QString winProcess, qint32 winStyle);
	void windowTitleChanged(HWND hwnd, QString title);
	void windowRemoved(HWND hwnd);

	void windowSelected(HWND hwnd);
	void windowSelectionCanceled();

	void receivedWindowList();
	void receivedQuitMessage();

	void disconnected();

public slots:
	void connectToServer();

	void sendMessage(QVariantList message);

	void setWindowStyle(HWND hwnd, qint32 style);

private:
	void handleMessage(QDataStream& stream, QString message);

	void unpackWindowInfo(QDataStream& stream, HWND& hwnd, QString& winTitle, QString& winClass, QString& winProcess, qint32& winStyle);

	QString m_socketName;
	QLocalSocket* m_localSocket;
	QVariantList m_windowMoveBuffer;
};

#endif // IPCCLIENT_H
