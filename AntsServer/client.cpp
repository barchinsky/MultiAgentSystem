#include <qdatastream.h>

#include "client.h"
#include "server.h"

Client::Client(int handle, Server *serv, QObject *parent):
    QObject(parent)
{
    _server = serv;

    _socket = new QTcpSocket(this);
    _socket->setSocketDescriptor(handle);

    connect(_socket, SIGNAL(connected()), this, SLOT(onConnect()));
    connect(_socket, SIGNAL(disconnected()), this, SLOT(onDisconnect()));
    connect(_socket, SIGNAL(readyRead()), this, SLOT(onReadyRead()));
    connect(_socket, SIGNAL(error(QAbstractSocket::SocketError)), this, SLOT(onError(QAbstractSocket::SocketError)));

    qDebug() << "Client connected " << handle;
}

// slots

void Client::onConnect()
{
    qDebug() << "Client connected";
    //never calls, socket already connected to the tcpserver
    //we just binding to this socket here: _sok->setSocketDescriptor(desc);
}

void Client::onDisconnect()
{
    qDebug() << "Client disconnected";
    emit removeUser(this);
}

void Client::onError(QAbstractSocket::SocketError socketError) const
{
    qDebug()<<socketError;
    /*
    switch (socketError) {
    case QAbstractSocket::RemoteHostClosedError:
        break;
    case QAbstractSocket::HostNotFoundError:


    case QAbstractSocket::ConnectionRefusedError:

    }
    */

}

void Client::onReadyRead()
{
    QDataStream in(_socket);
    qint64 msgSize = -1;

    if (_socket->bytesAvailable() && msgSize == -1) {
        in >> msgSize;
    }

    while (_socket->bytesAvailable() < msgSize - sizeof(qint64)) {
        if (!_socket->waitForReadyRead(100)) {
            _socket->disconnectFromHost();
            break;
        }
    }

    QString str;
    in >> str;
    qDebug()<<str;
}
