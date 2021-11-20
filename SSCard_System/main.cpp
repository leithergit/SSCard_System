#pragma execution_character_set("utf-8")
#include "mainwindow.h"
#include <QApplication>
#include <QString>
#include "Gloabal.h"

DataCenterPtr g_pDataCenter ;

int main(int argc, char* argv[])
{
	google::InitGoogleLogging(argv[0]);

	// 日志目录必须存在才会生成日志
	//FLAGS_log_dir = "./log/";                             // 使用该设定时，默认情况下，不同级别日志使用不同的文件名
	google::SetLogDestination(google::GLOG_INFO, "./log/");	// 使用该设定时，默认情况下，所有级别日志都用同一的文件名
	google::SetStderrLogging(google::GLOG_INFO);
	google::SetLogFilenameExtension(".log");
	QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
	QApplication a(argc, argv);

    g_pDataCenter = make_shared<DataCenter>();
	if (!g_pDataCenter)
	{
        gError() << QString("内存不足,初始数据中心失败!").toLocal8Bit().data();
		return -1;
	}
	QString strError;
	if (g_pDataCenter->LoadSysConfigure(strError))
	{
        gError() << QString("加载系统配置失败:%1").arg(strError).toLocal8Bit().data();
		return -1;
    }
    if (g_pDataCenter->LoadCardForm(strError))
    {
        gError() << QString("加载卡版打印模块失败:%1").arg(strError).toLocal8Bit().data();
        return -1;
    }

    MainWindow w;
	//w.showFullScreen();
	w.showMaximized();

    int nRes = a.exec();
	google::ShutdownGoogleLogging();
	return nRes;
}
