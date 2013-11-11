#include "helpmath.h"

#include <QtCore/qmath.h>
#include <QPointF>

float Math::length(float x1, float y1, float x2, float y2)
{
    float x = x2 - x1;
    float y = y2 - y1;
    float length = qSqrt(qPow(x,2) + qPow(y,2));
    return length;
}

QPointF Math::directionFromPointToPoint(float x1, float y1, float x2, float y2)
{
    float x = x2 - x1;
    float y = y2 - y1;

    float length = Math::length(x,y);
    x /= length;
    y /= length;

    return QPointF(x,y);
}
