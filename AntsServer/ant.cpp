#include "ant.h"
#include "helpmath.h"

#include <QtCore/qmath.h>
#include <QDebug>

Ant::Ant(QPointF center, float scale, float direction, QObject *parent):
    QObject(parent)
{    
    _center = center;
    _scale = scale;
    _direction = direction;
    _baseShape = baseShape();
    setupShape();
}

// public

QPolygonF Ant::getShape()
{
    return _shape;
}


// slots
void Ant::onPositionChanged(QPointF center, QPointF vector)
{    
    qDebug() << "before Ant::onPositionChanged vec:  "<< vector;
    _center = center;    

    float x = vector.x();
    float y = vector.y();
    float angleInTriangle = qAtan(y / x);
    float angleInRad = 0;
    if (x >= 0 && y >= 0) {
        angleInRad = angleInTriangle;
    } else if (x < 0 && y >= 0) {
        angleInRad = M_PI - angleInTriangle;
    } else if (x < 0 && y < 0) {
        angleInRad = M_PI + angleInTriangle;
    } else if (x >= 0 && y < 0) {
        angleInRad = -angleInTriangle;
    }

    _direction = angleInRad * 180 / M_PI;
    qDebug() << "after Ant::onPositionChanged: vec:  "<< _direction;
    setupShape();
}

// private

QPolygonF Ant::baseShape()
{
    QPolygonF shape;
    // right side
    shape << QPointF(0.000, -1.000);
    shape << QPointF(0.250, -0.750);
    shape << QPointF(0.250, -0.350);
    shape << QPointF(0.020,  0.000);
    shape << QPointF(0.125,  0.250);
    shape << QPointF(0.020, (0.750 - 0.5)*2);
    shape << QPointF(0.200, (0.800 - 0.5)*2);
    shape << QPointF(0.050, 1.000);
    // left side
    shape << QPointF(-0.050, 1.000);
    shape << QPointF(-0.200, (0.800 - 0.5)*2);
    shape << QPointF(-0.020, (0.750 - 0.5)*2);
    shape << QPointF(-0.125, (0.625 - 0.5)*2);
    shape << QPointF(-0.020, (0.500 - 0.5)*2);
    shape << QPointF(-0.250, (0.325 - 0.5)*2);
    shape << QPointF(-0.250, (0.125 - 0.5)*2);

    return shape;
}

void Ant::setupShape()
{
    _shape = Math::polygonForShape(_baseShape,_center,_scale,_direction);
}
