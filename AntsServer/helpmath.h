#ifndef MATH_H
#define MATH_H

#include <QList>

class QPointF;
class QPolygonF;

class Math
{
public:
    static float length(float x,float y);
    static float length(float x1, float y1, float x2, float y2);
    static QPointF directionFromPointToPoint(float x1, float y1, float x2, float y2);
    static QPolygonF polygonForShape(QPolygonF const &inputShape, QPointF center, float scale, float angle);
    static QList<QPolygonF> dividePolygonByPolygon(QPolygonF const &basePolygon, QPolygonF const &dividedByPolygon);
};

#endif // MATH_H
