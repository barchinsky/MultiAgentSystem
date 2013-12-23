#include "helpmath.h"

#include <QtCore/qmath.h>
#include <QPointF>
#include <QPolygonF>
#include <QDebug>

float Math::length(float x, float y)
{
    float length = qSqrt(qPow(x,2) + qPow(y,2));
    length = round(length * 10000) / 10000.0;
    return length;
}

float Math::length(float x1, float y1, float x2, float y2)
{
    float x = x2 - x1;
    float y = y2 - y1;
    return Math::length(x,y);
}

QPointF Math::directionFromPointToPoint(float x1, float y1, float x2, float y2)
{
    float x = x2 - x1;
    float y = y2 - y1;

    float length = Math::length(x,y);
    if (length != 0) {
        x /= length;
        y /= length;
    } else {
        x = y = 0;
    }

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

QList<QPolygonF> Math::dividePolygonByPolygon(const QPolygonF &basePolygon, const QPolygonF &dividedByPolygon)
{
    QPolygonF subtractedPolygon = basePolygon.subtracted(dividedByPolygon);

    if (!subtractedPolygon.empty()) {
        subtractedPolygon.pop_back();

        QList<QPolygonF> resultsPolygons;

        QPolygonF tmpPolygon;

        foreach (QPointF point, subtractedPolygon) {
            if (tmpPolygon.count() == 0) {
                tmpPolygon << point;
            } else {
                if (tmpPolygon[0] == point) {
                    resultsPolygons << tmpPolygon;
                    tmpPolygon.clear();
                } else {
                    tmpPolygon << point;
                }
            }
        }

        if (resultsPolygons.count() == 0) {
            resultsPolygons << subtractedPolygon;
        }

        return resultsPolygons;
    }
    QList<QPolygonF> emptyList;
    return emptyList;
}
