#ifndef ANT_H
#define ANT_H

#include <QObject>
#include <QPolygonF>
#include <QPointF>

class Ant : public QObject
{
    Q_OBJECT
private:    
    float _direction;    
    float _scale;
    QPointF _center;
    QPointF _mouthPosition;
    QPolygonF _shape;
    QPolygonF _baseShape;

    QPolygonF baseShape();
    void setupShape();

public:    
    Ant(QPointF center, float scale = 1, float direction = 90.0, QObject *parent = 0);
    QPolygonF getShape();
    QPointF getMouthPosition();
    QPolygonF getLeaflet();

public slots:
    void onPositionChanged(QPointF center,QPointF vector);

};

#endif // ANT_H
