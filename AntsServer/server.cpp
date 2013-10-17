#include "server.h"
#include <qdebug.h>

#include "client.h"

Server::Server(QObject *parent) :
    QTcpServer(parent)
{    
}


bool Server::doStartServer(QHostAddress addr, qint16 port)
{
    if (!listen(addr, port))
    {
        qDebug() << "Server not started at" << addr << ":" << port;
        return false;
    }
    qDebug() << "Server started at" << addr << ":" << port;
    return true;
}

void Server::incomingConnection(qintptr handle)
{
    qDebug()<<handle;

    Client *client = new Client(handle, this, this);

    connect(client, SIGNAL(removeUser(Client*)), this, SLOT(onRemoveUser(Client*)));

    _clients.append(client);
}


// slots

void Server::onRemoveUser(Client *client)
{
    _clients.removeAt(_clients.indexOf(client));
}


