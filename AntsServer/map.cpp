#include <qdebug.h>

#include "map.h"
#include "ui_map.h"
#include <QtCore/qmath.h>


Map::Map(QWidget *parent) :
    QGLWidget(parent),
    ui(new Ui::Map)
{
    ui->setupUi(this);

    _serv = new Server(this);
    qDebug()<<_serv;

    if (_serv->doStartServer(QHostAddress::QHostAddress("213.227.249.221"), 9000)) {
        qDebug()<<"connected";
    } else {
        qDebug()<<"failed";
    }

    setFormat(QGLFormat(QGL::DoubleBuffer));
    glDepthFunc(GL_LEQUAL);

     // Configure the timer
    connect(&timer, SIGNAL(timeout()), this, SLOT(updateGL()));
    timer.start(17);


}

Map::~Map()
{
    delete ui;
}

void Map::initializeGL()
{
    qglClearColor(Qt::white); // Черный цвет фона
}

void Map::resizeGL(int nWidth, int nHeight)
{
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glViewport(0, 0, (GLint)nWidth, (GLint)nHeight);
    wax=nWidth;
        way=nHeight;
}

void Map::paintGL()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // чистим буфер изображения и буфер глубины
    glMatrixMode(GL_PROJECTION); // устанавливаем матрицу
    glLoadIdentity(); // загружаем матрицу
    glOrtho(0,wax,way,0,1,0); // подготавливаем плоскости для матрицы
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    qglColor(Qt::black);
//    renderText(10, 10 , 0, QString::fromUtf8("Вы набрали очков:"), QFont() , 2000);

//    glBegin(GL_POLYGON);
//    glColor4f(0,1,0, 0.25);// Цвет выделенной области
//    // Координаты выделенной области
//    glVertex2f(10, 10);
//    glVertex2f(10, 30);
//    glVertex2f(30, 30);
//    glVertex2f(30, 10);
//    glEnd();

    static float rad = 0;
    const float radius = 100;
    float x0 = 100;
    float y0 = 100;
    float x = x0 + radius*qCos(rad);
    float y = y0 + radius*qSin(rad);
    rad += 0.314 ;
    glBegin(GL_LINES);
    {
        glVertex2f(x0,y0);
        glVertex2f(x,y);
    }
    glEnd();


    swapBuffers();
}



