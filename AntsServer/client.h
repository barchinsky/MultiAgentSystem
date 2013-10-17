#ifndef CLIENT_H
#define CLIENT_H

#include <QObject>
#include <QtNetwork/qtcpsocket.h>

class Server;

class Client : public QObject
{    
    Q_OBJECT
private:
    Server *_server;
    QPoint *_position;
    QTcpSocket *_socket;
    QString _name;

public:    
    explicit Client(int desc, Server *serv, QObject *parent = 0);

signals:
    void removeUser(Client *);

public slots:
    void onConnect();
    void onDisconnect();
    void onError(QAbstractSocket::SocketError socketError) const;
    void onReadyRead();

};

#endif // CLIENT_H
