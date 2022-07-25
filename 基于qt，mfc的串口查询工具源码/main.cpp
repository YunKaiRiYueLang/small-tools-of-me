#define _AFXDLL
#include "comdisp.h"
#include <QtWidgets/QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    comdisp w;
    w.show();
    return a.exec();
}
