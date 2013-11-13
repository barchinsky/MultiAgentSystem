#include <qdatastream.h>

#include "client.h"
#include "server.h"
#include "ant.h"

#include <QPolygonF>
#include <QPointF>



Client::Client(int handle, Server *serv, QObject *parent):
    QObject(parent)
{
    _server = serv;
    _clientID = handle;
    _position = QPointF(0,0);
    _direction = QPointF(0,1);

    _ant = new Ant(_position,0.025);
    connect(this, SIGNAL(antPositionChanged(QPointF,QPointF)),_ant,SLOT(onPositionChanged(QPointF,QPointF)));

    _socket = new QTcpSocket(this);
    _socket->setSocketDescriptor(handle);

    connect(_socket, SIGNAL(connected()), this, SLOT(onConnect()));
    connect(_socket, SIGNAL(disconnected()), this, SLOT(onDisconnect()));
    connect(_socket, SIGNAL(readyRead()), this, SLOT(onReadyRead()));
    connect(_socket, SIGNAL(error(QAbstractSocket::SocketError)), this, SLOT(onError(QAbstractSocket::SocketError)));

    qDebug() << "Client connected " << handle;
}

Client::~Client()
{
    delete[] _socket;
    delete[] _ant;
}

qint32 Client::getID()
{
    return _clientID;
}

QPointF Client::getPosition()
{
    return _position;
}

QPointF Client::getDircetion()
{
    return _direction;
}

QPolygonF Client::getAntShape()
{
    return _ant->getShape();
}

void Client::setPositionAndDirection(QPointF newPosition, QPointF newDirection)
{
    _position = newPosition;
    _direction = newDirection;
    qDebug() << "position changed in client";
    emit antPositionChanged(_position,_direction);
}

void Client::sendData(QByteArray dataArray)
{
//    qDebug() << "Send to" << _clientID << "data:" << QString(dataArray);
    _socket->write(dataArray);
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
    if (_socket->bytesAvailable() == 0)
        return;

    QByteArray data = _socket->readAll();
    QString str = QString(data);
    _server->parseDataFromClient(this,data);
}
