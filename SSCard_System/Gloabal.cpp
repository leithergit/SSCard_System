#include <QSettings>
#include <QDir>
#include <QCoreApplication>
#include <QSettings>
#include <QFileInfo>
#include "Gloabal.h"

DataCenter::DataCenter()
{
}

DataCenter::~DataCenter()
{

}
// 
int DataCenter::LoadConfig(QString &strError)
{
   
    try
    {
		QString strConfigPath = QCoreApplication::applicationDirPath();
		strConfigPath += "/Config.ini";
		QFileInfo fi(strConfigPath);
		if (!fi.isFile())
		{
			strError = QString(tr("配置文件%1不存在!")).arg(strConfigPath);
			return -1;
		} 
        pConfig = make_shared<Config>();
		if (!pConfig)
		{
			strError = tr("内存不足!");
			return -1;
		}
    
		QSettings Config(strConfigPath, QSettings::IniFormat);

		Config.beginGroup("Device");

		Config.endGroup();

		Config.beginGroup("Region");
		Config.endGroup();

		Config.beginGroup("CardForm");
		Config.endGroup();

		Config.beginGroup("Other");
		Config.endGroup();
    }
   
    catch (std::exception & e)
    {
    }
   

    return 0;
}
