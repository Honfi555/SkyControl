// #include "mainwindow.h"
#include "authorization/authorization.h"

#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    Authorization window;
    // MainWindow window;
    window.show();
    return a.exec();
}
