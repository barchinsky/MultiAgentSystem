#ifndef MAP_H
#define MAP_H

#include <QWidget>
#include <QtOpenGL>
#include <QTimer>

#include "server.h"

namespace Ui {
class Map;
}

class Map : public QGLWidget
{
    Q_OBJECT

public:
    explicit Map(QWidget *parent = 0);
    ~Map();

    void initializeGL(); // Метод для инициализирования opengl
    void resizeGL(int nWidth, int nHeight); // Метод вызываемый после каждого изменения размера окна
    void paintGL(); // Метод для вывода изображения на экран

    static float antStep();
    static QPointF baseCoord();

    QList<QPointF> _foodPositions;
    QList<QList<QPointF> > _barrierPostions;

private:
    Ui::Map *ui;
    Server *_serv;
    QTimer timer;
    int _antsCount;

    void setupFoods();
    void setupBarriers();

    void drawBorder();
    void drawBase();
    void drawAnts();
    void drawFood();
    void drawBarries();

public slots:
    void clientsCountChanged(int clientsCount);

};

#endif // MAP_H
