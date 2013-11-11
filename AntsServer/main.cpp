#include "map.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    srand(time(NULL));
    Map w;
    w.show();

    return a.exec();
}
