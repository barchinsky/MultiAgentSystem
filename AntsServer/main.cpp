#include "map.h"
#include <QApplication>
#include <QDebug>


int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    srand(time(NULL));
    Map w;
    w.show();
    return a.exec();
}
