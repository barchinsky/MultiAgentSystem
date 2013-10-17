#ifndef MAP_H
#define MAP_H

#include <QWidget>

#include "server.h"

namespace Ui {
class Map;
}

class Map : public QWidget
{
    Q_OBJECT

public:
    explicit Map(QWidget *parent = 0);
    ~Map();

private:
    Ui::Map *ui;
    Server *_serv;
};

#endif // MAP_H
