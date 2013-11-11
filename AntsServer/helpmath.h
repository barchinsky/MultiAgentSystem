#ifndef MATH_H
#define MATH_H

class QPointF;

class Math
{
public:
    static float length(float x1, float y1, float x2 = 0.0, float y2 = 0.0);
    static QPointF directionFromPointToPoint(float x1, float y1, float x2, float y2);
};

#endif // MATH_H