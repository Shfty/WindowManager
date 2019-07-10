#include "public/IPCServer.h"

#include <QLocalServer>
#include <QLocalSocket>
#include <QDataStream>
#include <QMetaObject>
#include <QMetaProperty>
#include <QApplication>

#include <QDebug>
Q_LOGGING_CATEGORY(ipcServer, "launcher.ipcServer")

#include <SettingsContainer.h>
#include <WindowView.h>

#include "ServerCore.h"

bool operator ==(const AppClient& lhs, const AppClient& rhs)
{
	return lhs.name == rhs.name && lhs.hwnd == rhs.hwnd && lhs.wantsWindowUpdate == rhs.wantsWindowUpdate;
}

IPCServer::IPCServer(QString serverName, QObject* parent)
	: QObject(parent)
	, m_serverName(serverName)
{
	setObjectName("IPC Controller");
	qCInfo(ipcServer, "Creating local server");

	m_localServer = new QLocalServer(this);

	connect(m_localServer, &QLocalServer::newConnection, [=](){
		QLocalSocket* localSocket = m_localServer->nextPendingConnection();
		qCInfo(ipcServer) << "m_localServer newConnection" << localSocket;

		sendMessage(localSocket, {"Identify"});

		connect(localSocket, &QLocalSocket::readyRead, [=](){
			QDataStream stream(localSocket);
			stream.setVersion(QDataStream::Qt_5_12);

			while(true)
			{
				QVariant message;

				stream.startTransaction();
				stream >> message;
				QString mStr = message.toString();

				if(stream.commitTransaction())
				{
					handleMessage(localSocket, stream, mStr);
					QApplication::processEvents();
				}
				else
				{
					break;
				}
			}
		});

		connect(localSocket, static_cast<void (QLocalSocket::*)(QLocalSocket::LocalSocketError)>(&QLocalSocket::error), [=](QLocalSocket::LocalSocketError  error){
			qWarning() << "Local Socket Error" << error << localSocket->errorString();
		});
	});
}

IPCServer::~IPCServer()
{
	qDeleteAll(m_appClients.values());
	m_appClients.clear();
}

void IPCServer::startup()
{
	m_localServer->listen(m_serverName);
}

void IPCServer::sendMessage(QString socketName, QVariantList message)
{
	for(AppClient* candidate : m_appClients)
	{
		if(candidate->name == socketName)
		{
			qCInfo(ipcServer) << "Sending" << message << "to" << candidate->name;
			sendMessage(m_appClients.key(candidate), message);
			return;
		}
	}

	qCritical() << "No socket with name " + socketName;
}

void IPCServer::sendMessage(QLocalSocket* socket, QVariantList message)
{
	qCInfo(ipcServer) << "Sending" << message << "to" << socket;

	QDataStream stream(socket);
	stream.setVersion(QDataStream::Qt_5_12);

	for(QVariant submsg : message)
	{
		stream << submsg;
	}

	socket->flush();
}

void IPCServer::broadcastMessage(QVariantList message)
{
	if(m_appClients.isEmpty())
	{
		qCWarning(ipcServer) << "broadcastMessage: Tried sending" << message << "to no clients";
		return;
	}

	for(AppClient* client : m_appClients.values())
	{
		sendMessage(m_appClients.key(client), message);
	}
}

void IPCServer::broadcastWindowUpdate(QVariantList message)
{
	if(m_appClients.isEmpty())
	{
		qCWarning(ipcServer) << "broadcastWindowUpdate: Tried sending" << message << "to no clients";
		return;
	}

	for(AppClient* client : m_appClients.values())
	{
		if(client->wantsWindowUpdate)
		{
			sendMessage(m_appClients.key(client), message);
		}
	}
}

void IPCServer::syncObjectPropertyChanged()
{
	const QMetaObject* senderMeta = sender()->metaObject();

	QMetaProperty notifyProperty;
	for(int i = senderMeta->propertyOffset(); i < senderMeta->propertyCount(); ++i)
	{
		QMetaProperty prop = senderMeta->property(i);
		if(prop.notifySignalIndex() == senderSignalIndex())
		{
			notifyProperty = prop;
			break;
		}
	}

	broadcastMessage({"SyncObjectPropertyChanged", sender()->objectName(), notifyProperty.name(), notifyProperty.read(sender())});
}

void IPCServer::windowCreated(WindowInfo wi)
{
	qCInfo(ipcServer) << "IPC server window added";
	broadcastWindowUpdate({ "WindowAdded", QVariant::fromValue<WindowInfo>(wi) });
}

void IPCServer::windowTitleChanged(HWND hwnd, QString newTitle)
{
	qCInfo(ipcServer) << "IPC server window title changed";
	broadcastWindowUpdate({
		"WindowTitleChanged",
		QVariant::fromValue<HWND>(hwnd),
		newTitle
	});
}

void IPCServer::windowDestroyed(HWND hwnd)
{
	qCInfo(ipcServer) << "IPC server window removed";
	broadcastWindowUpdate({
		"WindowRemoved",
		QVariant::fromValue<HWND>(hwnd)
	});
}

void IPCServer::handleMessage(QLocalSocket* socket, QDataStream& stream, QString message)
{
	if(message == "Identify")
	{
		QVariant nameVar;
		stream >> nameVar;
		QString name = nameVar.toString();

		qCInfo(ipcServer) << "ID from" << name;

		AppClient* newClient = new AppClient(name);
		m_appClients.insert(socket, newClient);
	}
	if(message == "WindowChanged")
	{
		QVariant hwndVar;
		stream >> hwndVar;
		HWND hwnd = hwndVar.value<HWND>();

		qCInfo(ipcServer) << "Window Changed from" << m_appClients.value(socket)->name << hwnd;

		AppClient* client = m_appClients.value(socket);
		HWND oldHwnd = client->hwnd;
		client->hwnd = hwnd;

		emit clientWindowChanged(*client, oldHwnd);
	}
	else if(message == "MoveOverlay")
	{
		QVariant posVar;
		QVariant sizeVar;

		stream >> posVar;
		stream >> sizeVar;

		qCInfo(ipcServer) << "Move request from" << m_appClients.value(socket)->name << posVar << sizeVar;

		QPoint pos = posVar.toPoint();
		QSize size = sizeVar.toSize();

		emit moveOverlayRequested(pos, size);
	}
	else if(message == "ShowOverlay")
	{
		QVariant id;
		stream >> id;
		QString idStr = id.toString();

		emit showOverlayRequested(idStr);
	}
	else if(message == "ToggleTray")
	{
		QVariant posVar;
		stream >> posVar;
		QPointF pos = posVar.toPointF();

		emit toggleTrayRequested(pos);
	}
	else if(message == "SetPendingWindowInfoSocket")
	{
		emit setPendingWindowInfoSocket(m_appClients.value(socket)->name);
	}
	else if(message == "MoveWindow")
	{
		QVariant hwndVar, geometryVar, visibleVar;
		stream >> hwndVar >> geometryVar >> visibleVar;

		HWND hwnd = hwndVar.value<HWND>();
		QRect geometry = geometryVar.toRect();
		bool visible = visibleVar.toBool();

		emit moveWindow(hwnd, geometry, visible);

	}
	else if(message == "CommitWindowMove")
	{
		emit commitWindowMove();
	}
	else if(message == "MoveWindows")
	{
		QVariant countVar;
		stream >> countVar;
		int count = countVar.toInt();

		for(int i = 0; i < count; i++)
		{
			QVariant hwndVar, geometryVar, visibleVar;
			stream >> hwndVar >> geometryVar >> visibleVar;

			HWND hwnd = hwndVar.value<HWND>();
			QRect geometry = geometryVar.toRect();
			bool visible = visibleVar.toBool();

			emit moveWindow(hwnd, geometry, visible);
		}

		emit commitWindowMove();
	}
	else if(message == "SetWindowStyle")
	{
		QVariant hwndVar;
		QVariant styleVar;

		stream >> hwndVar;
		stream >> styleVar;

		HWND hwnd = hwndVar.value<HWND>();
		qint32 style = styleVar.value<qint32>();

		emit setWindowStyle(hwnd, style);
	}
	else if(message == "CloseWindow")
	{
		QVariant hwndVar;

		stream >> hwndVar;

		HWND hwnd = hwndVar.value<HWND>();

		emit closeWindow(hwnd);
	}
	else if(message == "WindowList")
	{
		AppClient* client = m_appClients.value(socket);
		qCInfo(ipcServer) << "Window list requested from" << client->name;

		client->wantsWindowUpdate = true;

		emit windowListRequested(client->name);
	}
	else if(message == "Quit")
	{
		broadcastMessage({"Quit"});
	}
	else
	{
		qCInfo(ipcServer) << "Unknown message from" << m_appClients.value(socket)->name << ":" << message;
	}
}
