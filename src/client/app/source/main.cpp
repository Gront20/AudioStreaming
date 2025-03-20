#include <QApplication>
#include "baseAppCore.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    BaseAppCore app;
    app.start();
    return a.exec();
}
