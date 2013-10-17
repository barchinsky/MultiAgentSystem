#ifndef SERVER_H
#define SERVER_H

#include <QtNetwork/qtcpserver.h>
#include <QtNetwork/qtcpsocket.h>

class Client;

class Server : public QTcpServer
{
    Q_OBJECT
private:
    QTcpSocket *_sok;
    QList<Client *> _clients;
protected:
    void incomingConnection(qintptr handle);

public:
    explicit Server(QObject *parent = 0);
    bool doStartServer(QHostAddress addr, qint16 port);

signals:

public slots:
    void onRemoveUser(Client*);
};

#endif // SERVER_H
