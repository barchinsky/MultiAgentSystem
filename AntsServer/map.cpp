#include <qdebug.h>
#include <QtCore/qmath.h>


#include "map.h"
#include "ui_map.h"
#include "client.h"
#include "constants.h"
#include "clover.h"
#include "ant.h"
#include "helpmath.h"


Map::Map(QWidget *parent) :
    QGLWidget(parent),
    ui(new Ui::Map)
{
    ui->setupUi(this);
    _serv = new Server(this,this);

    if (_serv->doStartServer(QHostAddress(QString("127.0.0.1")),9000)) {
        qDebug()<<"connected";
        connect(_serv,SIGNAL(onClientsCountChanged(int)),this,SLOT(clientsCountChanged(int)));
        connect(_serv,SIGNAL(onClientDiedAtPoint(QPointF)),this,SLOT(antDiedAtPosition(QPointF)));

        _antsCount = 0;
        _antStep = 0.01;
        _antLookingRadius = 0.2;

        setupFood();
        setupBarriers();

        setFormat(QGLFormat(QGL::DoubleBuffer));
        glDepthFunc(GL_LEQUAL);

        QTimer *timer = new QTimer(this);
        connect(timer, SIGNAL(timeout()), this, SLOT(updateGL()));
        timer->start(16);

        QTimer *timer2 = new QTimer(this);
        connect(timer2, SIGNAL(timeout()), this, SLOT(onUpdateAntsFeromons()));
        timer2->start(5000);

    } else {
        qDebug()<<"failed";
    }
}

Map::~Map()
{
    delete ui;
}

// slots

void Map::clientsCountChanged(int clientsCount)
{    
    _antsCount = clientsCount;
}

void Map::antDiedAtPosition(QPointF position)
{
    _diedAntsPositions << position;
}

void Map::onDisappearFeromon(Feromon *feromon)
{    
    _feromons.removeAt(_feromons.indexOf(feromon));    
}

void Map::initializeGL()
{
    qglClearColor(Qt::white); // Черный цвет фона
    glEnable(GL_BLEND);
    glBlendFunc (GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}

void Map::resizeGL(int width, int height)
{
    int side = qMin(width, height);
    glViewport((width - side) / 2, (height - side) / 2, side, side);

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
#ifdef QT_OPENGL_ES_1
    glOrthof(-0.1, 1.1, -0.1, 1.1, 1.0, 0.0);
#else
    glOrtho(-0.1, 1.1, -0.1, 1.1, 1.0, 0.0);
#endif
    glMatrixMode(GL_MODELVIEW);
}

void Map::paintGL()
{
   // TRACE("GL Draw");
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // чистим буфер изображения и буфер глубины
    glLoadIdentity();

    qglColor(Qt::black);

    drawBorder();
    renderText(0.01,1 + 2*_antStep,0.0,QString::fromUtf8("Ants count: %1").arg(_antsCount));
    drawBase();
    drawFeromons();
    drawAnts();
    drawFood();
    drawBarries();
    drawDiedAnts();

    swapBuffers();
}

// static methods

QPointF Map::baseCoord()
{
    return QPointF(0.15,0.15);
}

float Map::antScaleFactor()
{
    return 0.01;
}

// public methods

float Map::getAntStep()
{
    return _antStep;
}

QPointF Map::antBornPoint()
{
    float x = baseCoord().x();
    float y = baseCoord().y();

    QPointF bornPoint(x,y);
    return bornPoint;
}

QPointF Map::nextPositionForAnt(Client *ant,bool *isStucked)
{
    QPointF nextPosition = ant->getPosition() + ant->getDircetion() * _antStep;
    QPointF fewAntSteps = ant->getPosition() + ant->getDircetion() * _antStep*2;
    *isStucked = true;

    QPolygonF mapBorder;
    mapBorder << QPointF(0.0,0.0) << QPointF(1.0,0.0) << QPointF(1.0,1.0) << QPointF(0.0,1.0);
    if (!mapBorder.containsPoint(nextPosition,Qt::OddEvenFill)) {
        qDebug()<<"containsPoint in mapBorder";
        return ant->getPosition();
    }

    foreach (QPolygonF barrier, _barrierPostions) {
        if (barrier.containsPoint(fewAntSteps,Qt::OddEvenFill)) {
            qDebug()<<"containsPoint in barrier";
            return ant->getPosition();
        }
    }

    foreach (QPolygonF food, _foodPositions) {
        if (food.containsPoint(nextPosition,Qt::OddEvenFill)) {
            qDebug()<<"containsPoint in food";
            return ant->getPosition();
        }
    }



    *isStucked = false;
    return nextPosition;
}

NearObjects Map::nearObjectsForAnt(Client *ant)
{   
    NearObjects objects;

    int sectorsCount = 6;
    float radianStep = 2*M_PI / (float)sectorsCount;

    float x0,y0,x,y;
    x0 = ant->getPosition().x();
    y0 = ant->getPosition().y();

    QPolygonF antPolygon;

    for (int i = 0; i < sectorsCount; i++) {
        float rad = i * radianStep;
        x = x0 + _antLookingRadius * cos(rad);
        y = y0 + _antLookingRadius * sin(rad);
        antPolygon << QPointF(x,y);
    }

    objects.ants = antsNearAntPolygon(ant,antPolygon);
    objects.barriers = barriersNearAntPolygon(antPolygon);

    QPointF antsMouthPosition = ant->getAntMouthPosition();

    objects.food = foodPositionsNearAntPolygon(antsMouthPosition);

    return objects;
}

QVector<QPolygonF> Map::barriersNearAntPolygon(QPolygonF &antPolygon)
{
    QVector<QPolygonF> nearBarriers;

    foreach (QPolygonF barrier, _barrierPostions) {
        if (!(barrier.intersected(antPolygon)).isEmpty()) {
            nearBarriers << barrier;
        }
    }

    return nearBarriers;
}

QPolygonF Map::foodPositionsNearAntPolygon(QPointF &checkPoint)
{
    foreach (QPolygonF foodPol, _foodPositions) {
        if (foodPol.containsPoint(checkPoint,Qt::OddEvenFill)) {
            return foodPol;
        }
    }

    return QPolygonF(0);
}

QVector<Client *> Map::antsNearAntPolygon(Client *inputAnt, QPolygonF &antPolygon)
{
    QVector<Client *> nearAnts;

    if (inputAnt->isCanPutFeromon()) {
        foreach (Client *ant, _serv->_clients) {
            if (inputAnt != ant && ant->isCanPutFeromon()) {
                if (antPolygon.containsPoint(ant->getPosition(),Qt::OddEvenFill)) {
                    nearAnts << ant;
                    break;
                }
            }
        }
    }

    return nearAnts;
}

bool Map::antTryCutTheFood(Client *ant)
{
    QPointF mouthPosition = ant->getAntMouthPosition();

    QPolygonF foodForEating = foodPositionsNearAntPolygon(mouthPosition);

    if (foodForEating.count() == 0) {
        return false;
    } else {
        QPointF direction = ant->getDircetion();

        float rad_step = M_PI * 2.0 / 3.0;
        float start_rad = qAcos(direction.x());
        float cutRadius = 2 * antScaleFactor();

        QPolygonF cutRegion;
        for (int i = 0; i<3; i++) {
            float x = mouthPosition.x() + cutRadius * qCos(start_rad + rad_step*i);
            float y = mouthPosition.y() + cutRadius * qSin(start_rad + rad_step*i);
            cutRegion << QPointF(x,y);
        }

        QList<QPolygonF> cutPolygons = Math::dividePolygonByPolygon(foodForEating,cutRegion);

        int index = _foodPositions.indexOf(foodForEating);
        _foodPositions.removeAt(index);

        foreach (QPolygonF newPol, cutPolygons) {
            _foodPositions.append(newPol);
        }

        return true;
    }

    return false;
}

// private methods

void Map::setupFood()
{
    Point cloverCenter;
    cloverCenter.x = 0.78;
    cloverCenter.y = 0.78;
    QPolygonF baseFood  = polygonForClover(cloverCenter,0.2,30.0);
    _foodPositions << baseFood;
}

void Map::setupBarriers()
{
    /*
    QPolygonF barrier1;
    barrier1 << QPointF(0.15,0.52);
    barrier1 << QPointF(0.43,0.48);
    barrier1 << QPointF(0.40,0.31);
    barrier1 << QPointF(0.30,0.21);

    QPolygonF barrier2;
    barrier2 << QPointF(0.50,0.38);
    barrier2 << QPointF(0.52,0.24);
    barrier2 << QPointF(0.45,0.21);
    barrier2 << QPointF(0.43,0.30);

    QPolygonF barrier3;
    barrier3 << QPointF(0.61,0.29);
    barrier3 << QPointF(0.60,0.55);
    barrier3 << QPointF(0.75,0.61);
    barrier3 << QPointF(0.70,0.35);
    barrier3 << QPointF(0.73,0.36);
    barrier3 << QPointF(0.69,0.15);

    QPolygonF barrier4;
    barrier4 << QPointF(0.32,0.59);
    barrier4 << QPointF(0.10,0.75);
    barrier4 << QPointF(0.29,0.81);
    barrier4 << QPointF(0.40,0.78);
    barrier4 << QPointF(0.42,0.61);

    QPolygonF barrier5;
    barrier5 << QPointF(0.50,0.15);
    barrier5 << QPointF(0.60,0.10);
    barrier5 << QPointF(0.40,0.09);
    barrier5 << QPointF(0.45,0.14);

    _barrierPostions.append(barrier1);        
    _barrierPostions.append(barrier2);    
    _barrierPostions.append(barrier3);    
    _barrierPostions.append(barrier4);    
    _barrierPostions.append(barrier5);
    */

    QPolygonF testBarrier;
     // Simple TEST
    testBarrier << QPointF(0.3,0.1);
    testBarrier << QPointF(0.4,0.2);
    testBarrier << QPointF(0.2,0.4);
    testBarrier << QPointF(0.1,0.3);

/*
    // hard test
    testBarrier << QPointF(0.3,0.1);
    testBarrier << QPointF(0.5,0.1);
    testBarrier << QPointF(0.5,0.3);
    testBarrier << QPointF(0.4,0.35);
    testBarrier << QPointF(0.6,0.35);
    testBarrier << QPointF(0.2,0.6);
    testBarrier << QPointF(0.2,0.4);
    testBarrier << QPointF(0.05,0.5);
*/
//    _barrierPostions.append(testBarrier);
}

void Map::onUpdateAntsFeromons()
{
    QList<Client *> ants = _serv->_clients;

    foreach (Client *client, ants) {        
        if (client->isCanPutFeromon()) {
            Feromon *newFeromon = new Feromon(client->getPosition());
            connect(newFeromon,SIGNAL(disappearFeromon(Feromon*)),this,SLOT(onDisappearFeromon(Feromon*)));
            _feromons.append(newFeromon);
        }
    }
}

void Map::drawBorder()
{
    qglColor(Qt::black);

    float offset = antScaleFactor();

    glBegin(GL_LINE_LOOP);
    glVertex2f(-offset,1+offset);
    glVertex2f(1+offset,1+offset);
    glVertex2f(1+offset,-offset);
    glVertex2f(-offset,-offset);
    glEnd();
}

void Map::drawBase()
{
    qglColor(QColor(121,79,66));

    QPointF baseCoords = baseCoord();

    float x = baseCoords.x();
    float y = baseCoords.y();
    float size = 0.01;

    glBegin(GL_QUADS);
    glVertex2f(x - size, y + size);
    glVertex2f(x + size, y + size);
    glVertex2f(x + size, y - size);
    glVertex2f(x - size, y - size);
    glEnd();
}


void Map::drawAnts()
{    
    QList<Client *> ants = _serv->_clients;

    foreach (Client *ant, ants) {
        qglColor(Qt::black);
        glBegin(GL_LINE_LOOP);
        {
            foreach (QPointF point, ant->getAntShape()) {
                glVertex2f(point.x(),point.y());
            }
        }
        glEnd();

        if (ant->isWithFood()) {
            glBegin(GL_TRIANGLES);
            qglColor(QColor(0,255,0,230)/*QColor(176,214,0,200)*/);


            foreach (QPointF point, ant->_ant->getLeaflet()) {
                glVertex2f(point.x(),point.y());
            }

            glEnd();
        }

        // test draw mouth position
//        glBegin(GL_POINTS);
//        QPointF mouth = ant->getAntMouthPosition();
//        glVertex2f(mouth.x(),mouth.y());
//        glEnd();
    }
}


void Map::drawFood()
{
    qglColor(Qt::green);
    foreach (QPolygonF foodPol, _foodPositions) {
        glBegin(GL_LINE_LOOP);
        {
            foreach (QPointF foodPoint, foodPol) {
                glVertex2f(foodPoint.x(),foodPoint.y());
            }
        }
        glEnd();
    }
}

void Map::drawBarries()
{
    qglColor(Qt::black);

    foreach (QPolygonF barrier, _barrierPostions) {
        glBegin(GL_LINE_LOOP);
        {
            foreach (QPointF point, barrier) {
                glVertex2f(point.x(),point.y());
            }
            glEnd();
        }
    }
}

void Map::drawDiedAnts()
{
    qglColor(Qt::red);

    glBegin(GL_LINES);
    foreach (QPointF pos, _diedAntsPositions) {
        float offset = 0.5 * antScaleFactor();
        float x0 = pos.x();
        float y0 = pos.y();
        glVertex2f(x0 - offset,y0 - offset);
        glVertex2f(x0 + offset,y0 + offset);

        glVertex2f(x0 + offset,y0 - offset);
        glVertex2f(x0 - offset,y0 + offset);
    }
    glEnd();
}

void Map::drawFeromons()
{
    glPointSize(5);
    glBegin(GL_POINTS);

    foreach (Feromon *feromon, _feromons) {
        float alpha = 255.0 * feromon->getAlpha();
        QPointF position = feromon->getPosition();
        qglColor(QColor(145,122,0,alpha));
        glVertex2f(position.x(),position.y());
    }
    glEnd();
}



