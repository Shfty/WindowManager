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

#include "LauncherCore.h"

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

void IPCServer::listen()
{
	m_localServer->listen(m_serverName);
}

void IPCServer::sendMessage(QString socketName, QVariantList message)
{
	QLocalSocket* socket = m_localSockets.value(socketName);
	Q_ASSERT(socket != nullptr);

	sendMessage(socket, message);
}

void IPCServer::sendMessage(QLocalSocket* socket, QVariantList message)
{
	qCInfo(ipcServer) << "Sending" << message << "to" << m_localSockets.key(socket);

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
	for(QLocalSocket* socket : m_localSockets.values())
	{
		sendMessage(socket, message);
	}
}

void IPCServer::broadcastWindowUpdate(QVariantList message)
{
	qCInfo(ipcServer) << "Broadcasting window update";
	for(QLocalSocket* socket : m_windowUpdateSockets.values())
	{
		sendMessage(socket, message);
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

void IPCServer::windowAdded(HWND hwnd, QString winTitle, QString winClass, QString winProcess, qint32 winStyle)
{
	qCInfo(ipcServer) << "IPC server window added";
	broadcastWindowUpdate({
		"WindowAdded",
		QVariant::fromValue<HWND>(hwnd),
		winTitle,
		winClass,
		winProcess,
		QVariant(winStyle)
	});
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

void IPCServer::windowRemoved(HWND hwnd)
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
		QVariant id;
		stream >> id;
		QString idStr = id.toString();

		qCInfo(ipcServer) << "ID from" << idStr;
		m_localSockets.insert(idStr, socket);
	}
	if(message == "Log")
	{
		QVariant type;
		QVariant category;
		QVariant msg;
		stream >> type;
		stream >> category;
		stream >> msg;
		QtMsgType msgType = QtMsgType(type.toInt());
		QString categoryStr = category.toString();
		QString msgStr = msg.toString();

		QString msgFmt;
		QTextStream ts(&msgFmt);
		ts << "[" << m_localSockets.key(socket) << "] " << categoryStr << ": " << msgStr;

		switch(msgType) {
			case QtMsgType::QtDebugMsg:
				qDebug().noquote().nospace() << msgFmt;
				break;
			case QtMsgType::QtInfoMsg:
				qInfo().noquote().nospace() << msgFmt;
				break;
			case QtMsgType::QtWarningMsg:
				qWarning().noquote().nospace() << msgFmt;
				break;
			case QtMsgType::QtCriticalMsg:
				qCritical().noquote().nospace() << msgFmt;
				break;
			case QtMsgType::QtFatalMsg:
				qFatal(msgFmt.toStdString().c_str());
				break;
		}
	}
	else if(message == "MoveOverlay")
	{
		QVariant posVar;
		QVariant sizeVar;

		stream >> posVar;
		stream >> sizeVar;

		qCInfo(ipcServer) << "Move request from" << m_localSockets.key(socket) << posVar << sizeVar;

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
		emit setPendingWindowInfoSocket(m_localSockets.key(socket));
	}
	else if(message == "MoveWindows")
	{
		QVariant countVar;
		stream >> countVar;
		int count = countVar.toInt();

		emit beginMoveWindows();

		for(int i = 0; i < count; i++)
		{
			QVariant hwndVar, positionVar, sizeVar, layerVar, extraFlagsVar;
			stream >> hwndVar >> positionVar >> sizeVar >> layerVar >> extraFlagsVar;

			HWND hwnd = hwndVar.value<HWND>();
			QPoint position = positionVar.toPoint();
			QSize size = sizeVar.toSize();
			qlonglong layer = layerVar.toLongLong();
			quint32 extraFlags = extraFlagsVar.value<quint32>();

			emit moveWindow(hwnd, position, size, layer, extraFlags);
		}

		emit endMoveWindows();
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
	else if(message == "WindowList")
	{
		qCInfo(ipcServer) << "Window list requested from" << m_localSockets.key(socket);
		m_windowUpdateSockets.insert(m_localSockets.key(socket), socket);

		emit windowListRequested(m_localSockets.key(socket));
	}
	else if(message == "Quit")
	{
		broadcastMessage({"Quit"});
	}
	else
	{
		qCInfo(ipcServer) << "Unknown message from" << m_localSockets.key(socket) << ":" << message;
	}
}
