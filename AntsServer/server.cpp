#include "server.h"
#include <qdebug.h>
#include <QtCore/qmath.h>

#include <qjsonarray.h>
#include <qjsondocument.h>
#include <qjsonobject.h>
#include <qjsonvalue.h>

#include <qrect.h>

#include "client.h"
#include "constants.h"
#include "map.h"

Server::Server(Map *map, QObject *parent) :
    QTcpServer(parent)
{
    _map = map;
}

// private

QByteArray Server::parseJSONwithKeyAndObject(Client *ant, QString key, QJsonObject obj)
{
    if (key == kAPI_register) {
        return registerClient(ant);
    } else if (key == kAPI_is_ant_can_move) {
        return isAntCanMove(ant,obj);
    } else if (key == kAPI_nearest_objects) {
        return getNearestObjects(ant);
    }

    return getErrorJSONData();
}

QByteArray Server::getErrorJSONData()
{
    QJsonObject errorJSON;
    errorJSON.insert(kJSON_API_KEY,QJsonValue(kAPI_error));

    QJsonObject msg;
    msg.insert(kJSON_MESSAGE,QJsonValue(QString("Unexpected Error")));

    errorJSON.insert(kJSON_OBJECT,QJsonValue(msg));
    QJsonDocument json(errorJSON);

    return json.toJson();
}

QByteArray Server::registerClient(Client *ant)
{
    QJsonObject coordJSON;
    coordJSON.insert(kJSON_API_KEY,QJsonValue(kAPI_register));

    QJsonArray coordArray;    
    ant->_position = _map->antBornPoint();

    coordArray.push_back(QJsonValue(ant->_position.x()));
    coordArray.push_back(QJsonValue(ant->_position.y()));

    QJsonObject coord;
    coord.insert(kJSON_COORD_ANT,QJsonValue(coordArray));

    QPointF baseCoords = Map::baseCoord();

    double x = baseCoords.x();
    double y = baseCoords.y();
    coordArray[0] = QJsonValue(x);
    coordArray[1] = QJsonValue(y);
    coord.insert(kJSON_COORD_BASE,QJsonValue(coordArray));

    coordJSON.insert(kJSON_OBJECT,QJsonValue(coord));
    QJsonDocument json(coordJSON);

    return json.toJson();
}

QByteArray Server::isAntCanMove(Client *ant, QJsonObject vectorObject)
{
    QJsonValue vectorValue = vectorObject.value(kJSON_VECTOR);
    if (vectorValue.isArray()) {
        QJsonArray vectorArray = vectorValue.toArray();

        if (vectorArray.size() == 2) {
            QJsonValue coordValueX = vectorArray.at(0);
            QJsonValue coordValueY = vectorArray.at(1);


            if (coordValueX.isDouble() && coordValueY.isDouble()) {
                double x,y;
                x = coordValueX.toDouble();
                y = coordValueY.toDouble();

                double vectorLength = qSqrt(qPow(x,2) + qPow(y,2));


                if (vectorLength <= 1.0) {
                    //QPointF antPosition = ant->_position;
                    QPointF vector(x,y);
                    ant->_direction = vector;

                    ant->_position = _map->nextPositionForAnt(ant);
                    /*
                    QPointF newAntPosition = antPosition + vector*Map::antStep();

                    QRect mapRect(0,0,100,100);
                    QPoint intPoint((int)newAntPosition.x()*100,(int)newAntPosition.y()*100);
                    if (!mapRect.contains(intPoint)) {
                        newAntPosition = antPosition;
                    } else {
                        ant->_position = newAntPosition;
                        ant->_direction = vector;
                    }
                    */

                    // create JSON
                    QJsonObject resultJSON;
                    resultJSON.insert(kJSON_API_KEY,QJsonValue(kAPI_is_ant_can_move));

                    QJsonArray coordArray;
                    coordArray.push_back(QJsonValue(ant->_position.x()));
                    coordArray.push_back(QJsonValue(ant->_position.y()));

                    QJsonObject ant_coord;
                    ant_coord.insert(kJSON_COORD_ANT,QJsonValue(coordArray));

                    resultJSON.insert(kJSON_OBJECT,QJsonValue(ant_coord));
                    QJsonDocument json(resultJSON);

                    return json.toJson();
                }
            }
        }
    }

    // if something wrong return error
    return getErrorJSONData();
}

QByteArray Server::getNearestObjects(Client *ant)
{
    QJsonObject rootObject;
    rootObject.insert(kJSON_API_KEY,QJsonValue(kAPI_nearest_objects));

    QJsonObject mainObject;

    NearObjects nearObjects = _map->nearObjectsForAnt(ant);
    qDebug()<<"ants "<<nearObjects.ants;
    qDebug()<<"foods "<<nearObjects.foods;
    qDebug()<<"barriers "<<nearObjects.barriers;


    QJsonArray antsArray;
    foreach (Client *client, nearObjects.ants) {
        QJsonObject antObject;

        QPointF antPosition = client->getPosition();
        QJsonArray coordsValue;
        coordsValue.push_back(QJsonValue(antPosition.x()));
        coordsValue.push_back(QJsonValue(antPosition.y()));
        antObject.insert(kJSON_COORD,QJsonArray(coordsValue));

        QString socketStr;
        socketStr += client->_socket->peerAddress().toString();
        socketStr += ":" + QString::number(client->_socket->peerPort());
        antObject.insert(kJSON_SOCKET,QJsonValue(socketStr));

        antsArray.push_back(QJsonValue(antObject));
    }
    mainObject.insert(kJSON_ANTS,QJsonValue(antsArray));


    QJsonArray foodsArray;
    foreach (QPointF foodPoint, nearObjects.foods) {
        QJsonArray coordsValue;
        coordsValue.push_back(QJsonValue(foodPoint.x()));
        coordsValue.push_back(QJsonValue(foodPoint.y()));
        foodsArray.push_back(QJsonValue(coordsValue));
    }
    mainObject.insert(kJSON_FOODS,QJsonValue(foodsArray));

    QJsonArray barriersArray;
    foreach (QPolygonF polygon, nearObjects.barriers) {
        QJsonArray coordsArray;
        foreach (QPointF point, polygon) {
            QJsonArray coordsValue;
            coordsValue.push_back(QJsonValue(point.x()));
            coordsValue.push_back(QJsonValue(point.y()));
            coordsArray.push_back(QJsonValue(coordsValue));
        }
        barriersArray.push_back(QJsonValue(coordsArray));
    }
    mainObject.insert(kJSON_BARRIERS,QJsonValue(barriersArray));

    rootObject.insert(kJSON_OBJECT,QJsonValue(mainObject));

    QJsonDocument json(rootObject);
    return json.toJson();
}

// public

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

void Server::parseDataFromClient(Client *client, QByteArray byteArray)
{
    QString str(byteArray);
//    qDebug()<<"Client: "<<client->getID()<<"data: "<<str;

    QJsonDocument d = QJsonDocument::fromJson(str.toUtf8());

    QString apiKey;
    QJsonObject object;

    if (!d.isNull() && d.isObject()) {
        QJsonObject jsonObject = d.object();
        QJsonValue value = jsonObject.value(kJSON_API_KEY);

        if (value.isString()) {
            apiKey = value.toString();
        }

        value = jsonObject.value(kJSON_OBJECT);
        if (value.isObject()) {
            object = value.toObject();
        }
    }

    if (!apiKey.isEmpty()) {
        sendDataToClient(client,parseJSONwithKeyAndObject(client,apiKey,object));
    } else {
        sendDataToClient(client,getErrorJSONData());
    }
}

void Server::sendDataToClient(Client *client, QByteArray byteArray)
{
    client->sendData(byteArray);
}

void Server::incomingConnection(qintptr handle)
{
    Client *client = new Client(handle, this, this);

    qDebug() << "Local: \n";
    qDebug() << client->_socket->localAddress().toString();
    qDebug() << client->_socket->localPort();

    qDebug() << "Peer: \n";
    qDebug() << client->_socket->peerAddress().toString();
    qDebug() << client->_socket->peerPort();

    connect(client, SIGNAL(removeUser(Client*)), this, SLOT(onRemoveUser(Client*)));

    _clients.append(client);
    emit onClientsCountChanged(_clients.count());
}


// slots

void Server::onRemoveUser(Client *client)
{
    _clients.removeAt(_clients.indexOf(client));
    emit onClientsCountChanged(_clients.count());
}


