#ifndef MAP_H
#define MAP_H

#include <QWidget>
#include <QtOpenGL>
#include <QTimer>
#include <QPolygonF>

#include "server.h"

namespace Ui {
class Map;
}



typedef struct {
    QVector<Client *> ants;
    QPolygonF food;
    QVector<QPolygonF> barriers;
} NearObjects;

class Map : public QGLWidget
{
    Q_OBJECT

public:
    explicit Map(QWidget *parent = 0);
    ~Map();

    void initializeGL(); // Метод для инициализирования opengl
    void resizeGL(int nWidth, int nHeight); // Метод вызываемый после каждого изменения размера окна
    void paintGL(); // Метод для вывода изображения на экран

    static QPointF baseCoord();

    QPointF antBornPoint();
    QPointF nextPositionForAnt(Client *ant);

    NearObjects nearObjectsForAnt(Client *ant);
    QVector<QPolygonF> barriersNearAntPolygon(QPolygonF &antPolygon);
    QPolygonF foodPositionsNearAntPolygon(QPolygonF &antPolygon);
    QVector<Client *> antsNearAntPolygon(QPolygonF &antPolygon);

private:
    Ui::Map *ui;
    Server *_serv;
    QTimer timer;
    int _antsCount;

    float _antStep;
    float _antLookingRadius;

    QPolygonF _foodPositions;
    QList<QPolygonF> _barrierPostions;

    void setupFood();
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
