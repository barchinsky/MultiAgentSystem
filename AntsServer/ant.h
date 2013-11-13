#ifndef ANT_H
#define ANT_H

#include <QObject>
#include <QPolygonF>
#include <QPointF>

class Ant : public QObject
{
private:
    float _direction;
    float _previusDirection;
    float _scale;
    QPointF _center;
    QPolygonF _shape;

    QPolygonF baseShape();
    void setupShape();

public:    
    Ant(QPointF center, float scale = 1, float direction = 90.0, QObject *parent = 0);
    QPolygonF getShape();

public slots:
    void positionChanged(QPointF center,QPointF vector);

};

#endif // ANT_H
