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

QPointF Ant::getMouthPosition()
{
    return _mouthPosition;
}

QPolygonF Ant::getLeaflet()
{
    QPolygonF leafletShape;
    leafletShape << _mouthPosition;

    float x0 = _mouthPosition.x();
    float y0 = _mouthPosition.y();

    float radius = 3.0 * _scale;

    float angle1 = (float)(rand()%10) * M_PI / 180.0;
    float angle2 = angle1 + 100 * M_PI / 180.0;

    float angles[2] = {angle1,angle2};

    for (int i = 0; i < 2; i++) {
        float randAngle = angles[i];
        float x = x0 + radius * cos(randAngle);
        float y = y0 + radius * sin(randAngle);
        leafletShape << QPointF(x,y);
    }

    return leafletShape;
}


// slots
void Ant::onPositionChanged(QPointF center, QPointF vector)
{       
    _center = center;    

    float x = vector.x();
    float y = vector.y();

    float angleInTriangle = qAtan(y / x);
    float angleInRad = 0;
    if (x >= 0 && y >= 0) {
        angleInRad = angleInTriangle;
    } else if (x < 0 && y >= 0) {
        angleInRad = M_PI + angleInTriangle;
    } else if (x < 0 && y < 0) {
        angleInRad = M_PI + angleInTriangle;
    } else if (x >= 0 && y < 0) {
        angleInRad = angleInTriangle;
    }

    _direction = angleInRad * 180 / M_PI - 90;
    setupShape();

    _mouthPosition = _center + vector * _scale * 1.5;
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
    shape << QPointF(0.020,  0.500);
    shape << QPointF(0.200,  0.600);
    shape << QPointF(0.050, 1.000);
    // left side
    shape << QPointF(-0.050, 1.000);
    shape << QPointF(-0.200, 0.600);
    shape << QPointF(-0.020, 0.500);
    shape << QPointF(-0.125, 0.250);
    shape << QPointF(-0.020, 0.000);
    shape << QPointF(-0.250, -0.350);
    shape << QPointF(-0.250, -0.750);

    return shape;
}

void Ant::setupShape()
{
    _shape = Math::polygonForShape(_baseShape,_center,_scale,_direction);    
}
