#include "clover.h"
#include <math.h>


QPolygonF polygonForClover(Point center, float scale, float angle)
{
    QPolygonF cloverPolygon;

    float rad = angle * 0.017453293;

    float a = cos(rad);
    float b = sin(rad);
    float c = -sin(rad);
    float d = cos(rad);
    float e = center.x;
    float f = center.y;

    for (int i = 0; i < cloverPointsCount; i++) {
        Point point = clover[i];
        float x = (point.x*a + point.y*c) * scale  + e;
        float y = (point.x*b + point.y*d) * scale  + f;
        cloverPolygon.append(QPointF(x,y));
    }

    return cloverPolygon;
}
