#ifndef CLIENT_H
#define CLIENT_H

#include <QObject>
#include <QtNetwork/qtcpsocket.h>
#include <qpoint.h>

class Server;
class Ant;

class Client : public QObject
{    
    friend class Server;
    Q_OBJECT
private:
    Server *_server;
    Ant *_ant;
    QPointF _position;
    QPointF _direction;
    QTcpSocket *_socket;
    QString _name;    
    qint32 _clientID;    

public:    
    QString _socketForConnection;

    explicit Client(int desc, Server *serv, QObject *parent = 0);
    ~Client();

    qint32 getID();
    QPointF getPosition();
    QPointF getDircetion();
    QPolygonF getAntShape();
    QPointF getAntMouthPosition();
    void setPositionAndDirection(QPointF newPosition, QPointF newDirection);
    void sendData(QByteArray);

signals:
    void removeUser(Client *);    
    void antPositionChanged(QPointF position, QPointF vector);

public slots:
    void onConnect();
    void onDisconnect();
    void onError(QAbstractSocket::SocketError socketError) const;
    void onReadyRead();

};

#endif // CLIENT_H
