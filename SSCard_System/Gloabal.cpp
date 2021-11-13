#include <QSettings>
#include <QDir>
#include <QCoreApplication>
#include <QSettings>
#include <QFileInfo>
#include "Gloabal.h"

DataCenterPtr g_pDataCenter = make_shared<DataCenter>();

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
			strError = QString("�����ļ�%1������!").arg(strConfigPath);
			return -1;
		} 
		QSettings ConfigIni(strConfigPath, QSettings::IniFormat);
        pConfig = make_shared<Config>(&ConfigIni);
		if (!pConfig)
		{
			strError = "�ڴ治��!";
			return -1;
		}	
		return 0;
    }
   
    catch (std::exception & e)
    {
		strError = "�����쳣:";
		strError += e.what();
		//_error() << "Catch exception:" << e.what();
		return -1;
    }
}
