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

    if (_serv->doStartServer(QHostAddress::QHostAddress("127.0.0.1"), 9000)) {
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
    timer->start(1);
}

Map::~Map()
{
    delete ui;
}

// slots

void Map::clientsCountChanged(int clientsCount)
{
    qDebug()<<"clients count:"<<clientsCount;
//    ui->lbClientCount->setText(QString::number(clientsCount));
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
    glOrthof(0.0, 1.0, 0.0, 1.0, 1.0, 0.0);
#else
    glOrtho(0, 1, 0, 1, 1.0, 0.0);
#endif
    glMatrixMode(GL_MODELVIEW);
}

void Map::paintGL()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // чистим буфер изображения и буфер глубины
    glLoadIdentity();

    qglColor(Qt::black);

    renderText(0.5,0.5,0.0,QString("ants "));
    drawAnts();

    swapBuffers();
}

void Map::drawAnts()
{
//    QList<Client *> ants = _serv->_clients;
    int count = _serv->_clients.count();


    glLineWidth(2.0);
    glBegin(GL_LINES);
    {

        for (int i = 0; i < 10; i++) {
            float x = rand()%100 / 100.0;
            float y = rand()%100 / 100.0;
            glVertex2f(x,y);
            glVertex2f(x+0.01,y+0.01);
        }
        /*

        foreach (Client *ant, ants) {
            QPointF position = ant->getPosition();
            QPointF direction = ant->getDircetion();

            glVertex2f(position.x(),position.y());
            glVertex2f(direction.x()*0.01,direction.y()*0.01);
        }
        */
    }
    glEnd();
    glLineWidth(1.0);
}



