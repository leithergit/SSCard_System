#pragma execution_character_set("utf-8")
#include "mainwindow.h"
#include <QApplication>
#include <QString>
#include <QDesktopWidget>
#include <QScreen>
#include <fstream>
#include <QSharedMemory>
#include "qtsingleapplication.h"
#include "waitingprogress.h"

#include "DevBase.h"

#include "Gloabal.h"
#include "license.h"
#include "showlicense.h"
#include "BugTrap.h"
#include "../utility/json/CJsonObject.hpp"
#pragma comment(lib,"BugTrapU.lib")
extern QScreen* g_pCurScreen;

DataCenterPtr g_pDataCenter;
using namespace neb;
using namespace std;
const char* g_szPageOperation[4] =
{
	Str(Return_MainPage),
	Str(Stay_CurrentPage),
	Str(Switch_NextPage),
	Str(Skip_NextPage)
	Str(Retry_CurrentPage)
};

static void SetupExceptionHandler()
{
	BT_SetAppName(L"SSCard_System");
	BT_SetActivityType(BTA_SAVEREPORT);
	TCHAR szReportPath[1024] = { 0 };
	GetAppPath(szReportPath, 1024);
	BT_SetReportFilePath(szReportPath);
	BT_SetFlags(BTF_DETAILEDMODE | BTF_RESTARTAPP);
	BT_InstallSehFilter();

}

//const char* g_pUniqueID = "2F026B66-2CCA-40AB-AD72-435B5AC2E625";

int main(int argc, char* argv[])
{
	SetupExceptionHandler();

	//qputenv("QT_ENABLE_HIGHDPI_SCALING", "1");
	QGuiApplication::setHighDpiScaleFactorRoundingPolicy(Qt::HighDpiScaleFactorRoundingPolicy::PassThrough);
	QGuiApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
	//QCoreApplication::setAttribute(Qt::AA_UseHighDpiPixmaps);
	//QApplication a(argc, argv);
	QtSingleApplication a(argc, argv);
	//if (a.sendMessage("Wake up!"))
	//	return 0;
	//QApplication a(argc, argv);
	//google::InitGoogleLogging(argv[0]);
	//font.setStyleStrategy(QFont::PreferAntialias);
//    QFileInfo fi("D:\\Work\\SSCard_System\\MainProject\\SSCard_System\\debug\\log\\2021_12_31\\20211231-102058.13112.log");
//    qDebug()<<"fi.absoluteDir()="<<fi.absoluteDir();
//    qDebug()<<"fi.absoluteFilePath()="<<fi.absoluteFilePath();
//    qDebug()<<"fi.baseName()="<<fi.baseName();
//    qDebug()<<"fi.bundleName()="<<fi.bundleName();
//    qDebug()<<"fi.canonicalPath()="<<fi.canonicalPath();
//    qDebug()<<"fi.canonicalFilePath()="<<fi.canonicalFilePath();
//    qDebug()<<"fi.completeBaseName()="<<fi.completeBaseName();
//    qDebug()<<"fi.completeSuffix()="<<fi.completeSuffix();
//    qDebug()<<"fi.fileName()="<<fi.fileName();
//    qDebug()<<"fi.filePath()="<<fi.filePath();
//    qDebug()<<"fi.path()="<<fi.path();
//    string strFilePath = "D:\\Work\\SSCard_System\\MainProject\\SSCard_System\\debug\\log\\2021_12_31\\20211231-102058.13112.log";
//    string strPath = "D:\\Work\\SSCard_System\\MainProject\\SSCard_System\\debug\\log";
//    int nPos = strFilePath.find(strPath);
//    nPos += strPath.size();
//    string strlogPath = strFilePath.substr(nPos,strFilePath.size() - nPos);
//    qDebug()<< strlogPath.c_str();

	// 日志目录必须存在才会生成日志
	//FLAGS_log_dir = "./log/";                             // 使用该设定时，默认情况下，不同级别日志使用不同的文件名
	QString strLogPath = QCoreApplication::applicationDirPath();
	QDir::setCurrent(strLogPath);
	qDebug() << QDir::currentPath();
	strLogPath += "/log";
	QDateTime curDate = QDateTime::currentDateTime();
	QString strLogDate = curDate.toString("yyyy_MM_dd");
	QString strLogDatePath = QString("%1/%2/").arg(strLogPath).arg(strLogDate);
	vector<QString> vecDir = { strLogPath,strLogDatePath };
	for (auto var : vecDir)
	{
		QFileInfo fidir(var);
		if (!fidir.isDir())
		{
			gInfo() << "Directory " << var.toStdString() << "not exist,try to create...!";
			QDir dir;
			if (!dir.mkpath(var))
			{
				gInfo() << "************Failed in creating directory '" << var.toStdString() << "',may cause some exception!************";
			}
		}
	}
	FLAGS_max_log_size = 256;
	google::SetLogDestination(google::GLOG_INFO, strLogDatePath.toLocal8Bit().data());	// 使用该设定时，默认情况下，所有级别日志都用同一的文件名
	google::SetStderrLogging(google::GLOG_INFO);	// 大于该级别的日志输出到stderr
	google::SetLogFilenameExtension(".log");
	google::InitGoogleLogging(strLogDatePath.toLocal8Bit().data());

	ifstream ifs("./AppRuning.json");
	int nRunCount = 0;
	QDateTime Now = QDateTime::currentDateTime();
	if (ifs)
	{
		stringstream ss;
		ss << ifs.rdbuf();
		CJsonObject RunJson(ss.str());
		if (RunJson.KeyExist("StartTime") &&
			RunJson.KeyExist("Count"))
		{
			string strStartTime;
			RunJson.Get("StartTime", strStartTime);
			RunJson.Get("Count", nRunCount);
			QDateTime lastStartTime = QDateTime::fromString(strStartTime.c_str(), "yyyy-MM-dd hh:mm:ss");

			int nDiffSecond = lastStartTime.secsTo(Now);
			if (nDiffSecond <= 1)
			{
				if (nRunCount >= 3)
					return 0;
				else
					nRunCount++;
			}
		}
	}

	ofstream ofs("./AppRuning.json");
	CJsonObject RunJson;
	QString strRunTime = Now.toString("yyyy-MM-dd hh:mm:ss");
	RunJson.Add("StartTime", strRunTime.toStdString());
	RunJson.Add("Count", nRunCount);
	ofs << RunJson.ToString();
	ofs.close();

	curl_global_init(CURL_GLOBAL_WIN32);

	g_pDataCenter = make_shared<DataCenter>();
	QString strMessage;
	int nResult = -1;
	do
	{
		if (!g_pDataCenter)
		{
			strMessage = "内存不足,初始化数据中心失败!";
			break;
		}
		QString strError;
		if (g_pDataCenter->LoadSysConfigure(strError))
		{
			strMessage = QString("加载系统配置失败:%1").arg(strError);
			gError() << QString("加载系统配置失败:%1").arg(strError).toLocal8Bit().data();
			break;
		}
		if (g_pDataCenter->LoadCardForm(strError))
		{
			strMessage = QString("加载卡片打印模板失败:%1").arg(strError);

			break;
		}
		nResult = 0;
	} while (0);

	if (nResult)
	{
		gError() << gQStr(strMessage);
		QMessageBox::critical(nullptr, "提示", strMessage, QMessageBox::Ok);
		return nResult;
	}

	RegionInfo& Reg = g_pDataCenter->GetSysConfigure()->Region;
	char szOutInfo[1024] = { 0 };
	CJsonObject jsonReg;
	jsonReg.Add("province", Reg.nProvinceCode);
	jsonReg.Add("user", Reg.strCMAccount);
	jsonReg.Add("pwd", Reg.strCMPassword);
	jsonReg.Add("city", Reg.strCityCode);
	string strJson = jsonReg.ToString();
	CJsonObject jsonT(strJson);
	qDebug() << strJson.c_str();
	int nProvince;
	string struser, strpwd, strcity;
	jsonT.Get("province", nProvince);
	jsonT.Get("user", struser);
	jsonT.Get("pwd", strpwd);
	jsonT.Get("city", strcity);

	WaitingProgress WaitingUI;
	WaitingUI.show();
	a.exec();
	if (!WaitingUI.bPrinterReady)
	{
		QMessageBox_CN(QMessageBox::Critical, "提示", "初始化打印机超时,请检查打印机是否已正常连接!", QMessageBox::Ok, &WaitingUI);
		return 0;
	}

	if (!CheckLocalLicense(Code_License))
	{
		ShowLicense s;
		s.show();
		return a.exec();
	}

	initCardInfo(jsonReg.ToString().c_str(), szOutInfo);
	MainWindow w;
	a.setActivationWindow(&w);

	QObject::connect(&a, &QtSingleApplication::messageReceived, &w, &MainWindow::OnNewInstance);

	auto listScreens = QApplication::screens();
	g_pCurScreen = listScreens.at(0);
	if (listScreens.size() > 1)		// 若多块屏幕，只在1080p的屏幕上显示
	{
		for (auto Screen : listScreens)
		{
			if (Screen->size().height() == 1080 && Screen->size().width() == 1920)
			{
				g_pCurScreen = Screen;
				break;
			}
		}
	}

	w.showFullScreen();
	w.setGeometry(g_pCurScreen->geometry());
	//w.showMaximized();
	//QRect size(1, 1, 1918, 1078);
	//w.setGeometry(size);
	//w.show();
	int nRes = a.exec();
	QString strInfo = "系统正常关闭!";
	gInfo() << gQStr(strInfo);
	google::ShutdownGoogleLogging();
	return nRes;
}
