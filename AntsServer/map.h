#ifndef MAP_H
#define MAP_H

#include <QWidget>
#include <QtOpenGL>
#include <QTimer>
#include <QPolygonF>
#include <QPointF>

#include "server.h"
#include "feromon.h"

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
    static float antScaleFactor();
    float getAntStep();

    QPointF antBornPoint();
    QPointF nextPositionForAnt(Client *ant, bool *isStucked);

    NearObjects nearObjectsForAnt(Client *ant);
    QVector<QPolygonF> barriersNearAntPolygon(QPolygonF &antPolygon);
    QPolygonF foodPositionsNearAntPolygon(QPointF &checkPoint);
    QVector<Client *> antsNearAntPolygon(Client *inputAnt, QPolygonF &antPolygon);
    bool antTryCutTheFood(Client *ant);


private:
    Ui::Map *ui;
    Server *_serv;
    QTimer timer;
    int _antsCount;

    float _antStep;
    float _antLookingRadius;

    QList<QPolygonF> _foodPositions;
    QPolygonF _diedAntsPositions;
    QList<QPolygonF> _barrierPostions;
    QList<Feromon *> _feromons;

    void setupFood();
    void setupBarriers();


    void drawBorder();
    void drawBase();
    void drawAnts();
    void drawFood();
    void drawBarries();
    void drawDiedAnts();
    void drawFeromons();

public slots:
    void clientsCountChanged(int clientsCount);
    void antDiedAtPosition(QPointF point);
    void onUpdateAntsFeromons();
    void onDisappearFeromon(Feromon *feromon);
};

#endif // MAP_H
