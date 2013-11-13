#include "ant.h"
#include "helpmath.h"

#include <QtCore/qmath.h>
#include <QDebug>

Ant::Ant(QPointF center, float scale, float direction, QObject *parent):
    QObject(parent)
{
    _center = center;
    _previusCenter = QPointF(0,0);

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
void Ant::onPositionChanged(QPointF center, QPointF vector)
{
    _previusCenter = _center;
    _center = center;
    _previusDirection = _direction;
    _direction = (qAcos(vector.x()) * 180 / M_PI) - 90;

    qDebug() << "\n\n\nPosition changed "<<_center<<" angle "<<_direction;

    setupShape();
}

// private

QPolygonF Ant::baseShape()
{
    QPolygonF shape;
    // right side
    shape << QPointF(0.500 - 0.5, (0.000 - 0.5)*2);
    shape << QPointF(0.750 - 0.5, (0.125 - 0.5)*2);
    shape << QPointF(0.750 - 0.5, (0.325 - 0.5)*2);
    shape << QPointF(0.520 - 0.5, (0.500 - 0.5)*2);
    shape << QPointF(0.625 - 0.5, (0.625 - 0.5)*2);
    shape << QPointF(0.520 - 0.5, (0.750 - 0.5)*2);
    shape << QPointF(0.700 - 0.5, (0.800 - 0.5)*2);
    shape << QPointF(0.550 - 0.5, (1.000 - 0.5)*2);
    // left side
    shape << QPointF(-0.550 + 0.5, (1.000 - 0.5)*2);
    shape << QPointF(-0.700 + 0.5, (0.800 - 0.5)*2);
    shape << QPointF(-0.520 + 0.5, (0.750 - 0.5)*2);
    shape << QPointF(-0.625 + 0.5, (0.625 - 0.5)*2);
    shape << QPointF(-0.520 + 0.5, (0.500 - 0.5)*2);
    shape << QPointF(-0.750 + 0.5, (0.325 - 0.5)*2);
    shape << QPointF(-0.750 + 0.5, (0.125 - 0.5)*2);

    return shape;
}

void Ant::setupShape()
{
    static bool firstSetup = true;
    float scale = firstSetup?_scale:1;
    float direction = _direction - _previusDirection;
    QPointF position = _center - _previusCenter;
    firstSetup = false;
    _shape = Math::polygonForShape(_shape,position,scale,direction);

    qDebug() << "New ant coords";
    foreach (QPointF point, _shape) {
                qDebug() << "["<<point.x()<<"\t;\t"<<point.y()<<"]";
    }
}
