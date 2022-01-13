#include "register.h"

#include <QApplication>

int main(int argc, char* argv[])
{
	QGuiApplication::setHighDpiScaleFactorRoundingPolicy(Qt::HighDpiScaleFactorRoundingPolicy::PassThrough);
	QGuiApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
	QApplication a(argc, argv);
	Register w;
	w.show();
	return a.exec();
}
