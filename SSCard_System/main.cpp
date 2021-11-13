#include "mainwindow.h"

#include <QApplication>
#include "Gloabal.h"

int main(int argc, char *argv[])
{
    google::InitGoogleLogging(argv[0]);

    // 日志目录必须存在才会生成日志
    //FLAGS_log_dir = "./log/";                             // 使用该设定时，默认情况下，不同级别日志使用不同的文件名
    google::SetLogDestination(google::GLOG_INFO, "./log/");	// 使用该设定时，默认情况下，所有级别日志都用同一的文件名
    google::SetStderrLogging(google::GLOG_INFO);
    google::SetLogFilenameExtension(".log");
    QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
    QApplication a(argc, argv);
    MainWindow w;
    int nInfo = 1;
    int nWarning = 2;
    int nError = 3;
    info()<<"This is a information:"<<nInfo;
    warning()<<"This is a Warning:"<<nWarning;
    error()<<"This is a information:"<<nError;
    //w.showFullScreen();
    w.showMaximized();
    int nRes =  a.exec();
    google::ShutdownGoogleLogging();
    return nRes;
}
