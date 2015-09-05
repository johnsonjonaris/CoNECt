#include <QApplication>
#include "mainwindow.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    MainWindow w;
    w.setWindowTitle("CoNECt");
    w.showMaximized();
    w.resetViews();

    return a.exec();
}
