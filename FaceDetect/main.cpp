#include "FaceDetect.h"
#include <QtWidgets/QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    FaceDetect w;
    w.show();
    return a.exec();
}
