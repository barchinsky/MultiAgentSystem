#include "helpmath.h"

#include <QtCore/qmath.h>
#include <QPointF>
#include <QPolygonF>

float Math::length(float x1, float y1, float x2, float y2)
{
    float x = x2 - x1;
    float y = y2 - y1;
    float length = qSqrt(qPow(x,2) + qPow(y,2));
    length = round(length * 100) / 100.0;
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


QPolygonF Math::polygonForShape(QPolygonF const &inputShape, QPointF center, float scale, float angle)
{
    QPolygonF newPolygon;

    float rad = angle * 0.017453293;

    float a = cos(rad);
    float b = sin(rad);
    float c = -sin(rad);
    float d = cos(rad);
    float e = center.x();
    float f = center.y();

    foreach (QPointF point, inputShape) {
        float x0 = point.x();
        float y0 = point.y();
        float x = (x0*a + y0*c) * scale  + e;
        float y = (x0*b + y0*d) * scale  + f;
        newPolygon.append(QPointF(x,y));
    }

    return newPolygon;
}
