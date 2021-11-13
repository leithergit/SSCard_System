#include "mainwindow.h"

#include <QApplication>
#include <QString>
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
    if (!g_pDataCenter)
    {
        gError() << "内存不足，数据中心无法初始化!";
        return -1;
    }
    QString strError;
    if (g_pDataCenter->LoadConfig(strError))
    {
        gError() << "加载配置文件失败:"<<strError.toStdString();
        return -1;
    }
    //w.showFullScreen();
    w.showMaximized();
    int nRes =  a.exec();
    google::ShutdownGoogleLogging();
    return nRes;
}
