#ifndef IPCSERVER_H
#define IPCSERVER_H

#include <QObject>
#include <QMap>
#include <QPoint>
#include <QSize>
#include <QVariant>

#include <QLoggingCategory>
Q_DECLARE_LOGGING_CATEGORY(ipcServer);

#include <WindowInfo.h>


class QLocalServer;
class QLocalSocket;
class LauncherCore;

class IPCServer : public QObject
{
	Q_OBJECT

public:
	explicit IPCServer(QString serverName, QObject* parent = nullptr);

public slots:
	void listen();

	void sendMessage(QString socketName, QVariantList message);
	void sendMessage(QLocalSocket* socket, QVariantList message);
	void broadcastMessage(QVariantList message);

	void broadcastWindowUpdate(QVariantList message);

	void syncObjectPropertyChanged();

	void windowAdded(HWND hwnd, QString winTitle, QString winClass, QString winProcess, qint32 winStyle);
	void windowTitleChanged(HWND hwnd, QString newTitle);
	void windowRemoved(HWND hwnd);

signals:
	void moveOverlayRequested(QPoint pos, QSize size);
	void showOverlayRequested(QString item);

	void toggleTrayRequested(QPointF pos);

	void windowListRequested(QString socketName);

	void setPendingWindowInfoSocket(QString socketName);

	void beginMoveWindows();
	void moveWindow(HWND hwnd, QPoint position, QSize size = QSize(), qlonglong layer = -2LL, quint32 extraFlags = 0);
	void endMoveWindows();
	void setWindowStyle(HWND hwnd, qint32 style);

private:
	void handleMessage(QLocalSocket* socket, QDataStream& stream, QString message);

	QString m_serverName;

	QLocalServer* m_localServer;
	QMap<QString, QLocalSocket*> m_localSockets;
	QMap<QString, QLocalSocket*> m_windowUpdateSockets;
};

#endif // IPCSERVER_H
