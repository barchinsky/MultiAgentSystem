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



    if (_serv->doStartServer(QHostAddress(QString("127.0.0.1")),12000)) {
        qDebug()<<"connected";
        connect(_serv,SIGNAL(onClientsCountChanged(int)),this,SLOT(clientsCountChanged(int)));
    } else {
        qDebug()<<"failed";
    }

    setFormat(QGLFormat(QGL::DoubleBuffer));
    glDepthFunc(GL_LEQUAL);

     // Configure the timer
//    connect(&timer, SIGNAL(timeout()), this, SLOT(updateGL()));
//    timer.start(16);

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
    renderText(0.01,1.01,0.0,QString::fromUtf8("Ants count: %1").arg(_antsCount));
    drawBase();
    drawAnts();

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

void Map::drawBorder()
{
    qglColor(Qt::black);

    glBegin(GL_LINE_LOOP);
    glVertex2f(0,1);
    glVertex2f(1,1);
    glVertex2f(1,0);
    glVertex2f(0,0);
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



