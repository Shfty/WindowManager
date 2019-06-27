#ifndef IPCSERVER_H
#define IPCSERVER_H

#include <QObject>
#include <QMap>
#include <QRect>
#include <QVariant>

#include <QLoggingCategory>
Q_DECLARE_LOGGING_CATEGORY(ipcServer);

#include <WindowInfo.h>


class QLocalServer;
class QLocalSocket;
class LauncherCore;

struct AppClient {
	AppClient(QString name = "", HWND hwnd = nullptr)
		: name(name)
		, hwnd(hwnd)
		, wantsWindowUpdate(false)
	{}

	AppClient(const AppClient& ac)
		: name(ac.name)
		, hwnd(ac.hwnd)
		, wantsWindowUpdate(ac.wantsWindowUpdate)
	{}

	QString name;
	HWND hwnd;
	bool wantsWindowUpdate;
};

Q_DECLARE_METATYPE(AppClient);

class IPCServer : public QObject
{
	Q_OBJECT

public:
	explicit IPCServer(QString serverName, QObject* parent = nullptr);
	~IPCServer();

signals:
	void socketReady(AppClient client);

	void moveOverlayRequested(QPoint pos, QSize size);
	void showOverlayRequested(QString item);

	void toggleTrayRequested(QPointF pos);

	void windowListRequested(QString socketName);

	void setPendingWindowInfoSocket(QString socketName);

	void beginMoveWindows();
	void moveWindow(HWND hwnd, QRect geometry, bool visible);
	void endMoveWindows();
	void setWindowStyle(HWND hwnd, qint32 style);

public slots:
	void startup();

	void sendMessage(QString socketName, QVariantList message);
	void sendMessage(QLocalSocket* socket, QVariantList message);
	void broadcastMessage(QVariantList message);

	void broadcastWindowUpdate(QVariantList message);

	void syncObjectPropertyChanged();

	void windowAdded(HWND hwnd, QString winTitle, QString winClass, QString winProcess, qint32 winStyle);
	void windowTitleChanged(HWND hwnd, QString newTitle);
	void windowRemoved(HWND hwnd);

private:
	void handleMessage(QLocalSocket* socket, QDataStream& stream, QString message);

	QString m_serverName;

	QLocalServer* m_localServer;
	QMap<QLocalSocket*, AppClient*> m_appClients;
};

#endif // IPCSERVER_H
