#include "camnetctrlwidget.h"

#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    CamNetCtrlWidget w;
    w.show();
    return a.exec();
}
