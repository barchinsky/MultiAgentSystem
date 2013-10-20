#ifndef CLIENT_H
#define CLIENT_H

#include <QObject>
#include <QtNetwork/qtcpsocket.h>
#include <qpoint.h>

class Server;

class Client : public QObject
{    
    friend class Server;
    Q_OBJECT
private:
    Server *_server;
    QPointF _position;
    QTcpSocket *_socket;
    QString _name;    
    qint32 _clientID;

public:    
    explicit Client(int desc, Server *serv, QObject *parent = 0);

    qint32 getID();
    void sendData(QByteArray);

signals:
    void removeUser(Client *);

public slots:
    void onConnect();
    void onDisconnect();
    void onError(QAbstractSocket::SocketError socketError) const;
    void onReadyRead();

};

#endif // CLIENT_H
