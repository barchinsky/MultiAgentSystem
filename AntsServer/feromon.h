#ifndef FEROMON_H
#define FEROMON_H

#include <QObject>
#include <QPointF>
#include <QTime>

class QTimer;

class Feromon : public QObject
{
    Q_OBJECT
private:
    QPointF _position;
    QTime _startTime;
    float _alpha;
    QTimer *_timer;

public:
    explicit Feromon(QPointF position, QObject *parent = 0);
    float getAlpha();
    QPointF getPosition();
    
signals:
    void disappearFeromon(Feromon *feromon);
    
public slots:
    void onUpdateState();
    
};

#endif // FEROMON_H
