#include "dialog.h"

#include <QApplication>
#include <QString>

int main(int argc, char *argv[])
{
    QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
    QApplication a(argc, argv);
    QString strAppPath = QCoreApplication::applicationDirPath();
    Dialog w;
    w.show();
    return a.exec();
}
