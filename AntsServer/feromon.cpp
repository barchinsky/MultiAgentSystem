#include "feromon.h"
#include <stdio.h>

#include <QTimer>
#include <QDebug>


Feromon::Feromon(QPointF position, QObject *parent) :
    QObject(parent)
{
    _position = position;
    _startTime = QTime::currentTime();
    _alpha = 1.0;

    _timer = new QTimer(this);
    connect(_timer, SIGNAL(timeout()), this, SLOT(onUpdateState()));
    _timer->start(100);
}

void Feromon::onUpdateState()
{
    QTime currentTime = QTime::currentTime();
    int diffIn_msec = currentTime.msec() - _startTime.msec();


    qDebug() <<"Diff time" << diffIn_msec;

    if (diffIn_msec >= 10000 && diffIn_msec <= 20000) {
        _alpha = 1.0 - ((diffIn_msec - 10000) / 10000);
    }

    if (diffIn_msec > 20000) {
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