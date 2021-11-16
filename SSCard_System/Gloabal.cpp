#pragma execution_character_set("utf-8")
#include <QSettings>
#include <QDir>
#include <QCoreApplication>
#include <QSettings>
#include <QFileInfo>
#include "Gloabal.h"

extern DataCenterPtr g_pDataCenter;
DataCenter::DataCenter()
{
}

DataCenter::~DataCenter()
{

}
// 
int DataCenter::LoadSysConfigure(QString& strError)
{
	try
	{
		QString strConfigPath = QCoreApplication::applicationDirPath();
		strConfigPath += "/Configure.ini";
		QFileInfo fi(strConfigPath);
		if (!fi.isFile())
		{
            strError = QString("加载配置文件失败:%1!").arg(strConfigPath);
			return -1;
		}
		QSettings ConfigIni(strConfigPath, QSettings::IniFormat);
		pConfig = make_shared<SysConfig>(&ConfigIni);
		if (!pConfig)
		{
            strError = "内存不足，无法初始化数据中心!";
			return -1;
		}
		return 0;
	}

	catch (std::exception& e)
	{
		strError = "Catch an exception:";
		strError += e.what();
        gError() << strError.toLatin1().data();
		return -1;
	}
}


int DataCenter::LoadCardForm(QString& strError)
{
	try
	{
		QString strConfigPath = QCoreApplication::applicationDirPath();
		strConfigPath += "/CardForm.ini";
		QFileInfo fi(strConfigPath);
		if (!fi.isFile())
		{
            strError = QString("加载卡版打印版式失败:%1!").arg(strConfigPath);
			return -1;
		}
		QSettings ConfigIni(strConfigPath, QSettings::IniFormat);
        pCardForm = make_shared<CardForm>(&ConfigIni);
        if (!pCardForm)
		{
            strError = "内存不足，无法初始化数据中心!";
			return -1;
		}
		return 0;
	}

	catch (std::exception& e)
	{
		strError = "Catch an exception:";
		strError += e.what();
        gError() << strError.toLatin1().data();
		return -1;
	}
}
