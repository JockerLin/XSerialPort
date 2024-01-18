#include "XSerialPort.h"
#include <QtWidgets/QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    XSerialPort w;
    w.show();
    return a.exec();
}
