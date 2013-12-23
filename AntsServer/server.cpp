#include "server.h"
#include <qdebug.h>
#include <QtCore/qmath.h>

#include <qjsonarray.h>
#include <qjsondocument.h>
#include <qjsonobject.h>
#include <qjsonvalue.h>

#include <QPointF>
#include <QSemaphore>

#include "client.h"
#include "constants.h"
#include "map.h"

#include "helpmath.h"


Server::Server(Map *map, QObject *parent) :
    QTcpServer(parent)
{
    _map = map;
}

// private

QByteArray Server::parseJSONwithKeyAndObject(Client *ant, QString key, QJsonObject obj)
{
    if (key == kAPI_register) {
        return registerClient(ant,obj);
    } else if (key == kAPI_is_ant_can_move) {
        return isAntCanMove(ant,obj);
    } else if (key == kAPI_nearest_objects) {
        return getNearestObjects(ant);
    } else if (key == kAPI_got_food) {
        return isGotFood(ant);
    } else if (key == kAPI_put_food) {
        return isPutFood(ant);
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

QByteArray Server::registerClient(Client *ant, QJsonObject jsonObject)
{
    // in JSON
    if (jsonObject.contains(kJSON_SOCKET)) {
        QJsonValue socketValue = jsonObject.value(kJSON_SOCKET);
        if (socketValue.isString()) {
            ant->_socketForConnection = socketValue.toString();
        }
    }

    // out JSON
    QJsonObject coordJSON;
    coordJSON.insert(kJSON_API_KEY,QJsonValue(kAPI_register));

    QJsonArray coordArray;        
    QPointF position = _map->antBornPoint();
    ant->setPositionAndDirection(position,QPointF(0,1));

    coordArray.push_back(QJsonValue(position.x()));
    coordArray.push_back(QJsonValue(position.y()));

    QJsonObject coord;
    coord.insert(kJSON_COORD_ANT,QJsonValue(coordArray));

    QPointF baseCoords = Map::baseCoord();

    double x = baseCoords.x();
    double y = baseCoords.y();
    coordArray[0] = QJsonValue(x);
    coordArray[1] = QJsonValue(y);
    coord.insert(kJSON_COORD_BASE,QJsonValue(coordArray));

    double stepLength = _map->getAntStep();
    coord.insert(kJSON_STEP_LENGTH,QJsonValue(stepLength));

    coordJSON.insert(kJSON_OBJECT,QJsonValue(coord));
    QJsonDocument json(coordJSON);

    return json.toJson();
}

QByteArray Server::isAntCanMove(Client *ant, QJsonObject vectorObject)
{
    QJsonValue vectorValue = vectorObject.value(kJSON_VECTOR);
    QJsonValue nextCoordValue = vectorObject.value(kJSON_NEXT_COORD);

    bool isCanMove = false;
    float x,y;
    QPointF direction;

    if (getCoords(&x,&y,nextCoordValue)) {
        float antX = ant->getPosition().x();
        float antY = ant->getPosition().y();

        if (isRightDirection(antX,antY,x,y)) {
            isCanMove = true;
            direction = Math::directionFromPointToPoint(antX,antY,x,y);
//            qDebug() << "ant position " << antX<<"\t"<<antY<<'\n';
//            qDebug() << "next position " << x<<"\t"<<y<<'\n';
//            qDebug() << "direction " << direction.x()<<'\t'<<direction.y()<<'\n';
        }

    } else if (getCoords(&x,&y,vectorValue)) {
        if (isRightDirection(x,y)) {
            isCanMove = true;
            direction = QPointF(x,y);
        }
    }

//    qDebug() << "isCanMove " << isCanMove;

    if (isCanMove) {
        ant->_direction = direction;
        bool isStucked = false;
        QPointF newPosition = _map->nextPositionForAnt(ant,&isStucked);

//        qDebug() << "isStucked " << isStucked;

        ant->setPositionAndDirection(newPosition,direction);

        // create JSON
        QJsonObject resultJSON;
        resultJSON.insert(kJSON_API_KEY,QJsonValue(kAPI_is_ant_can_move));

        QJsonObject ant_coord;

        if (!isStucked) {
            QJsonArray coordArray;
            coordArray.push_back(QJsonValue(newPosition.x()));
            coordArray.push_back(QJsonValue(newPosition.y()));
            ant_coord.insert(kJSON_COORD_ANT,QJsonValue(coordArray));
        }

        resultJSON.insert(kJSON_OBJECT,QJsonValue(ant_coord));

        QJsonDocument json(resultJSON);

        return json.toJson();
    }

    // if something wrong return error
    return getErrorJSONData();
}

QByteArray Server::isGotFood(Client *ant)
{
    QJsonObject JsonObject;
    JsonObject.insert(kJSON_API_KEY,QJsonValue(kAPI_got_food));

    QJsonObject isGot;
    isGot.insert(kJSON_GOT_FOOD,QJsonValue(1));

    JsonObject.insert(kJSON_OBJECT,QJsonValue(isGot));
    QJsonDocument json(JsonObject);

    qDebug() << "Food GOT";

    bool isGotValue = _map->antTryCutTheFood(ant);

    ant->setWithFood(isGotValue);

    return json.toJson();
}

QByteArray Server::isPutFood(Client *ant)
{
    QJsonObject JsonObject;
    JsonObject.insert(kJSON_API_KEY,QJsonValue(kAPI_put_food));

    QJsonObject isPut;
    isPut.insert(kJSON_PUT_FOOD,QJsonValue(1));

    JsonObject.insert(kJSON_OBJECT,QJsonValue(isPut));
    QJsonDocument json(JsonObject);

    ant->setWithFood(false);
    return json.toJson();
}

bool Server::getCoords(float *x, float *y, QJsonValue coordArray)
{
    if (coordArray.isArray()) {
        QJsonArray vectorArray = coordArray.toArray();

        if (vectorArray.size() == 2) {
            QJsonValue coordValueX = vectorArray.at(0);
            QJsonValue coordValueY = vectorArray.at(1);

            if (coordValueX.isDouble() && coordValueY.isDouble()) {
                *x = (float)coordValueX.toDouble();
                *y = (float)coordValueY.toDouble();
                return true;
            }
        }
    }
    return false;
}

bool Server::isRightDirection(float x, float y)
{
    if (Math::length(x,y) > 1.0)
        return false;
    return true;
}

bool Server::isRightDirection(float x0, float y0, float x1, float y1)
{
    if (Math::length(x0,y0,x1,y1) > _map->getAntStep())
        return false;
    return true;
}

QByteArray Server::getNearestObjects(Client *ant)
{
    QJsonObject rootObject;
    rootObject.insert(kJSON_API_KEY,QJsonValue(kAPI_nearest_objects));

    QJsonObject mainObject;

    NearObjects nearObjects = _map->nearObjectsForAnt(ant);

    QJsonArray antsArray;
    foreach (Client *client, nearObjects.ants) {
        QJsonObject antObject;

        QPointF antPosition = client->getPosition();
        QJsonArray coordsValue;
        coordsValue.push_back(QJsonValue(antPosition.x()));
        coordsValue.push_back(QJsonValue(antPosition.y()));
        antObject.insert(kJSON_COORD,QJsonArray(coordsValue));

        if (!client->_socketForConnection.isEmpty()) {
            antObject.insert(kJSON_SOCKET,QJsonValue(client->_socketForConnection));
        }

        antsArray.push_back(QJsonValue(antObject));
    }
    mainObject.insert(kJSON_ANTS,QJsonValue(antsArray));


    bool isExistFood = nearObjects.food.count() != 0;
    mainObject.insert(kJSON_FOODS,QJsonValue(isExistFood?1:0));

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
    TRACE("parse Data from client %d", client->getID());
    qDebug() << str;

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
    TRACE("send Data To client %d", client->getID());
    qDebug()<< QString(byteArray);
    client->sendData(byteArray);
}

void Server::incomingConnection(qintptr handle)
{
    Client *client = new Client(handle, this, this);
    TRACE("Connected client %d", client->getID());

    connect(client, SIGNAL(removeUser(Client*)), this, SLOT(onRemoveUser(Client*)));

    _clients.append(client);
    emit onClientsCountChanged(_clients.count());
}


// slots

void Server::onRemoveUser(Client *client)
{
    TRACE("Disconnected client %d", client->getID());
    _clients.removeAt(_clients.indexOf(client));
    emit onClientsCountChanged(_clients.count());
    emit onClientDiedAtPoint(client->getPosition());
}


