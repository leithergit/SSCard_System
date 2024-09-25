#pragma execution_character_set("utf-8")
#include "mainwindow.h"
#include <QApplication>
#include <QString>
#include <QDesktopWidget>
#include <QScreen>
#include <fstream>
#include <QSharedMemory>
#include <QMutex>
#include "waitingprogress.h"
#include "DevBase.h"
#include "Gloabal.h"
#include "license.h"
#include "showlicense.h"
#include "BugTrap.h"
#include "../utility/json/CJsonObject.hpp"
#pragma comment(lib,"BugTrapU.lib")
extern QScreen* g_pCurScreen;
extern MainWindow* g_pMainWnd;

DataCenterPtr g_pDataCenter;
using namespace neb;
using namespace std;
const char* g_szPageOperation[4] =
{
	Str(Return_MainPage),
	Str(Stay_CurrentPage),
	Str(Switch_NextPage),
	//Str(Skip_NextPage)
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
	if (QFailed(g_pDataCenter->UpdatePrinterStatus(*pStrMessage)))
	{
		gInfo() << pStrMessage->toLocal8Bit().data();
		return -1;
	}
	return 0;
}

const wchar_t* g_pUniqueID = L"Global\\2F026B66-2CCA-40AB-AD72-435B5AC2E625";
HANDLE g_hAppMutex = nullptr;

bool g_bSkipLicense = false;

void TestJson()
{
	// Test Saving json
	CJsonObjectPtr pJson = make_shared<CJsonObject>();
	CJsonObjectPtr pSubJson = make_shared<CJsonObject>();
	pSubJson->Add("First", 1);
	pSubJson->Add("Second", 2);
	pSubJson->Add("Third", 15);
	pJson->Add("Progress", *pSubJson);
	qDebug() << pJson->ToFormattedString().c_str();
	if (pJson->KeyExist("Progress"))
	{
		CJsonObject pSubJson = (*pJson)["Progress"];
		int nFirst, nSecond, nThird;
		pSubJson.Get("First", nFirst);
		pSubJson.Get("Second", nSecond);
		pSubJson.Get("Third", nThird);
	}
	g_pDataCenter->SaveProgress(pJson, "d:/SSCard_System/data/t.json");
}
#include <QTextCodec>
void InitCodec()
{
#if (QT_VERSION <= QT_VERSION_CHECK(5,0,0))
#if _MSC_VER
	QTextCodec* codec = QTextCodec::codecForName("GBK");
#else
	QTextCodec* codec = QTextCodec::codecForName("UTF-8");
#endif
	QTextCodec::setCodecForLocale(codec);
	QTextCodec::setCodecForCStrings(codec);
	QTextCodec::setCodecForTr(codec);
#else
	QTextCodec* codec = QTextCodec::codecForName("GBK");
	QTextCodec::setCodecForLocale(codec);
#endif
}
#include <mbctype.h>


#include <QJsonDocument>  
#include <QJsonObject>  
#include <QJsonArray>  
#include <QFile>  
#include <QDebug>  

void printJsonKeyValue(const QJsonValue& value, const QString& key, const QString& prefix = "")
{
	QString fullKey = prefix.isEmpty() ? key : prefix + "." + key;

	switch (value.type()) {
	case QJsonValue::Null:
		qDebug() << fullKey << ": null";
		break;
	case QJsonValue::Bool:
		qDebug() << fullKey << ":" << (value.toBool() ? "true" : "false");
		break;
	case QJsonValue::Double:
		qDebug() << fullKey << ":" << value.toDouble();
		break;
	case QJsonValue::String:
		qDebug() << fullKey << ":" << value.toString();
		break;
	case QJsonValue::Array:
	{
		qDebug() << fullKey << ": [Array]";
		QJsonArray arr = value.toArray();
		for (int i = 0; i < arr.size(); ++i) {
			printJsonKeyValue(arr[i], QString("[%1]").arg(i), fullKey);
		}
		break;
	}
	case QJsonValue::Object:
	{
		qDebug() << fullKey << ": {Object}";
		QJsonObject obj = value.toObject();
		for (auto it = obj.begin(); it != obj.end(); ++it) {
			printJsonKeyValue(it.value(), it.key(), fullKey);
		}
		break;
	}
	default:
		qDebug() << fullKey << ": Unknown type";
	}
}

bool readAndPrintJson(const QString& filePath)
{
	QFile file(filePath);
	if (!file.open(QIODevice::ReadOnly)) {
		qWarning() << "Could not open file:" << filePath;
		return false;
	}

	QByteArray jsonData = file.readAll();
	file.close();

	QJsonDocument document = QJsonDocument::fromJson(jsonData);
	if (document.isNull()) {
		qWarning() << "Failed to parse JSON";
		return false;
	}

	if (document.isObject()) {
		QJsonObject obj = document.object();
		for (auto it = obj.begin(); it != obj.end(); ++it) {
			printJsonKeyValue(it.value(), it.key());
		}
	}
	else if (document.isArray()) {
		QJsonArray arr = document.array();
		for (int i = 0; i < arr.size(); ++i) {
			printJsonKeyValue(arr[i], QString("[%1]").arg(i));
		}
	}
	else {
		qWarning() << "Document is neither an object nor an array";
		return false;
	}

	return true;
}
int main(int argc, char* argv[])
{
	//TestJson();
// 	QFile jsfile("D:/Work/SSCard_System.main/MainProject/现场日志/Testoriginal.json");
// 	if (jsfile.open(QIODevice::ReadOnly))
// 	{
// 		QByteArray baJson = jsfile.readAll();
// 		QJsonParseError jsonParseError;
// 		QJsonDocument jsonDocument(QJsonDocument::fromJson(baJson.data(), &jsonParseError));
// 		if (QJsonParseError::NoError != jsonParseError.error)
// 		{
// 			QString strMessage = QString("JsonParseError: %1").arg(jsonParseError.errorString());
// 			gInfo() << gQStr(strMessage);
// 			return -1;
// 		}
// 		QJsonObject rootObject = jsonDocument.object();
// 
// 		if (!rootObject.keys().contains("msg") ||
// 			!rootObject.keys().contains("code"))
// 		{
// 			gInfo() << "There is no msg or code field in the respond!";
// 			return -1;
// 		}
// 		
// 
// 		QString strCode = rootObject.value("code").toString();
// 		int nCode = strCode.toInt();
// 		QString strMsg = rootObject.value("msg").toString();
// 		qDebug() << "jsCode = " << strCode;
// 		qDebug() << "jsMsg = " << strMsg;
// 		
// 		if (nCode == -1 &&
// 			strMsg.contains("一个月内有补卡缴费成功记录,不能获取缴款码"))
// 		{
// 			return 1;
// 		}
// 	}
	
	SetupExceptionHandler();
	QMutex t;
	if (OpenMutexW(MUTEX_ALL_ACCESS, FALSE, g_pUniqueID))
		return 0;
	g_hAppMutex = CreateMutexW(nullptr, TRUE, g_pUniqueID);
	shared_ptr<void> FreeMutex(g_hAppMutex, CloseHandle);
	//qputenv("QT_ENABLE_HIGHDPI_SCALING", "1");
	QGuiApplication::setHighDpiScaleFactorRoundingPolicy(Qt::HighDpiScaleFactorRoundingPolicy::PassThrough);
	QGuiApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
	QApplication a(argc, argv);
	/*int nR =_setmbcp(CP_ACP)

	char* pResult = setlocale(LC_ALL, "chinese-simplified");
	SetThreadLocale(MAKELCID(MAKELANGID(LANG_CHINESE, SUBLANG_CHINESE_SIMPLIFIED), SORT_DEFAULT));
	char szError[1024] = { 0 };
	SetConsoleOutputCP(936);
	int len = FormatMessageA(FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS, NULL, 10060, MAKELANGID(LANG_CHINESE, SUBLANG_CHINESE_SIMPLIFIED), (LPSTR)szError, 1024, NULL);*/
	//InitCodec();
#ifdef _DEBUG
	QStringList strArgList = a.arguments();
	for (auto var : strArgList)
	{
		if (var.compare("/SkipLicense", Qt::CaseInsensitive) == 0)
		{
			g_bSkipLicense = true;
			break;
		}
	}
#endif // DEBUG

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
	FLAGS_logbufsecs = 0;	// 无迟延写入
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

	DeviceConfig& devInfo = g_pDataCenter->GetSysConfigure()->DevConfig;
	RegionInfo& Reg = g_pDataCenter->GetSysConfigure()->Region;
	char szOutInfo[1024] = { 0 };
	CJsonObject jsonReg;
	jsonReg.Add("province", Reg.nProvinceCode);
	jsonReg.Add("user", Reg.strCMAccount);
	jsonReg.Add("pwd", Reg.strCMPassword);
	jsonReg.Add("city", Reg.strCityCode);
	jsonReg.Add("TerminatorID", devInfo.strTerminalCode);
	jsonReg.Add("OperatorID", devInfo.strOperatorID);
	string strJson = jsonReg.ToString();
	//string strJsonPath = "./Data/Progress_411624200409164543.json";
	//readAndPrintJson(strJsonPath.c_str());

	//SSCardInfoPtr pSSCardInfo = make_shared<SSCardInfo>();
	//g_pDataCenter->LoadSSCardData(strJsonPath, pSSCardInfo);

	//////////////////////////////////////////////////////////////////////////
	// 引入山东授权管理代码

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
		if (!CheckLocalLicense(Code_License))
		{
			ShowLicense s;
			s.show();
			return a.exec();
		}
	}
	//////////////////////////////////////////////////////////////////////////
	// 引入山东授权管理代码结束

	// 原河南授权管理代码
	//WaitingProgress WaitingUI;
	//WaitingUI.show();
	//a.exec();
	//if (!WaitingUI.bPrinterReady)
	//{
	//	QMessageBox_CN(QMessageBox::Critical, "提示", "初始化打印机超时,请检查打印机是否已正常连接!", QMessageBox::Ok, &WaitingUI);
	//	return 0;
	//}

	//if (!CheckLocalLicense(Code_License))
	//{
	//	ShowLicense s;
	//	s.show();
	//	return a.exec();
	//}
	// 原河南授权管理代码结束

	initCardInfo(jsonReg.ToString().c_str(), szOutInfo);
	MainWindow w;
	//a.setActivationWindow(&w);
	//QObject::connect(&a, &QtSingleApplication::messageReceived, &w, &MainWindow::OnNewInstance);

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
	//w.showMaximized();	// 会导致窗口无法覆盖任务栏
	g_pMainWnd = &w;
	int nRes = a.exec();
	QString strInfo = "系统正常关闭!";
	gInfo() << gQStr(strInfo);
	google::ShutdownGoogleLogging();
	return nRes;
}
