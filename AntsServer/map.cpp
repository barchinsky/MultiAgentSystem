#include <qdebug.h>
#include <QtCore/qmath.h>


#include "map.h"
#include "ui_map.h"
#include "client.h"
#include "constants.h"
#include "clover.h"


Map::Map(QWidget *parent) :
    QGLWidget(parent),
    ui(new Ui::Map)
{
    ui->setupUi(this);
    _serv = new Server(this,this);

    if (_serv->doStartServer(QHostAddress(QString("213.227.251.211")),16000)) {
        qDebug()<<"connected";
        connect(_serv,SIGNAL(onClientsCountChanged(int)),this,SLOT(clientsCountChanged(int)));

        _antsCount = 0;
        _antStep = 0.01;
        _antLookingRadius = 0.05;

        setupFood();
        setupBarriers();

        setFormat(QGLFormat(QGL::DoubleBuffer));
        glDepthFunc(GL_LEQUAL);

        QTimer *timer = new QTimer(this);
        connect(timer, SIGNAL(timeout()), this, SLOT(updateGL()));
        timer->start(16);

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


void Map::initializeGL()
{
    qglClearColor(Qt::white); // Черный цвет фона
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
    TRACE("GL Draw");
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // чистим буфер изображения и буфер глубины
    glLoadIdentity();

    qglColor(Qt::black);

    drawBorder();
    renderText(0.01,1 + 2*_antStep,0.0,QString::fromUtf8("Ants count: %1").arg(_antsCount));
    drawBase();
    drawAnts();
    drawFood();
    drawBarries();

    swapBuffers();
}

// static methods

QPointF Map::baseCoord()
{
    return QPointF(0.02,0.02);
}

// public methods

QPointF Map::antBornPoint()
{
    float x = rand()%20 / 100.0;
    float y = rand()%20 / 100.0;

    QPointF bornPoint(x,y);
    return bornPoint;
}

QPointF Map::nextPositionForAnt(Client *ant)
{
    QPointF nextPosition = ant->getPosition() + ant->getDircetion() * _antStep;
    QPolygonF mapBorder;
    mapBorder << QPointF(0.0,0.0) << QPointF(1.0,0.0) << QPointF(1.0,1.0) << QPointF(0.0,1.0);
    if (!mapBorder.containsPoint(nextPosition,Qt::OddEvenFill)) {
        return ant->getPosition();
    }

    foreach (QPolygonF barrier, _barrierPostions) {
        if (barrier.containsPoint(nextPosition,Qt::OddEvenFill)) {
            return ant->getPosition();
        }
    }
    return nextPosition;
}

NearObjects Map::nearObjectsForAnt(Client *ant)
{
    NearObjects objects;

    int sectorsCount = 8;
    float radianStep = 3.14 / (float)sectorsCount;

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

    objects.ants = antsNearAntPolygon(antPolygon);
    objects.food = foodPositionsNearAntPolygon(antPolygon);
    objects.barriers = barriersNearAntPolygon(antPolygon);

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

QPolygonF Map::foodPositionsNearAntPolygon(QPolygonF &antPolygon)
{
    foreach (QPointF foodPos, _foodPositions) {
        if (antPolygon.containsPoint(foodPos,Qt::OddEvenFill)) {
            return _foodPositions;
        }
    }
    return QPolygonF(0);
}

QVector<Client *> Map::antsNearAntPolygon(QPolygonF &antPolygon)
{
    QVector<Client *> nearAnts;

    foreach (Client *ant, _serv->_clients) {
        if (antPolygon.containsPoint(ant->getPosition(),Qt::OddEvenFill)) {
            nearAnts << ant;
        }
    }

    return nearAnts;
}

// private methods

void Map::setupFood()
{
    Point cloverCenter;
    cloverCenter.x = 0.78;
    cloverCenter.y = 0.78;
    _foodPositions = polygonForClover(cloverCenter,0.2,30.0);
}

void Map::setupBarriers()
{
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
}

void Map::drawBorder()
{
    qglColor(Qt::black);

    float offset = 0;

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
    qglColor(Qt::black);
    QList<Client *> ants = _serv->_clients;

    glBegin(GL_LINES);
    {
        foreach (Client *ant, ants) {
            QPointF position = ant->getPosition();
            QPointF direction = ant->getDircetion();
            QPointF position2 = position - direction * _antStep;

            glVertex2f(position.x(),position.y());
            glVertex2f(position2.x(),position2.y());
        }     
    }
    glEnd();    
}


void Map::drawFood()
{
    qglColor(Qt::green);
    glBegin(GL_LINE_LOOP);
    {
        foreach (QPointF foodPoint, _foodPositions) {
            glVertex2f(foodPoint.x(),foodPoint.y());
        }
    }
    glEnd(); 
}

void Map::drawBarries()
{
    qglColor(Qt::darkGray);

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



