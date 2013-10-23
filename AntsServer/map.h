#ifndef MAP_H
#define MAP_H

#include <QWidget>
#include <QtOpenGL>
#include <QTimer>

#include "server.h"

namespace Ui {
class Map;
}

class Map : public QGLWidget
{
    Q_OBJECT

public:
    explicit Map(QWidget *parent = 0);
    ~Map();

    void initializeGL(); // Метод для инициализирования opengl
    void resizeGL(int nWidth, int nHeight); // Метод вызываемый после каждого изменения размера окна
    void paintGL(); // Метод для вывода изображения на экран

private:
    Ui::Map *ui;
    Server *_serv;
    QTimer timer;
    int wax ,way; // Размеры окна
};

#endif // MAP_H
