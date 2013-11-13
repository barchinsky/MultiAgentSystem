#include "ant.h"
#include "helpmath.h"

#include <QtCore/qmath.h>

Ant::Ant(QPointF center, float scale, float direction,QObject *parent):
    QObject(parent)
{
    _center = center;
    _scale = scale;
    _direction = direction;
    _previusDirection = 0;

    _shape = baseShape();
    setupShape();

}

// public

QPolygonF Ant::getShape()
{
    return _shape;
}


// slots
void Ant::positionChanged(QPointF center, QPointF vector)
{
    _center = center;
    _previusDirection = _direction;
    _direction = qAcos(vector.x());
    setupShape();
}

// private

QPolygonF Ant::baseShape()
{
    QPolygonF shape;
    // right side
    shape << QPointF(0.500, 0.000);
    shape << QPointF(0.750, 0.125);
    shape << QPointF(0.750, 0.325);
    shape << QPointF(0.520, 0.500);
    shape << QPointF(0.625, 0.625);
    shape << QPointF(0.520, 0.750);
    shape << QPointF(0.700, 0.800);
    shape << QPointF(0.550, 1.000);
    // left size
    shape << QPointF(-0.550, 1.000);
    shape << QPointF(-0.700, 0.800);
    shape << QPointF(-0.520, 0.750);
    shape << QPointF(-0.625, 0.625);
    shape << QPointF(-0.520, 0.500);
    shape << QPointF(-0.750, 0.325);
    shape << QPointF(-0.750, 0.125);

    return shape;
}

void Ant::setupShape()
{
    static bool firstSetup = true;
    float scale = firstSetup?_scale:1;
    float direction = _direction - _previusDirection;
    firstSetup = false;
    _shape = Math::polygonForShape(_shape,_center,scale,direction);
}
