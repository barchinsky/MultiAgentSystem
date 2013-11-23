#include "feromon.h"
#include <stdio.h>

#include <QTimer>
#include <QDebug>

static const float kTimerUpdateInterval = 100; // in msec


Feromon::Feromon(QPointF position, QObject *parent) :
    QObject(parent)
{
    _position = position;
    _startTime = 0;
    _alpha = 1.0;

    _timer = new QTimer(this);
    connect(_timer, SIGNAL(timeout()), this, SLOT(onUpdateState()));
    _timer->start(kTimerUpdateInterval);
}

void Feromon::onUpdateState()
{    
    _startTime += kTimerUpdateInterval;
    if (_startTime >= 10000 && _startTime <= 20000) {
        _alpha = 1.0 - ((_startTime - 10000.0) / 10000.0);
    }

    if (_startTime > 20000) {
        _timer->stop();
        delete _timer;
        emit disappearFeromon(this);
    }
}

float Feromon::getAlpha()
{
    return _alpha;
}

QPointF Feromon::getPosition()
{
    return _position;
}
