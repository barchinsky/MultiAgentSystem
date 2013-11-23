#ifndef SERVER_H
#define SERVER_H

#include <QtNetwork/qtcpserver.h>
#include <QtNetwork/qtcpsocket.h>
#include <QPointF>

class Client;
class QJsonObject;
class QJsonValue;
class Map;


class Server : public QTcpServer
{
    friend class Map;

    Q_OBJECT
private:
    QTcpSocket *_sok;
    QList<Client *> _clients;
    Map *_map;
    QByteArray array;

    QByteArray parseJSONwithKeyAndObject(Client *ant,QString, QJsonObject);
    QByteArray getErrorJSONData();
    QByteArray registerClient(Client *ant, QJsonObject jsonObject);
    QByteArray isAntCanMove(Client *ant, QJsonObject vector);
    QByteArray isGotFood(Client *ant);
    QByteArray isPutFood(Client *ant);

    bool getCoords(float *x, float *y, QJsonValue coordArray);
    bool isRightDirection(float x, float y);
    bool isRightDirection(float x0, float y0, float x1, float y1);

    QByteArray getNearestObjects(Client *ant);

protected:
    void incomingConnection(qintptr handle);

public:
    explicit Server( Map *map, QObject *parent = 0);
    bool doStartServer(QHostAddress addr, qint16 port);

    void parseDataFromClient(Client *,QByteArray);
    void sendDataToClient(Client *, QByteArray);

signals:
    void onClientsCountChanged(int);
    void onClientDiedAtPoint(QPointF);

public slots:
    void onRemoveUser(Client*);
};

#endif // SERVER_H
