#include <qdebug.h>

#include "map.h"
#include "ui_map.h"


Map::Map(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Map)
{
    ui->setupUi(this);

    _serv = new Server(this);
    qDebug()<<_serv;

    if (_serv->doStartServer(QHostAddress::QHostAddress("213.227.249.221"), 9000)) {
        qDebug()<<"connected";
    } else {
        qDebug()<<"failed";
    }
}

Map::~Map()
{
    delete ui;
}
