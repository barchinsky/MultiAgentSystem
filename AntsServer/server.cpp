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


Server::Server(QObject *parent) :
    QTcpServer(parent)
{    
}

// private

QByteArray Server::parseJSONwithKeyAndObject(Client *ant, QString key, QJsonObject obj)
{
    if (key == kAPI_register) {
        return registerClient(ant);
    } else if (key == kAPI_is_ant_can_move) {
        return isAntCanMove(ant,obj);
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
    double x,y;
    x = ((double)(rand()%200 - 100)) / 100.0;
    y = ((double)(rand()%200 - 100) / 100.0);
    ant->_position = QPointF(x,y);


    coordArray.push_back(QJsonValue(x));
    coordArray.push_back(QJsonValue(y));

    QJsonObject coord;
    coord.insert(kJSON_COORD_ANT,QJsonValue(coordArray));

    x = -0.7;
    y = -0.7;
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
                qDebug()<<x<<" "<<y<<" = "<<vectorLength;

                if (vectorLength <= 1.0) {
                    QPointF antPosition = ant->_position;
                    QPointF vector(x,y);
                    QPointF newAntPosition = antPosition + vector*0.01;

                    QRect mapRect(-100,-100,200,200);
                    QPoint intPoint((int)newAntPosition.x()*100,(int)newAntPosition.y()*100);
                    if (!mapRect.contains(intPoint)) {
                        newAntPosition = antPosition;
                    } else {
                        ant->_position =newAntPosition;
                    }

                    // create JSON
                    QJsonObject resultJSON;
                    resultJSON.insert(kJSON_API_KEY,QJsonValue(kAPI_is_ant_can_move));

                    QJsonArray coordArray;
                    coordArray.push_back(QJsonValue(newAntPosition.x()));
                    coordArray.push_back(QJsonValue(newAntPosition.y()));

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
    qDebug()<<"Client: "<<client->getID()<<"data: "<<str;

    QJsonDocument d = QJsonDocument::fromJson(str.toUtf8());
    qWarning() << d;

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


