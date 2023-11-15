#pragma execution_character_set("utf-8")
#include "mainwindow.h"
#include <QObject>
#include <QApplication>
#include <QString>
#include <QDesktopWidget>
#include <QScreen>
#include <fstream>
#include <sstream>
#include <QSharedMemory>
#include <QSplashScreen>
#ifdef _DEBUG
#include <windows.h>
#endif
#include "DevBase.h"
#include "Gloabal.h"
#include "license.h"
#include "showlicense.h"
#include "BugTrap.h"
#include "../utility/json/CJsonObject.hpp"
#include "../Utility/Markup.h"
#include "waitingprogress.h"
#include "dialogconfigbank.h"

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

//int WaitForDebugEvent()
//{
//	while (true)
//	{
//		Sleep(10000);
//	}
//	return 0;
//}

int TestPrinter(void* pParam)
{
	if (!pParam)
		return -1;
	int nResult = -1;
	QString* pStrMessage = (QString*)pParam;
	if (QFailed(nResult = g_pDataCenter->OpenPrinter(*pStrMessage)))
	{
		gInfo() << pStrMessage->toLocal8Bit().data();
		return -1;
	}
	else
		return 0;
}

void resizeChildren(QWidget* parent, double scale);

#define WSA_VERSION MAKEWORD(2,2)
const wchar_t* g_pUniqueID = L"Global\\2F026B66-2CCA-40AB-AD72-435B5AC2E625";
HANDLE g_hAppMutex = nullptr;

bool g_bSkipLicense = false;


int main(int argc, char* argv[])
{
#ifdef _DEBUG
	//WaitForDebugEvent();
#endif
	SetupExceptionHandler();
	CurlInitializer curlInit;
	if (OpenMutexW(MUTEX_ALL_ACCESS, FALSE, g_pUniqueID))
		return 0;
	g_hAppMutex = CreateMutexW(nullptr, TRUE, g_pUniqueID);
	shared_ptr<void> FreeMutex(g_hAppMutex, CloseHandle);
	//qputenv("QT_ENABLE_HIGHDPI_SCALING", "1");
	QGuiApplication::setHighDpiScaleFactorRoundingPolicy(Qt::HighDpiScaleFactorRoundingPolicy::PassThrough);
	QGuiApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
	//QCoreApplication::setAttribute(Qt::AA_UseHighDpiPixmaps);

	QApplication a(argc, argv);
#ifdef _DEBUG
	QStringList strArgList = a.arguments();
	for (auto var : strArgList)
	{
		if (var.compare("/OverviewLicense", Qt::CaseInsensitive) == 0)
		{
			g_bSkipLicense = true;
			break;
		}
	}
#endif // DEBUG


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
	FLAGS_logbufsecs = 0;
	google::SetLogDestination(google::GLOG_INFO, strLogDatePath.toLocal8Bit().data());	// 使用该设定时，默认情况下，所有级别日志都用同一的文件名
	google::SetStderrLogging(google::GLOG_INFO);	// 大于该级别的日志输出到stderr
	google::SetLogFilenameExtension(".log");
	google::InitGoogleLogging(strLogDatePath.toLocal8Bit().data());

	WSADATA		WSAData = { 0 };
	if (WSAStartup(WSA_VERSION, &WSAData))
	{
		if (LOBYTE(WSAData.wVersion) != LOBYTE(WSA_VERSION) ||
			HIBYTE(WSAData.wVersion) != HIBYTE(WSA_VERSION))
		{
			WSACleanup();
			gInfo() << "Failed in initialize winsock 2.0!";
		}
	}

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
			if (lastStartTime.isValid())
			{
				int nDiffSecond = lastStartTime.secsTo(Now);
				if (nDiffSecond < 0)
					nRunCount = 1;
				else if (nDiffSecond <= 1)
				{
					if (nRunCount >= 3)
						return 0;
					else
						nRunCount++;
				}
			}
			else
			{
				nRunCount = 1;
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
		gError() << QString("加载卡片打印模板失败:%1").arg(strError).toLocal8Bit().data();
		return -1;
	}

	if (!g_pDataCenter->LoadBankCode(strError))
	{
		gError() << gQStr(strError);
		return -1;
	}

	if (!g_pDataCenter->InitializeDB(strError))
	{
		gError() << gQStr(strError);
		return -1;
	}
	QString strMessage;

	if (!g_bSkipLicense)
	{
		WaitingProgress WaitingUI(TestPrinter,		// 执行回调
			&strMessage,		// 参数
			g_pDataCenter->GetSysConfigure()->nTimeWaitForPrinter,	// 超时
			QString("正在初始化打印机:%1%"));	// 显示格式


		if (WaitingUI.exec() == QDialog::Rejected)
		{
			QMessageBox_CN(QMessageBox::Critical, "提示", "初始化打印机超时,请检查打印机是否已正常连接!", QMessageBox::Ok, &WaitingUI);
			return 0;
		}
		//桌面版
		QDateTime current = QDateTime::currentDateTime();
		QDateTime deadline(QDate(2024, 2, 1));
		if (current > deadline)
		{
			if (!CheckLocalLicense(Code_License))
			{
				ShowLicense s;
				s.show();
				return a.exec();
			}
		}
	}

	string strBankName;
	if (QFailed(g_pDataCenter->CheckBankCode(strBankName, strMessage)))
	{
		DialogConfigBank dlg;
		if (dlg.exec() != QDialog::Accepted)
		{
			gError() << GBKString("放弃配置银行代码!");
			return 0;
		}
	}

	MainWindow w;

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
	//double scale = 0.7;
	//w.resize(w.size() * scale);
	//resizeChildren(&w, scale);

	w.show();
	//w.showMaximized();		// 会导致窗口无法覆盖任务栏
	int nRes = a.exec();
	QString strInfo = "系统正常关闭!";
	gInfo() << gQStr(strInfo);
	google::ShutdownGoogleLogging();
	return nRes;
}

void resizeChildren(QWidget* parent, double scale)
{
	for(QObject * obj:parent->children())
	{
		QWidget* widget = qobject_cast<QWidget*>(obj);
		if (widget)
		{
			widget->resize(widget->size() * scale);
			QPoint pos = widget->pos();
			pos *= scale;
			widget->move(pos);
			resizeChildren(widget, scale);
		}
	}
}