#include <QApplication>
#include <QMainWindow>

#include "mainwidget.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    QMainWindow w;
    MainWidget mainWidget;
    w.setCentralWidget(&mainWidget);
    w.show();
    return a.exec();
}
