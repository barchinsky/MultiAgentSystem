#include <qdebug.h>
#include <QtCore/qmath.h>

#include "map.h"
#include "ui_map.h"
#include "client.h"



Map::Map(QWidget *parent) :
    QGLWidget(parent),
    ui(new Ui::Map)
{
    ui->setupUi(this);

    _serv = new Server(this);
    qDebug()<<_serv;

    _antsCount = 0;
    setupFoods();
    setupBarriers();

    if (_serv->doStartServer(QHostAddress(QString("127.0.0.1")),12000)) {
        qDebug()<<"connected";
        connect(_serv,SIGNAL(onClientsCountChanged(int)),this,SLOT(clientsCountChanged(int)));
    } else {
        qDebug()<<"failed";
    }

    setFormat(QGLFormat(QGL::DoubleBuffer));
    glDepthFunc(GL_LEQUAL);

    QTimer *timer = new QTimer(this);
    connect(timer, SIGNAL(timeout()), this, SLOT(updateGL()));
    timer->start(16);
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
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // чистим буфер изображения и буфер глубины
    glLoadIdentity();

    qglColor(Qt::black);

    drawBorder();
    renderText(0.01,1 + 2*antStep(),0.0,QString::fromUtf8("Ants count: %1").arg(_antsCount));
    drawBase();
    drawAnts();
    drawFood();
    drawBarries();

    swapBuffers();
}

float Map::antStep()
{
    return 0.01;
}

QPointF Map::baseCoord()
{
    return QPointF(0.02,0.02);
}

void Map::setupFoods()
{
    _foodPositions.append(QPointF(0.70,0.70));
    _foodPositions.append(QPointF(0.80,0.68));
    _foodPositions.append(QPointF(0.59,0.89));
    _foodPositions.append(QPointF(0.78,0.49));
    _foodPositions.append(QPointF(0.89,0.64));
}

void Map::setupBarriers()
{
    QList<QPointF> barrier1;
    barrier1.append(QPointF(0.15,0.52));
    barrier1.append(QPointF(0.43,0.48));
    barrier1.append(QPointF(0.50,0.19));
    barrier1.append(QPointF(0.30,0.21));

    QList<QPointF> barrier2;
    barrier2.append(QPointF(0.61,0.29));
    barrier2.append(QPointF(0.60,0.55));
    barrier2.append(QPointF(0.75,0.61));
    barrier2.append(QPointF(0.70,0.35));
    barrier2.append(QPointF(0.73,0.36));
    barrier2.append(QPointF(0.69,0.15));

    QList<QPointF> barrier3;
    barrier3.append(QPointF(0.32,0.59));
    barrier3.append(QPointF(0.10,0.75));
    barrier3.append(QPointF(0.29,0.81));
    barrier3.append(QPointF(0.40,0.78   ));
    barrier3.append(QPointF(0.42,0.61));

    _barrierPostions.append(barrier1);
    _barrierPostions.append(barrier2);
    _barrierPostions.append(barrier3);

}

void Map::drawBorder()
{
    qglColor(Qt::black);

    float offset = antStep();

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
            QPointF position2 = position + direction * antStep();

            glVertex2f(position.x(),position.y());
            glVertex2f(position2.x(),position2.y());
        }     
    }
    glEnd();    
}


void Map::drawFood()
{
    qglColor(Qt::green);
    glPointSize(5);
    glBegin(GL_POINTS);
    {
        foreach (QPointF foodPoint, _foodPositions) {
            glVertex2f(foodPoint.x(),foodPoint.y());
        }
    }
    glEnd();
    glPointSize(1);
}

void Map::drawBarries()
{
    qglColor(Qt::darkGray);

    foreach (QList<QPointF> barrier, _barrierPostions) {
        glBegin(GL_LINE_LOOP);
        {
            foreach (QPointF point, barrier) {
                glVertex2f(point.x(),point.y());
            }
            glEnd();
        }
    }
}



