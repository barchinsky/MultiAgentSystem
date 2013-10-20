#ifndef SERVER_H
#define SERVER_H

#include <QtNetwork/qtcpserver.h>
#include <QtNetwork/qtcpsocket.h>

class Client;
class QJsonObject;

class Server : public QTcpServer
{
    Q_OBJECT
private:
    QTcpSocket *_sok;
    QList<Client *> _clients;
    QByteArray array;

    QByteArray parseJSONwithKeyAndObject(Client *ant,QString, QJsonObject);
    QByteArray getErrorJSONData();
    QByteArray registerClient(Client *ant);
    QByteArray isAntCanMove(Client *ant, QJsonObject vector);

protected:
    void incomingConnection(qintptr handle);

public:
    explicit Server(QObject *parent = 0);
    bool doStartServer(QHostAddress addr, qint16 port);

    void parseDataFromClient(Client *,QByteArray);
    void sendDataToClient(Client *, QByteArray);

signals:         

public slots:
    void onRemoveUser(Client*);
};

#endif // SERVER_H
