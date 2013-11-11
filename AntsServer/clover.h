#ifndef CLOVER_H
#define CLOVER_H

#include <QPolygonF>

typedef struct {
    float x;
    float y;
} Point;

static const int cloverPointsCount = 25;
static Point clover[cloverPointsCount] =
{
    //root start
    {-0.20,-1.00},
    { 0.00,-1.00},
    { 0.10,-0.50},
    { 0.00, 0.00},
    // right leaflet
    { 0.30,-0.30},
    { 0.75,-0.40},
    { 1.00,-0.25},
    { 0.80, 0.00},
    { 1.00, 0.25},
    { 0.75, 0.40},
    { 0.30, 0.30},
    // top leaflet
    { 0.40, 0.75},
    { 0.25, 1.00},
    { 0.00, 0.80},
    {-0.25, 1.00},
    {-0.40, 0.75},
    //left leaflet
    {-0.30, 0.30},
    {-0.75, 0.40},
    {-1.00, 0.25},
    {-0.80, 0.00},
    {-1.00,-0.25},
    {-0.75,-0.40},
    {-0.30,-0.30},
    //root end
    { 0.00, 0.00},
    {-0.05,-0.60}
};

QPolygonF polygonForClover(Point center, float scale, float angle);

#endif // CLOVER_H
