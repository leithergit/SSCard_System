#pragma execution_character_set("utf-8")
#include <QDir>
#include <QApplication>
#include <QCoreApplication>
#include <QFileInfo>
#include <QTextCodec>
#include "Gloabal.h"
#include "Payment.h"
#include "qstackpage.h"
#include "SimpleIni.h"
#include <QSqlError>


#pragma comment(lib,"user32.lib")
#pragma comment(lib,"advapi32.lib")
#pragma comment(lib,"OleAut32.lib")
#pragma comment(lib,"Version.lib")
#pragma comment(lib,"../sdk/FaceCapture/DVTGKLDCamSDK.lib")
#pragma comment(lib,"../sdk/BugTrap/BugTrapU.lib")
#ifdef _DEBUG
#pragma comment(lib, "../SDK/KT_Printer/KT_Printerd")
#pragma comment(lib, "../SDK/KT_Reader/KT_Readerd")
#pragma comment(lib, "../SDK/SSCardDriver/SSCardDriverd")
#pragma comment(lib, "../SDK/SSCardHSM/SSCardHSMd")
#pragma comment(lib, "../SDK/SSCardInfo_Henan/SSCardInfod")
#pragma comment(lib, "../SDK/libcurl/libcurl")
#pragma comment(lib, "../SDK/QREncode/qrencoded")
#pragma comment(lib, "../SDK/IDCard/IDCard_API")
#pragma comment(lib, "../SDK/glog/glogd")
#pragma comment(lib, "../SDK/PinKeybroad/XZ_F31_API")
#pragma comment(lib, "../SDK/7Z/lib/bit7z_d.lib")
#pragma comment(lib,"../SSCard_System/Debug/Update")
#else
#pragma comment(lib, "../SDK/KT_Printer/KT_Printer")
#pragma comment(lib, "../SDK/KT_Reader/KT_Reader")
#pragma comment(lib, "../SDK/SSCardDriver/SSCardDriver")
#pragma comment(lib, "../SDK/SSCardHSM/SSCardHSM")
#pragma comment(lib, "../SDK/SSCardInfo_Henan/SSCardInfo")
#pragma comment(lib, "../SDK/libcurl/libcurl")
#pragma comment(lib, "../SDK/QREncode/qrencode")
#pragma comment(lib, "../SDK/IDCard/IDCard_API")
#pragma comment(lib, "../SDK/glog/glog")
#pragma comment(lib, "../SDK/PinKeybroad/XZ_F31_API")
#pragma comment(lib, "../SDK/7Z/lib/bit7z.lib")
#pragma comment(lib,"../SSCard_System/release/Update")
#endif
#define PORT_NUM__ 100
const char* szPrinterTypeList[PRINTER_MAX] =
{
	Str(EVOLIS_KC200),
	Str(EVOLIS_ZENIUS),
	Str(EVOLIS_AVANSIA),
	Str(HITI_CS200),
	Str(HITI_CS220),
	Str(HITI_CS290),
	Str(ENTRUCT_EM1),
	Str(ENTRUCT_EM2),
	Str(ENTRUCT_CD809)
};

const char* szReaderTypeList[READER_MAX + 1] =
{
	Str(IN_VALID),
	Str(DC_READER),//德卡读卡器
	Str(MH_READER),//明华读卡器
	Str(HD_READER),//华大读卡器
};
QWaitCursor::QWaitCursor()
{
	QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
	bStart = true;
}
void QWaitCursor::RestoreCursor()
{
	QApplication::restoreOverrideCursor();
	bStart = false;
}
QWaitCursor::~QWaitCursor()
{
	if (bStart)
		QApplication::restoreOverrideCursor();
}


extern DataCenterPtr g_pDataCenter;

HWND hTouchKeybroad = nullptr;

QScreen* g_pCurScreen = nullptr;
vector<NationaltyCode> g_vecNationCode = {
			{"01", "汉族"},
			{ "02","蒙古族" },
			{ "03","回族" },
			{ "04","藏族" },
			{ "05","维吾尔族" },
			{ "06","苗族" },
			{ "07","彝族" },
			{ "08","壮族" },
			{ "09","布依族" },
			{ "10","朝鲜族" },
			{ "11","满族" },
			{ "12","侗族" },
			{ "13","瑶族" },
			{ "14","白族" },
			{ "15","土家族" },
			{ "16","哈尼族" },
			{ "17","哈萨克族" },
			{ "18","傣族" },
			{ "19","黎族" },
			{ "20","傈傈族" },
			{ "21","佤族" },
			{ "22","畲族" },
			{ "23","高山族" },
			{ "24","拉祜族" },
			{ "25","水族" },
			{ "26","东乡族" },
			{ "27","纳西族" },
			{ "28","景颇族" },
			{ "29","柯尔克孜族" },
			{ "30","土族" },
			{ "31","达翰尔族" },
			{ "32","仫佬族" },
			{ "33","羌族" },
			{ "34","布朗族" },
			{ "35","撒拉族" },
			{ "36","毛南族" },
			{ "37","仡佬族" },
			{ "38","锡伯族" },
			{ "39","阿昌族" },
			{ "40","普米族" },
			{ "41","塔吉克族" },
			{ "42","怒族" },
			{ "43","乌孜别克族" },
			{ "44","俄罗斯族" },
			{ "45","鄂温克族" },
			{ "46","德昂族" },
			{ "47","保安族" },
			{ "48","裕固族" },
			{ "49","京族" },
			{ "50","塔塔尔族" },
			{ "51","独龙族" },
			{ "52","鄂伦春族" },
			{ "53","赫哲族" },
			{ "54","门巴族" },
			{ "55","珞巴族" },
			{ "56","基诺族" },
			{ "57","穿青人" },
			{ "90","外籍人士" },
			{ "99","其他" }
};

vector<DocType> g_vecDocCode ={
	{"A","居民身份证（户口簿）"},
	{"C","护照"},
	{"D","外国人永久居留证"},
	{"J","港澳居民来往内地通行证"},
	{"K","台湾居民往来大陆通行证"},
	{"M","港澳台居民居住证"},
	{"JH","香港特区护照/港澳居民来往内地通行证"},
	{"JM","澳门特区护照/港澳居民来往内地通行证"}
};

vector<HukouCode> g_vecHukouCode = {
	{"10","非农业户口"},
	{"11","本地非农业户口（本地城镇）"},
	{"12","外地非农业户口（外地城镇）"},
	{"20","农业户口"},
	{"21","本地农业户口（本地农村）"},
	{"22","外地农村户口（外地农村）"},
	{"30","港澳台侨人员"},
	{"31","香港特别行政区居民"},
	{"32","澳门特别行政区居民"},
	{"33","台湾地区居民"},
	{"34","华侨"},
	{"40","外籍人员"},
	{"41","未取得永久居留权的外国人"},
	{"42","取得永久居留权的外国人"},
	{"50","居民户口（含农业户口、非农业户口）"},
	{"51","本地居民户口"},
	{"52","外地居民户口"}
};

vector<EductionCode> g_vecEducationCode = {
	{"10","研究生及以上"},
	{"11","博士研究生"},
	{"12","博士后"},
	{"14","硕士研究生"},
	{"20","大学本科"},
	{"30","大学专科"},
	{"40","中专技校"},
	{"41","中等专科"},
	{"44","职业高中"},
	{"47","技工学校"},
	{"60","普通高中"},
	{"70","初中及以下"},
	{"71","初级中学"},
	{"81","小学"},
	{"81","其他"}
};

vector<GuojiCode> g_vecGuojiCode = {
	{"ABW","阿鲁巴岛"},
	{"AFG","阿富汗"},
	{"AGO","安哥拉"},
	{"AIA","安圭拉"},
	{"ALA","奥兰群岛"},
	{"ALB","阿尔巴尼亚"},
	{"AND","安道尔"},
	{"ANT","荷属安地列斯群岛"},
	{"ARE","阿拉伯联合酋长国"},
	{"ARG","阿根廷"},
	{"ARM","亚美尼亚"},
	{"ASM","美属萨摩亚"},
	{"ATA","南极洲"},
	{"ATF","法属南部领地"},
	{"ATG","安提瓜岛与巴布达岛"},
	{"AUS","澳大利亚"},
	{"AUT","奥地利"},
	{"AZE","阿塞拜疆"},
	{"BDI","布隆迪"},
	{"BEL","比利时"},
	{"BEN","贝宁"},
	{"BFA","布基纳法索"},
	{"BGD","孟加拉国"},
	{"BGR","保加利亚"},
	{"BHR","巴林渠道"},
	{"BHS","巴哈马群岛"},
	{"BIH","波斯尼亚与黑塞哥维那"},
	{"BLR","白俄罗斯"},
	{"BLZ","伯利兹"},
	{"BMU","百慕大群岛"},
	{"BOL","玻利维亚"},
	{"BRA","巴西"},
	{"BRB","巴巴多斯岛"},
	{"BRN","文莱"},
	{"BTN","不丹"},
	{"BVT","博维特岛"},
	{"BWA","博茨瓦纳"},
	{"CAF","中非共和国"},
	{"CAN","加拿大"},
	{"CCK","科科斯群岛"},
	{"CHE","瑞士"},
	{"CHL","智利"},
	{"CHN","中国"},
	{"CIV","科特迪瓦"},
	{"CMR","喀麦隆"},
	{"COD","刚果（金）"},
	{"COG","刚果"},
	{"COK","库克群岛"},
	{"COL","哥伦比亚"},
	{"COM","科摩罗"},
	{"CPV","佛得角"},
	{"CRI","哥斯达黎加"},
	{"CUB","古巴"},
	{"CXR","圣诞岛"},
	{"CYM","开曼群岛"},
	{"CYP","塞浦路斯"},
	{"CZE","捷克共和国"},
	{"DEU","德国"},
	{"DJI","吉布提"},
	{"DMA","多米尼加"},
	{"DNK","丹麦"},
	{"DOM","多米尼加共和国"},
	{"DZA","阿尔及尔"},
	{"ECU","厄瓜多尔"},
	{"EGY","埃及"},
	{"ERI","厄立特里亚"},
	{"ESH","西撒哈拉"},
	{"ESP","西班牙"},
	{"EST","爱沙尼亚"},
	{"ETH","埃塞俄比亚"},
	{"FIN","芬兰"},
	{"FJI","斐济"},
	{"FRA","法国"},
	{"FRO","法罗群岛"},
	{"FSM","密克罗尼西亚"},
	{"GAB","加蓬"},
	{"GBR","英国"},
	{"GEO","乔治亚州"},
	{"GGY","格恩西岛"},
	{"GHA","加纳"},
	{"GIB","直布罗陀"},
	{"GIN","几内亚"},
	{"GLP","瓜德罗普岛"},
	{"GMB","冈比亚"},
	{"GNB","几内亚比绍"},
	{"GNQ","赤道几内亚"},
	{"GRC","希腊"},
	{"GRD","格林纳达"},
	{"GRL","格陵兰"},
	{"GTM","危地马拉"},
	{"GUF","法属圭亚那"},
	{"GUM","关岛"},
	{"GUY","圭亚那"},
	{"HKG","中国香港"},
	{"HMD","赫德和麦克唐纳群岛"},
	{"HND","洪都拉斯"},
	{"HRV","克罗地亚"},
	{"HTI","海地"},
	{"HUN","匈牙利"},
	{"IDN","印度尼西亚"},
	{"IMN","曼恩岛"},
	{"IND","印度"},
	{"IOT","英属印度洋领地"},
	{"IRL","爱尔兰"},
	{"IRN","伊朗"},
	{"IRQ","伊拉克"},
	{"ISL","冰岛"},
	{"ISR","以色列"},
	{"ITA","意大利"},
	{"JAM","牙买加"},
	{"JEY","泽西岛"},
	{"JOR","约旦"},
	{"JPA","日本"},
	{ "JPN","日本" },
	{ "KAZ","哈萨克斯坦" },
	{ "KEN","肯尼亚" },
	{ "KGZ","吉尔吉斯斯坦" },
	{ "KHM","柬埔寨" },
	{ "KIR","基里巴斯" },
	{ "KLK","福克兰群岛" },
	{ "KNA","圣基茨和尼维斯" },
	{ "KOR","韩国" },
	{ "KWT","科威特" },
	{ "LAO","老挝" },
	{ "LBN","黎巴嫩" },
	{ "LBR","利比里亚" },
	{ "LBY","利比亚" },
	{ "LCA","圣卢西亚岛" },
	{ "LIE","列支敦士登" },
	{ "LKA","斯里兰卡" },
	{ "LSO","莱索托" },
	{ "LTU","立陶宛" },
	{ "LUX","卢森堡" },
	{ "LVA","拉脱维亚" },
	{ "MAC","中国澳门" },
	{ "MAR","摩洛哥" },
	{ "MCO","摩纳哥" },
	{ "MDA","摩尔多瓦" },
	{ "MDG","马达加斯加" },
	{ "MDV","马尔代夫" },
	{ "MEX","墨西哥" },
	{ "MHL","马绍尔群岛" },
	{ "MKD","马其顿" },
	{ "MLI","马里" },
	{ "MLT","马耳他" },
	{ "MMR","缅甸" },
	{ "MNE","黑山" },
	{ "MNG","蒙古" },
	{ "MNP","北马里亚纳群岛自由联邦" },
	{ "MOZ","莫桑比克" },
	{ "MRT","毛里塔尼亚" },
	{ "MSR","蒙特塞拉特岛" },
	{ "MTQ","马提尼克岛" },
	{ "MUS","毛里求斯" },
	{ "MWI","马拉维" },
	{ "MYS","马来西亚" },
	{ "MYT","马约特岛" },
	{ "NAM","纳米比亚" },
	{ "NCL","新喀里多尼亚" },
	{ "NER","尼日尔" },
	{ "NFK","诺福克岛" },
	{ "NGA","尼日利亚" },
	{ "NIC","尼加拉瓜" },
	{ "NIU","纽埃岛" },
	{ "NLD","荷兰" },
	{ "NOR","挪威" },
	{ "NPL","尼泊尔" },
	{ "NRU","瑙鲁" },
	{ "NZL","新西兰" },
	{ "OMN","阿曼" },
	{ "PAK","巴基斯坦" },
	{ "PAN","巴拿马" },
	{ "PCN","皮克特恩岛" },
	{"PER","秘鲁"},
	{ "PHL","菲律宾" },
	{ "PLW","帕劳岛" },
	{ "PNG","巴布亚新几内亚" },
	{ "POL","波兰" },
	{ "PRI","波多黎各" },
	{ "PRK","朝鲜" },
	{ "PRT","葡萄牙" },
	{ "PRY","巴拉圭" },
	{ "PSE","巴勒斯坦" },
	{ "PYF","法属波利尼西亚" },
	{ "QAT","卡塔尔" },
	{ "REU","留尼旺岛" },
	{ "ROU","罗马尼亚" },
	{ "RUS","俄国" },
	{ "RWA","卢旺达" },
	{ "SAU","沙特阿拉伯" },
	{ "SDN","苏丹" },
	{ "SEN","塞内加尔" },
	{ "SGP","新加坡" },
	{ "SGS","南乔治亚和南桑威奇群岛" },
	{ "SHN","圣赫勒拿岛" },
	{ "SJM","斯瓦尔巴岛和扬马延岛" },
	{ "SLB","所罗门群岛" },
	{ "SLE","塞拉利昂" },
	{ "SLV","萨尔瓦多" },
	{ "SMR","圣马力诺" },
	{ "SOM","索马里" },
	{ "SPM","圣皮埃尔和密克隆岛" },
	{ "SRB","塞尔维亚和黑山" },
	{ "STP","圣多美和普林西比" },
	{ "SUR","苏里南" },
	{ "SVK","斯洛伐克" },
	{ "SVN","斯洛文尼亚" },
	{ "SWE","瑞典" },
	{ "SWZ","斯威士兰" },
	{ "SYC","塞舌尔" },
	{ "SYR","叙利亚" },
	{ "TCA","特克斯和凯利克斯群岛" },
	{ "TCD","乍得" },
	{ "TGO","多哥" },
	{ "THA","泰国" },
	{ "TJK","塔吉克斯坦" },
	{ "TKL","托克劳群岛" },
	{ "TKM","土库曼斯坦" },
	{ "TLS","东帝汶" },
	{ "TON","汤加" },
	{ "TTO","特立尼达和多巴哥" },
	{ "TUN","突尼斯" },
	{ "TUR","土耳其" },
	{ "TUV","图瓦卢" },
	{ "TWN","中国台湾" },
	{ "TZA","坦桑尼亚" },
	{ "UGA","乌干达" },
	{ "UKR","乌克兰" },
	{ "UMI","美属萨摩亚" }, 
	{ "URY","乌拉圭" },
	{ "USA","美国" },
	{ "UZB","乌兹别克斯坦" },
	{ "VAT","梵蒂冈" },
	{ "VCT","圣文森特和格林纳丁斯群岛" },
	{ "VEN","委内瑞拉" },
	{ "VGB","维尔京群岛（英属）" },
	{ "VIR","维尔京群岛（美属）" },
	{ "VNM","越南" },
	{ "VUT","瓦努阿图" },
	{ "WLF","瓦利斯群岛和富图纳群岛" },
	{ "WSM","萨摩亚群岛" },
	{ "YEM","也门" },
	{ "ZAF","南非" },
	{ "ZMB","赞比亚" },
	{ "ZWE","津巴布韦" }
};
vector<MarrageCode> g_vecMarriageCode = {
	{"1","未婚"},
	{"2","已婚"},
	{"3","丧偶"},
	{"4","离婚"},
	{"5","其他"}
};

vector<_CareerType> vecCareer = {
						{"1000000","单位负责人"},
						{"2000000","专业、技术人员"},
						{"3000000","办事人员"},
						{"4000000","商业、服务人员"},
						{"5000000","农林牧渔水利生产人员"},
						{"6000000","生产运输工人"},
						{"8000000","其他从业人员"} };



bool DataCenter::LoadBankCode(QString& strError)
{
	try
	{
		gInfo() << "Try to load BankCode.json";
		QString strBankFile = QCoreApplication::applicationDirPath();
		strBankFile += "/Data/BankCode.json";
		QFileInfo fi(strBankFile);
		if (!fi.isFile())
		{
			strError = QString("找不到银行代码库文件:%1!").arg(strBankFile);
			return false;
		}

		ifstream ifs(strBankFile.toStdString(), ios::in);
		stringstream ss;
		ss << ifs.rdbuf();
		CJsonObject json;
		auto tNow = chrono::high_resolution_clock::now();
		if (!json.Parse(ss.str()))
		{
			strError = QString("无效的银行代码库:1%").arg(strBankFile);
			return false;
		}
		if (!json.KeyExist("BankList"))
		{
			strError = "银行代码库文件被损坏!";
			return false;
		}
		CJsonObject jsonBankList = json["BankList"];
		if (!jsonBankList.IsArray())
		{
			strError = "银行代码库文件被损坏!";
			return false;
		}
		auto tDuration = chrono::duration_cast<chrono::milliseconds>(chrono::high_resolution_clock::now() - tNow);

		cout << "Parsar duration:" << tDuration.count() << endl;
		tNow = chrono::high_resolution_clock::now();
		int nArraySize = jsonBankList.GetArraySize();
		CJsonObject jsonItem;
		for (int i = 0; i < nArraySize; i++)
		{
			if (!jsonBankList.Get(i, jsonItem))
				break;
			string strCode = jsonItem("code");
			string strName = jsonItem("Name");
			auto [it, Inserted] = mapBankCode.try_emplace(strCode, strName);
		}

		tDuration = chrono::duration_cast<chrono::milliseconds>(chrono::high_resolution_clock::now() - tNow);

		cout << "Parsar duration:" << tDuration.count() << endl;
		tNow = chrono::high_resolution_clock::now();
		//for (auto var : mapBankCode)
		//{
		//	cout << var.first << "-->" << var.second;
		//}
		return true;
	}
	catch (std::exception& e)
	{
		cout << "exception:" << e.what();
	}
	return false;
}

DataCenter::DataCenter()
{
	DVTGKLDCam_Init();

}

DataCenter::~DataCenter()
{
	DVTGKLDCam_UnInit();
}
// 
int DataCenter::LoadSysConfigure(QString& strError)
{
	try
	{
		CloseDevice();
		gInfo() << "Try to load configure.ini";
		QString strConfigPath = QCoreApplication::applicationDirPath();
		strConfigPath += "/Configure.ini";
		QFileInfo fi(strConfigPath);
		if (!fi.isFile())
		{
			strError = QString("加载配置文件失败:%1!").arg(strConfigPath);
			return -1;
		}
		CSimpleIniA ConfigIni;
		if (ConfigIni.LoadFile(strConfigPath.toLocal8Bit().data()) != SI_OK)
		{
			strError = QString("加载配置文件失败:%1!").arg(strConfigPath);
			return -1;
		}
		string strPrinter = ConfigIni.GetValue("Device", "Printer");
		//QSettings ConfigIni(strConfigPath, QSettings::IniFormat);
		pSysConfig = make_shared<SysConfig>(&ConfigIni);
		if (!pSysConfig)
		{
			strError = "内存不足，无法初始化DataCenter类实例!";
			return -1;
		}
		bDebug = GetSysConfigure()->bDebug;
		bEnableUpdate = GetSysConfigure()->bEnableUpdate;
		bSkipWriteCard = GetSysConfigure()->bSkipWriteCard;
		bSkipPrintCard = GetSysConfigure()->bSkipPrintCard;
		nNetTimeout = GetSysConfigure()->nNetTimeout;
		bTestCard = GetSysConfigure()->bTestCard;
		bPreSetBankCard = GetSysConfigure()->bPreSetBankCard;
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
		CSimpleIniA ConfigIni;
		ConfigIni.LoadFile(strConfigPath.toStdString().c_str());
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

int QMessageBox_CN(QMessageBox::Icon nIcon, QString strTitle, QString strText, QMessageBox::StandardButtons stdButtons, QWidget* parent)
{
	QMessageBox Msgbox(nIcon, strTitle, strText, stdButtons, parent);
	Msgbox.setStandardButtons(stdButtons);
	struct ButtonInfo
	{
		QMessageBox::StandardButton nBtn;
		QString     strText;
	};

	static std::map<QMessageBox::StandardButton, QString>  ButtonMap =
	{
		{QMessageBox::NoButton          ,""},
		{QMessageBox::Ok                ,QObject::tr("确定")},
		{QMessageBox::Save              ,QObject::tr("保存")},
		{QMessageBox::SaveAll           ,QObject::tr("全部保存")},
		{QMessageBox::Open              ,QObject::tr("打开")},
		{QMessageBox::Yes               ,QObject::tr("是")},
		{QMessageBox::YesToAll          ,QObject::tr("所有都是")},
		{QMessageBox::No                ,QObject::tr("否")},
		{QMessageBox::NoToAll           ,QObject::tr("所有都不")},
		{QMessageBox::Abort             ,QObject::tr("中止")},
		{QMessageBox::Retry             ,QObject::tr("重试")},
		{QMessageBox::Ignore            ,QObject::tr("忽略")},
		{QMessageBox::Close             ,QObject::tr("关闭")},
		{QMessageBox::Cancel            ,QObject::tr("取消")},
		{QMessageBox::Discard           ,QObject::tr("放弃")},
		{QMessageBox::Help              ,QObject::tr("帮忙")},
		{QMessageBox::Apply             ,QObject::tr("应用")},
		{QMessageBox::Reset             ,QObject::tr("复位")},
		{QMessageBox::RestoreDefaults   ,QObject::tr("恢复默认")},
		{QMessageBox::FirstButton       ,QObject::tr("首个按钮")},
		{QMessageBox::LastButton        ,QObject::tr("末尾按钮")},
		{QMessageBox::YesAll            ,QObject::tr("全是")},
		{QMessageBox::NoAll             ,QObject::tr("全否")},
		{QMessageBox::Default           ,QObject::tr("默认")},
		{QMessageBox::Escape            ,QObject::tr("取消操作")},
		{QMessageBox::FlagMask          ,""},
		{QMessageBox::ButtonMask        ,""}         // obsolete
	};

	QString strQSS = ".QPushButton{background-color:#298cf0;border-radius:8px;color:#ffffff;}";
	//".QPushButton:pressed{background:#38a357;}"\
		//".QPushButton:!enabled{background:#cacaca;}";


	for (auto btnType : ButtonMap)
	{
		if ((btnType.first & stdButtons) == btnType.first)
		{
			Msgbox.setButtonText(btnType.first, btnType.second);
		}
	}
	Msgbox.setStyleSheet(strQSS);
	return Msgbox.exec();
}

string UTF8_GBK(const char* strUtf8)
{
	QTextCodec* utf8Codec = QTextCodec::codecForName("utf-8");
	QTextCodec* GBKCodec = QTextCodec::codecForName("GB18030");
	QString strUnicode = utf8Codec->toUnicode(strUtf8);
	QByteArray ByteGb2312 = GBKCodec->fromUnicode(strUnicode);
	return string(ByteGb2312.data(), ByteGb2312.size());
}

string GBK_UTF8(const char* strGBK)
{
	QTextCodec* utf8Codec = QTextCodec::codecForName("utf-8");
	QTextCodec* GBKCodec = QTextCodec::codecForName("GB18030");
	QString strUnicode = GBKCodec->toUnicode(strGBK);
	QByteArray ByteUTF8 = utf8Codec->fromUnicode(strUnicode);
	return string(ByteUTF8.data(), ByteUTF8.size());
}

void  SetTableWidgetItemChecked(QTableWidget* pTableWidget, int nRow, int nCol, QButtonGroup* pButtonGrp, int nItemID, bool bChecked)
{
	QWidget* pCellWidget = new QWidget(pTableWidget);
	QHBoxLayout* pHLayout = new QHBoxLayout();
	pHLayout->setMargin(0);
	pHLayout->setSpacing(0);

	QCheckBox* pCheckBox = new QCheckBox();
	pCheckBox->setChecked(false);
	pCheckBox->setFont(pTableWidget->font());
	pCheckBox->setFocusPolicy(Qt::NoFocus);
	pCheckBox->setStyle(QStyleFactory::create("fusion"));
	pCheckBox->setStyleSheet(QString(".QCheckBox {margin:3px;border:0px;}QCheckBox::indicator {width: %1px; height: %1px; }").arg(20));

	pHLayout->addWidget(pCheckBox);
	pHLayout->setAlignment(pCheckBox, Qt::AlignCenter);
	pCellWidget->setLayout(pHLayout);
	//pCheckBox->setEnabled(false);
	pButtonGrp->addButton(pCheckBox, nItemID);
	pTableWidget->setCellWidget(nRow, nCol, pCellWidget);
	if (bChecked)
		pCheckBox->setCheckState(Qt::Checked);
}

QStringList SearchFiles(const QString& dir_path, QDateTime* pStart, QDateTime* pStop)
{
	QStringList FileList;
	QDir dir(dir_path);
	if (!dir.exists())
	{
		qDebug() << "it is not true dir_path";
	}

	/*设置过滤参数，QDir::NoDotAndDotDot表示不会去遍历上层目录*/
	QDirIterator dir_iterator(dir_path, QDir::Files | QDir::Dirs | QDir::NoDotAndDotDot, QDirIterator::Subdirectories);

	while (dir_iterator.hasNext())
	{
		dir_iterator.next();
		QFileInfo file_info = dir_iterator.fileInfo();
		QString files = file_info.absoluteFilePath();
		if (pStart && file_info.birthTime() < *pStart)
			continue;
		if (pStop && file_info.birthTime() > *pStop)
			continue;
		FileList.append(files);
	}

	return FileList;
}

int  DataCenter::ReaderIDCard(IDCardInfo* pIDCard)
{
	int nResult = IDCard_Status::IDCard_Succeed;
	auto strDevPort = GetSysConfigure()->DevConfig.strIDCardReaderPort;
	bool bDevOpened = false;
	do
	{
		if (strDevPort == "AUTO" || !strDevPort.size())
		{
			gInfo() << "Try to open IDCard Reader auto!" << strDevPort;
			nResult = OpenReader(nullptr);
		}
		else
		{
			gInfo() << "Try to open IDCard Reader " << strDevPort;
			nResult = OpenReader(strDevPort.c_str());
		}

		if (nResult != IDCard_Status::IDCard_Succeed)
		{
			break;
		}
		bDevOpened = true;
		nResult = FindIDCard();
		if (nResult != IDCard_Status::IDCard_Succeed)
		{
			break;
		}

		nResult = ReadIDCard(pIDCard);
		if (nResult != IDCard_Status::IDCard_Succeed)
		{
			break;
		}
		int nNationalityCode = strtol((char*)pIDCard->szNationaltyCode, nullptr, 10);
		if (nNationalityCode < 10)
			sprintf_s((char*)pIDCard->szNationaltyCode, sizeof(pIDCard->szNationaltyCode), "%02d", nNationalityCode);

	} while (0);
	//if (bDevOpened)
	{
		gInfo() << "Try to close IDCard Reader!";
		CloseReader();
	}
	return nResult;
}

int DataCenter::OpenDevice(QString& strMessage)
{
	gInfo() << __FUNCTION__;
	int nResult = -1;
	if (QFailed(nResult = OpenPrinter(strMessage)))
	{
		gError() << gQStr(strMessage);
		return nResult;
	}

	if (QFailed(nResult = OpenSSCardReader(strMessage)))
	{
		gError() << gQStr(strMessage);
		return nResult;
	}
	return 0;
}

int DataCenter::OpenSSCardService(SSCardService** ppService, QString& strMessage, IDCardInfoPtr pInputIDCard)
{
	int nResult = -1;
	RegionInfo& Regcfg = pSysConfig->Region;
	string strAppPath = QCoreApplication::applicationDirPath().toStdString();
	string strServiceMudule = strAppPath + "/" + Regcfg.strSSCardSeriveModule;
	string strErrText;
	try
	{
		do
		{
			if (nCardServiceType == ServiceType::Service_Unknown)
			{
				strMessage = "未知服务或不支持的服务类型!";
				return -1;
			}
			if (!pSServiceLib)
			{
				pSServiceLib = make_shared<KTModule<SSCardService>>(strServiceMudule);
				if (!pSServiceLib)
				{
					strMessage = QString("内存不足，加载‘%1’实例失败!").arg(strServiceMudule.c_str());
					break;
				}
			}
			if (!pSScardSerivce)
			{
				pSScardSerivce = pSServiceLib->Instance();
				if (!pSScardSerivce)
				{
					strMessage = QString("创建‘%1’实例失败!").arg(strServiceMudule.c_str());
					break;
				}
			}
			RegionInfo& Reg = GetSysConfigure()->Region;

			string strDescPath = strAppPath + "/" + Reg.strSSCardServiceDescription;
			QFileInfo fi(strDescPath.c_str());
			if (!fi.isFile())
			{
				strMessage = QString("无法加载卡管服务描述文件:%1").arg(strDescPath.c_str());
				break;
			}
			ifstream fsDesc(strDescPath);
			fsDesc.seekg(3);
			stringstream ss;
			ss << fsDesc.rdbuf();
			string strJsonIn = ss.str();
			gInfo() << "ServiceDescription:" << strJsonIn;
			string strJsonOut;
			if (QFailed(pSScardSerivce->Initialize(strJsonIn, strJsonOut)))
			{
				//CJsonObject jsonOut;
				//if (!jsonOut.Parse(strJsonOut))
				//{
				//	gInfo() << "Json Parse failed:" << strJsonOut;
				//	break;
				//}
				//if (!jsonOut.KeyExist("errflag") ||
				//	!jsonOut.KeyExist("errtext"))
				//{
				//	gInfo() << "Can't find field errflag or errtext:" << strJsonOut;
				//	break;
				//}
				//jsonOut.Get("errtext", strErrText);
				strMessage = QString::fromLocal8Bit(strJsonOut.c_str());
				break;
			}
			if (!pIDCard && !pInputIDCard)
			{
				strMessage = "设置业务类型失败,身份证信息无效！";
				break;
			}
			string strIdentity = pIDCard ? (const char*)pIDCard->szIdentity : (const char*)pInputIDCard->szIdentity;
			if (QFailed(pSScardSerivce->SetServiceType(nCardServiceType, strIdentity)))
			{
				strMessage = "设置业务类型失败,只能是新办卡,补办和挂失/解挂";
				break;
			}
			pSScardSerivce->SetSwitchBank(g_pDataCenter->bSwitchBank);
			LOG(ERROR) << "bSwitchBank : " << g_pDataCenter->bSwitchBank;
			*ppService = pSScardSerivce;
			nResult = 0;
		} while (0);
		if (QFailed(nResult))
			return -1;
		else
			return 0;
	}
	catch (std::exception& e)
	{
		strMessage = e.what();
		gError() << gQStr(strMessage);
		return -1;
	}
}

int DataCenter::CloseSSCardService(QString& strMessage)
{
	if (pSScardSerivce)
	{
		delete pSScardSerivce;
		pSScardSerivce = nullptr;
	}
	if (pSServiceLib)
		pSServiceLib = nullptr;

	return 0;
}

int DataCenter::OpenPrinter(QString& strMessage)
{
	gInfo() << __FUNCTION__;
	int nResult = -1;
	char szRCode[32] = { 0 };
	DeviceConfig& DevConfig = pSysConfig->DevConfig;
	QString strAppPath = QCoreApplication::applicationDirPath();
	QString strPrinterMudule = strAppPath + "/" + DevConfig.strPrinterModule.c_str();
	try
	{
		do
		{
			if (DevConfig.nPrinterType < PrinterType::PRINTER_MIN ||
				DevConfig.nPrinterType > PrinterType::PRINTER_MAX)
			{
				strMessage = QString("不支持打印机型号代码:%1").arg(DevConfig.nPrinterType);
				break;
			}

			if (!m_pPrinterlib)
			{
				m_pPrinterlib = make_shared<KTModule<KT_Printer>>(strPrinterMudule.toStdString());
				if (!m_pPrinterlib)
				{
					strMessage = QString("内存不足，加载‘%1’实例失败!").arg(strPrinterMudule);
					break;
				}
			}
			if (!m_pPrinter)
			{
				m_pPrinter = m_pPrinterlib->Instance();
				if (!m_pPrinter)
				{
					strMessage = QString("创建‘%1’实例失败!").arg(strPrinterMudule);
					break;
				}
			}
			if (QFailed(nResult = m_pPrinter->Printer_Init(DevConfig.nPrinterType, szRCode)))
			{
				strMessage = QString("打印机初始化‘%1’失败,错误代码:%2").arg(DevConfig.strPrinterType.c_str()).arg(szRCode);
				break;
			}
			if (QFailed(nResult = m_pPrinter->Printer_Open(szRCode)))
			{
				if (strcmp(szRCode, "0010") == 0)
					strMessage = QString("打开打印机‘%1’失败,未安装色带!").arg(DevConfig.strPrinterType.c_str());
				else if (strcmp(szRCode, "0011") == 0)
					strMessage = QString("打开打印机‘%1’失败,色带与打印不兼容!").arg(DevConfig.strPrinterType.c_str());
				else
					strMessage = QString("打开打印机‘%1’失败,错误代码:%2").arg(DevConfig.strPrinterType.c_str()).arg(szRCode);
				break;
			}
			nResult = 0;
		} while (0);
		if (QFailed(nResult))
			return -1;
		else
			return 0;
	}
	catch (std::exception& e)
	{
		strMessage = e.what();
		gError() << gQStr(strMessage);
		return -1;
	}
}

int DataCenter::OpenPrinter(QString strPrinterLib, PrinterType nPrinterType, int& nDepenseBox, QString& strDPI, QString& strMessage)
//KT_Printer* DataCenter::OpenPrinter(QString strPrinterLib, PrinterType nPrinterType, int& nDepenseBox, QString& strDPI, QString& strMessage)
{
	int nResult = -1;
	char szRCode[32] = { 0 };

	try
	{
		m_pPrinterlib = nullptr;
		do
		{
			if (nPrinterType < PrinterType::PRINTER_MIN ||
				nPrinterType > PrinterType::PRINTER_MAX)
			{
				strMessage = QString("不支持打印机型号代码:%1").arg(nPrinterType);
				break;
			}
			if (!m_pPrinterlib)
			{
				QString strAppPath = QCoreApplication::applicationDirPath();
				QString strFullPrinterModule = strAppPath + "/" + strPrinterLib;
				gInfo() << "Try to load " << strFullPrinterModule.toStdString();
				m_pPrinterlib = make_shared<KTModule<KT_Printer>>(strFullPrinterModule.toStdString());
				if (!m_pPrinterlib)
				{
					strMessage = QString("内存不足，加载‘%1’实例失败!").arg(strFullPrinterModule);
					break;
				}
				m_pPrinter = m_pPrinterlib->Instance();
				if (!m_pPrinter)
				{
					strMessage = QString("创建‘%1’实例失败!").arg(strFullPrinterModule);
					break;
				}

				if (QFailed(nResult = m_pPrinter->Printer_Init(nPrinterType, szRCode)))
				{
					strMessage = QString("Printer_Init‘%1’失败,错误代码:%2").arg(szPrinterTypeList[nPrinterType]).arg(szRCode);
					break;
				}

				if (QFailed(nResult = m_pPrinter->Printer_Open(szRCode)))
				{
					strMessage = QString("Printer_Open‘%1’失败,错误代码:%2").arg(szPrinterTypeList[nPrinterType]).arg(szRCode);
					break;
				}
				m_pPrinter->Printer_ExtraCommand("Ps;y;=;250", szRCode);
				m_pPrinter->Printer_ExtraCommand("Ps;m;=;250", szRCode);
				m_pPrinter->Printer_ExtraCommand("Ps;c;=;250", szRCode);
				TraceMsgA("%s Printer Address = %p", __FUNCTION__, &m_pPrinter);
			}

		} while (0);
		if (QFailed(nResult))
			return -1;
		else
			return 0;
		//return m_pPrinter;
	}
	catch (std::exception& e)
	{
		strMessage = e.what();
		gError() << gQStr(strMessage);
		return -1;
		//return nullptr;
	}
}

void DataCenter::CloseDevice()
{
	char szRCode[32] = { 0 };

	if (m_pPrinter)
	{
		m_pPrinter->Printer_Close(szRCode);
		m_pPrinter = nullptr;
	}

	m_pPrinterlib = nullptr;
	CloseSSCardReader();
}

void DataCenter::CloseSSCardReader()
{
	char szRCode[32] = { 0 };
	if (m_pSSCardReader)
	{
		m_pSSCardReader->Reader_Exit(szRCode);
		m_pSSCardReader = nullptr;
	}
	m_pSSCardReaderLib = nullptr;
}

int DataCenter::OpenSSCardReader(QString& strMessage)
{
	int nResult = -1;
	try
	{
		gInfo() << __FUNCTION__;
		do
		{
			char szRCode[32] = { 0 };
			DeviceConfig& DevConfig = pSysConfig->DevConfig;
			RegionInfo& Region = pSysConfig->Region;
			QString strAppPath = QCoreApplication::applicationDirPath();
			QString strReaderMudule = strAppPath + "/" + DevConfig.strReaderModule.c_str();
			if (!m_pSSCardReaderLib)
			{
				m_pSSCardReaderLib = make_shared<KTModule<KT_Reader>>(strReaderMudule.toStdString());
				if (!m_pSSCardReaderLib)
				{
					strMessage = strMessage = QString("内存不足，加载'%1'失败!").arg(strReaderMudule);
					break;
				}
			}
			if (!m_pSSCardReader)
			{
				m_pSSCardReader = m_pSSCardReaderLib->Instance();
				if (!m_pSSCardReader)
				{
					strMessage = QString("创建'%1'实例失败!").arg(strReaderMudule);
					break;
				}
				if (QFailed(nResult = m_pSSCardReader->Reader_Create(DevConfig.nSSCardReaderType, szRCode)))
				{
					strMessage = QString("Reader_Create'%1'失败,错误代码:%2").arg(DevConfig.strSSCardReadType.c_str()).arg(szRCode);
					break;
				}
				if (QFailed(nResult = m_pSSCardReader->Reader_Init(PORT_NUM__, szRCode)))
				{
					strMessage = QString("Reader_Init失败,错误代码:%2").arg(szRCode);
					break;
				}
			}

			char szOutInfo[1024] = { 0 };
			DriverInit(m_pSSCardReader, (char*)Region.strCountry.c_str(), (char*)Region.strSSCardDefaulutPin.c_str(), (char*)Region.strPrimaryKey.c_str(), szOutInfo);
			nResult = 0;
		} while (0);
		if (QFailed(nResult))
			return -1;
		else
			return 0;
	}
	catch (std::exception& e)
	{
		strMessage = e.what();
		gError() << gQStr(strMessage);
		return -1;
	}
	return 0;
}

int DataCenter::OpenSSCardReader(QString strLib, ReaderBrand nReaderType, QString& strMessage)
{
	int nResult = -1;
	try
	{
		m_pSSCardReaderLib = nullptr;
		do
		{
			if (!m_pSSCardReaderLib)
			{
				char szRCode[32] = { 0 };
				QString strAppPath = QCoreApplication::applicationDirPath();
				QString strReaderMudule = strAppPath + "/" + strLib.toStdString().c_str();

				m_pSSCardReaderLib = make_shared<KTModule<KT_Reader>>(strReaderMudule.toStdString());
				if (!m_pSSCardReaderLib)
				{
					strMessage = strMessage = QString("内存不足，加载‘%1’实例失败!").arg(strReaderMudule);
					break;
				}
				m_pSSCardReader = m_pSSCardReaderLib->Instance();
				if (!m_pSSCardReader)
				{
					strMessage = QString("创建‘%1’实例失败!").arg(strReaderMudule);
					break;
				}
				if (QFailed(nResult = m_pSSCardReader->Reader_Create(nReaderType, szRCode)))
				{
					strMessage = QString("Reader_Create(‘%1’)失败,错误代码:%2").arg(nReaderType).arg(szRCode);
					break;
				}

				if (QFailed(nResult = m_pSSCardReader->Reader_Init(PORT_NUM__, szRCode)))
				{
					strMessage = QString("Reader_Init失败,错误代码:%2").arg(szRCode);
					break;
				}
			}
			nResult = 0;
		} while (0);
		if (QFailed(nResult))
			return -1;
		else
			return 0;
	}
	catch (std::exception& e)
	{
		strMessage = e.what();
		gError() << gQStr(strMessage);
		return -1;
	}
	return 0;
}

int DataCenter::TestCard(QString& strMessage)
{
	PRINTERSTATUS PrinterStatus;
	DeviceConfig& DevConfig = pSysConfig->DevConfig;
	int& nDepenseBox = DevConfig.nDepenseBox;

	int nResult = -1;
	BOXINFO BoxInfo;
	ZeroMemory(&BoxInfo, sizeof(BOXINFO));
	char szRCode[32] = { 0 };
	do
	{
		qDebug() << __FILE__ << " " << __LINE__;
		if (QFailed(m_pPrinter->Printer_BoxStatus(BoxInfo, szRCode)))
		{
			strMessage = QString("Printer_BoxStatus失败:%1").arg(szRCode);
			break;
		}

		if (BoxInfo.BoxList[nDepenseBox - 1].BoxStatus == 2)
		{
			strMessage = QString("进卡箱无卡,请放入卡片后重试!").arg(nDepenseBox).arg(BoxInfo.BoxList[nDepenseBox].BoxStatus);
			break;
		}
		qDebug() << __FILE__ << " " << __LINE__;
		if (QFailed(g_pDataCenter->Depense(strMessage)))
		{
			break;
		}
		qDebug() << __FILE__ << " " << __LINE__;
		if (QFailed(m_pPrinter->Printer_Status(PrinterStatus, szRCode)))
		{

			strMessage = QString("Printer_Status失败，错误代码:%1!").arg(szRCode);
			break;
		}
		if (PrinterStatus.fwDevice != 0)
		{
			//avansia通过打印空图片才可进卡，此处会显示正忙
			if (DevConfig.nPrinterType == EVOLIS_AVANSIA && PrinterStatus.fwDevice == 1)
			{
				PrinterStatus.fwDevice = 0;
			}
			else {
				strMessage = QString("打印机未就绪,状态代码:%1!").arg(PrinterStatus.fwDevice);
				break;
			}

		}
		// 0-无卡；2-卡在内部；3-卡在上电位，4-卡在闸门外 5-堵卡；6-卡片未知
		bool bSucceed = false;
		switch (PrinterStatus.fwMedia)
		{
		case 0:		// 机内无卡
		{
			break;
		}
		case 1:			// 1-卡在门口:
		{
			strMessage = QString("打印机出卡号尚有未取走卡片,取走点击确定以继续!");
			nResult = 1;
			break;
		}
		case 2:			// 2-卡在内部；
		case 3:			// 3-卡在接触位
		case 4:			// 4-卡在非接位
		{
			if (!m_pSSCardReader)
				break;
			if (!g_pDataCenter->bTestCard)
			{
				bSucceed = true;
				break;
			}

			char szCardATR[128] = { 0 };
			char szRCode[128] = { 0 };
			int nCardATRLen = 0;
			int retryTimes{ 0 };
			while (retryTimes < 3)
			{
				if (QFailed(m_pSSCardReader->Reader_PowerOn(DevConfig.nSSCardReaderPowerOnType, szCardATR, nCardATRLen, szRCode)))
				{
					stringstream ss;
					ss << "卡片上电失败次数：" << retryTimes << ", 请更换卡片或重新进卡!";
					strMessage = QString::fromStdString(ss.str());
					LOG(INFO) << ss.str();
					retryTimes++;
					continue;
				}
				else
				{
					strMessage = "";
					break;
				}
			}
			const char* szCommand = "0084000008";
			char szOut[1024] = { 0 };
			int nOutLen = 0;
			if (QFailed(m_pSSCardReader->Reader_APDU((BYTE*)szCommand, strlen(szCommand), szOut, nOutLen, szRCode)))
			{
				strMessage = "卡片数据交互失败,请更换卡片或重新进卡!";
				break;
			}

			char szBankNum[64] = { 0 };
			if (QFailed(iReadBankNumber(DevConfig.nSSCardReaderPowerOnType, szBankNum)))
			{
				strMessage = QString("读卡信息失败,PowerOnType:%1,nResult:%2").arg((int)DevConfig.nSSCardReaderPowerOnType).arg(nResult);
				break;
			}
			gInfo() << "BankNum:" << szBankNum;

			//string strMatchBankName;
			//for (auto var : mapBankCode)
			//{
			//	const char* pDest = strstr((const char*)szBankNum, var.first.c_str());
			//	if ((pDest - szBankNum) == 0)
			//	{
			//		strMatchBankName = var.second;
			//		break;
			//	}
			//}
			//gInfo() << "Card BankNum" << szBankNum << "\tMatched BankName:" << UTF8_GBK(strMatchBankName.c_str());
			//if (strMatchBankName.size())
			//{
			//	string strCurrentBank;
			//	CheckBankCode(strCurrentBank, strMessage);
			//	gInfo() << "Current Bank:" << UTF8_GBK(strCurrentBank.c_str());
			//	if (strCurrentBank.find(strMatchBankName) == string::npos &&
			//		strMatchBankName.find(strCurrentBank) == string::npos)
			//	{
			//		strMessage = QString("当前社保卡(%1)与网点银行(%2)不匹配,请检查设置!").arg(strMatchBankName.c_str()).arg(strCurrentBank.c_str());
			//		break;
			//	}
			//}

			bSucceed = true;
			break;
		}
		case 5:
		{
			strMessage = QString("打印机堵卡,请联系相关技术人员处理!");
			break;
		}
		case 6:
		default:
		{
			strMessage = QString("发生未知错误,请联系相关技术人员处理!");
			break;
		}
		}
		if (!bSucceed)
			break;

		nResult = 0;
	} while (0);
	if (QFailed(nResult))
	{
		qDebug() << __FILE__ << " " << __LINE__;
		m_pPrinter->Printer_Retract(1, szRCode);
	}
	return nResult;
}

int DataCenter::TestPrinter(QString& strMessage)
{
	PRINTERSTATUS PrinterStatus;
	DeviceConfig& DevConfig = pSysConfig->DevConfig;
	int& nDepenseBox = DevConfig.nDepenseBox;

	int nResult = -1;
	BOXINFO BoxInfo;
	ZeroMemory(&BoxInfo, sizeof(BOXINFO));
	char szRCode[32] = { 0 };
	do
	{
		gInfo() << __FUNCTION__;
		if (QFailed(m_pPrinter->Printer_BoxStatus(BoxInfo, szRCode)))
		{
			strMessage = QString("Printer_BoxStatus失败:%1").arg(szRCode);
			break;
		}

		if (BoxInfo.BoxList[nDepenseBox - 1].BoxStatus == 2)
		{
			strMessage = QString("进卡箱无卡,请放入卡片后重试!").arg(nDepenseBox).arg(BoxInfo.BoxList[nDepenseBox].BoxStatus);
			break;
		}

		if (QFailed(m_pPrinter->Printer_Status(PrinterStatus, szRCode)))
		{
			strMessage = QString("Printer_Status失败，错误代码:%1!").arg(szRCode);
			break;
		}

		if (PrinterStatus.fwDevice != 0)
		{
			strMessage = QString("打印机未就绪,状态代码:%1!").arg(PrinterStatus.fwDevice);
			break;
		}
		// 0-无卡；2-卡在内部；3-卡在上电位，4-卡在闸门外 5-堵卡；6-卡片未知
		bool bSucceed = false;
		switch (PrinterStatus.fwMedia)
		{
		case 0:		// 机内无卡
		{
			bSucceed = true;
			break;
		}
		case 1:			// 1-卡在门口:
		{
			strMessage = QString("打印机出卡口尚有未取走卡片,请先取走卡片后重新操作!");
			nResult = 1;
			break;
		}
		case 2:			// 2-卡在内部；
		case 3:			// 3-卡在上电位
		case 4:			// 4-卡在闸门外
		{
			if (QFailed(m_pPrinter->Printer_Retract(1, szRCode)))
			{
				strMessage = QString("打印机尚有未取走卡片,尝试将其移动到回收箱失败，错误代码:%1!").arg(szRCode);
				break;
			}
			else
				bSucceed = true;
			break;
		}

		case 5:
		{
			strMessage = QString("打印机堵卡,请联系相关技术人员处理!");
			break;
		}
		case 6:
		default:
		{
			strMessage = QString("发生未知错误,请联系相关技术人员处理!");
			break;
		}
		}
		if (!bSucceed)
			break;
		// 0-FLLL;1-LOW;2-OUT;3-NOTSUPP;4-UNKNOW
		bSucceed = false;
		switch (PrinterStatus.fwToner)
		{
		case 0:
		{
			bSucceed = true;
			break;
		}
		case 1:
		{
			strMessage = QString("打印机色带耗余量不足,请更换色带!");
			break;
		}
		case 2:
		{
			strMessage = QString("打印机色带耗尽或未安装色带,状态代码:%1!").arg(PrinterStatus.fwToner);
			break;
		}
		case 3:
		{
			strMessage = QString("打印机中安装了不兼容的色带,请更换色带!").arg(PrinterStatus.fwToner);
			break;
		}
		case 4:
		default:
		{
			strMessage = QString("发生未知错误!").arg(PrinterStatus.fwToner);
			break;
		}
		}
		if (!bSucceed)
			break;
		nResult = 0;
	} while (0);
	return nResult;
}

int DataCenter::Depense(QString& strMessage)
{
	int nResult = -1;

	//BOXINFO BoxInfo = {0};
	//PRINTERSTATUS PrinterStatus;
	DeviceConfig& DevConfig = pSysConfig->DevConfig;
	char szRCode[32] = { 0 };
	do
	{
		if (!m_pPrinter)
		{
			strMessage = "打印机未初始化";
			break;
		}

		/* 此功能前置,不再在制卡时才检查是否有卡，而是在进行整个流程开始前就检查，以防止制止中途无卡时发生无卡的情况
		if (QFailed(nResult = TestPrinter(strMessage)))
		{
			break;
		}*/

		int nDepensePos = 2;	// 1-读磁位；2-接触IC位;3-非接IC位;4-打印位， 默认为接触位
		if (DevConfig.nSSCardReaderPowerOnType == 2)
			nDepensePos = 3;

		if (QFailed(m_pPrinter->Printer_Dispense(DevConfig.nDepenseBox, nDepensePos, szRCode)))
		{
			strMessage = QString("Printer_Dispense失败，错误代码:%1!").arg(szRCode);
			break;
		}
		nResult = 0;
	} while (0);

	if (QFailed(nResult))
	{
		gInfo() << gQStr(strMessage);
	}
	return nResult;
}

int DataCenter::Depense(int nDepenseBox, CardPowerType nPowerOnType, QString& strMessage)
{
	int nResult = -1;
	BOXINFO BoxInfo;
	ZeroMemory(&BoxInfo, sizeof(BOXINFO));
	char szRCode[32] = { 0 };
	do
	{
		if (!m_pPrinter)
		{
			strMessage = "打印机未初始化";
			break;
		}

		if (QFailed(nResult = TestPrinter(strMessage)))
		{
			break;
		}
		int nDepensePos = 4;	// 1-读磁位；2-接触IC位;3-非接IC位;4-打印位， 默认为接触位
		if (nPowerOnType == CardPowerType::READER_UNCONTACT)
			nDepensePos = 3;
		else if (nPowerOnType == CardPowerType::READER_CONTACT)
			nDepensePos = 2;

		if (QFailed(m_pPrinter->Printer_Dispense(nDepenseBox - 1, nDepensePos, szRCode)))
		{
			strMessage = QString("Printer_Dispense失败，错误代码:%1!").arg(szRCode);
			break;
		}
		nResult = 0;
	} while (0);

	if (QFailed(nResult))
	{
		gInfo() << gQStr(strMessage);
	}
	return nResult;
}

string DataCenter::MakeCardInfo(SSCardBaseInfoPtr& pSSCardInfo)
{
	string strCardInfo = "";
	//RegionInfo& Region = g_pDataCenter->GetSysConfigure()->Region;
	//卡识别码|初始化机构编码|发卡日期|卡有效期|卡号|社会保障号码|姓名|姓名拓展|性别|民族|出生日期|
	strCardInfo = "||";	// 卡识别码|初始化机构编码 两者为空
	//strCardInfo += pSSCardInfo->strOrganID;				strCardInfo += "|";
	strCardInfo += pSSCardInfo->strReleaseDate;				strCardInfo += "|";
	strCardInfo += pSSCardInfo->strValidDate;				strCardInfo += "|";
	strCardInfo += pSSCardInfo->strCardNum;					strCardInfo += "|";
	strCardInfo += pSSCardInfo->strIdentity;				strCardInfo += "|";
	strCardInfo += pSSCardInfo->strName;					strCardInfo += "||";//跳过姓名拓展
	strCardInfo += pSSCardInfo->strGenderCode;				strCardInfo += "|";
	strCardInfo += pSSCardInfo->strNationCode;				strCardInfo += "|";
	strCardInfo += pSSCardInfo->strBirthday;				strCardInfo += "|";
	return strCardInfo;
}

int DataCenter::PrintExtraText(QString strText, int nAngle, float fxPos, float fyPos, QString strFont, int nFontSize, int nColor)
{
	TextPosition Text;
	Text.strText = strText.toStdString();
	Text.fxPos = fxPos;
	Text.fyPos = fyPos;
	Text.nAngle = nAngle;
	Text.strFontName = strFont.toStdString();
	Text.nFontSize = nFontSize;
	Text.nColor = nColor;
	vecExtraText.push_back(Text);
	return 0;
}

int DataCenter::PrintCard(SSCardBaseInfoPtr& pSSCardInfo, QString strPhoto, QString& strMessage, bool bPrintText)
{
	char szBuffer[1024] = { 0 };
	int nBufferSize = sizeof(szBuffer);
	int nResult = -1;
	char szRCode[32] = { 0 };
	CardFormPtr& pCardForm = GetCardForm();
	string strStage = "PrintCard";
	string strDatagram = "";
	do
	{
		//if (!m_pSSCardReader)
		//{
		//	strMessage = "读卡器未就绪!";
		//	break;
		//}
		if (pSScardSerivce)
		{
			if (pSScardSerivce->GetStageStatus(strStage, nResult, strDatagram) && nResult == 0)
			{// 与之前写入成功的卡片的很行卡号进行核对
				string strLastBankNum;
				CJsonObject json(strDatagram);
				json.Get("BankNum", strLastBankNum);
				if (strLastBankNum == pSSCardInfo->strBankNum)
				{
					strMessage = "打印操作已经完成,无须重复操作!";
					gInfo() << gQStr(strMessage);
					break;
				}
			}
		}

		nResult = -1;
		nBufferSize = sizeof(szBuffer);
		ZeroMemory(szBuffer, nBufferSize);
		if (QFailed(m_pPrinter->Printer_InitPrint(nullptr, szRCode)))
		{
			strMessage = QString("Printer_InitPrint失败，错误代码:%1!").arg(szRCode);
			//break;
		}

		if (bPrintText)
		{
			TextPosition* pFieldPos = &pCardForm->posName;
			m_pPrinter->Printer_AddText((char*)pSSCardInfo->strName.c_str(), pFieldPos->nAngle, pFieldPos->fxPos, pFieldPos->fyPos, (char*)pCardForm->strFont.c_str(), pCardForm->nFontSize, 0, 0, szRCode);
			pFieldPos = &pCardForm->posIDNumber;
			m_pPrinter->Printer_AddText((char*)pSSCardInfo->strIdentity.c_str(), pFieldPos->nAngle, pFieldPos->fxPos, pFieldPos->fyPos, (char*)pCardForm->strFont.c_str(), pCardForm->nFontSize, 0, 0, szRCode);
			pFieldPos = &pCardForm->posSSCardID;
			m_pPrinter->Printer_AddText((char*)pSSCardInfo->strCardNum.c_str(), pFieldPos->nAngle, pFieldPos->fxPos, pFieldPos->fyPos, (char*)pCardForm->strFont.c_str(), pCardForm->nFontSize, 0, 0, szRCode);
			pFieldPos = &pCardForm->posIssueDate;

			stringstream date;
			auto tNow = chrono::system_clock::to_time_t(std::chrono::system_clock::now());
			// date = "2022-01-26 09:51:00"
			// date << std::put_time(std::localtime(&tNow), "%Y-%m-%d %X");
			date << std::put_time(std::localtime(&tNow), "%Y年%m月");
			pSSCardInfo->strReleaseDate = UTF8_GBK(date.str().c_str());
			m_pPrinter->Printer_AddText((char*)pSSCardInfo->strReleaseDate.c_str(), pFieldPos->nAngle, pFieldPos->fxPos, pFieldPos->fyPos, (char*)pCardForm->strFont.c_str(), pCardForm->nFontSize, 0, 0, szRCode);
		}

		if (vecExtraText.size())
		{
			for (auto var : vecExtraText)
				m_pPrinter->Printer_AddText((char*)var.strText.c_str(), var.nAngle, var.fxPos, var.fyPos, (char*)var.strFontName.c_str(), var.nFontSize, 0, var.nColor, szRCode);
			vecExtraText.clear();
		}

		ImagePosition& ImgPos = pCardForm->posImage;
		if (!strPhoto.size())
			m_pPrinter->Printer_AddImage((char*)strSSCardPhotoFile.c_str(), ImgPos.nAngle, ImgPos.fxPos, ImgPos.fyPos, ImgPos.fHeight, ImgPos.fWidth, szRCode);
		else
			m_pPrinter->Printer_AddImage((char*)strPhoto.toStdString().c_str(), ImgPos.nAngle, ImgPos.fxPos, ImgPos.fyPos, ImgPos.fHeight, ImgPos.fWidth, szRCode);

		if (QFailed(m_pPrinter->Printer_StartPrint(szRCode)))
		{
			strMessage = QString("Printer_StartPrint失败，错误代码:%1!").arg(szRCode);
			break;
		}

		if (pSScardSerivce)
		{
			CJsonObject json;
			json.Add("BankNum", pSSCardInfo->strBankNum);
			pSScardSerivce->UpdateStage(strStage, 0, json.ToFormattedString());
		}

		nResult = 0;

	} while (0);
	return nResult;
}

int DataCenter::PrintTestCard(SSCardBaseInfoPtr& pSSCardInfo, QString strPhoto, QString& strMessage, bool bPrintText)
{
	char szBuffer[1024] = { 0 };
	int nBufferSize = sizeof(szBuffer);
	int nResult = -1;
	char szRCode[32] = { 0 };
	CardFormPtr& pCardForm = GetCardForm();
	do
	{

		nResult = -1;
		nBufferSize = sizeof(szBuffer);
		ZeroMemory(szBuffer, nBufferSize);
		if (QFailed(m_pPrinter->Printer_InitPrint(nullptr, szRCode)))
		{
			strMessage = QString("Printer_InitPrint失败，错误代码:%1!").arg(szRCode);
			//break;
		}

		if (bPrintText)
		{
			TextPosition* pFieldPos = &pCardForm->posName;
			m_pPrinter->Printer_AddText((char*)pSSCardInfo->strName.c_str(), pFieldPos->nAngle, pFieldPos->fxPos, pFieldPos->fyPos, (char*)pCardForm->strFont.c_str(), pCardForm->nFontSize, 0, 0, szRCode);
			pFieldPos = &pCardForm->posIDNumber;
			m_pPrinter->Printer_AddText((char*)pSSCardInfo->strIdentity.c_str(), pFieldPos->nAngle, pFieldPos->fxPos, pFieldPos->fyPos, (char*)pCardForm->strFont.c_str(), pCardForm->nFontSize, 0, 0, szRCode);
			pFieldPos = &pCardForm->posSSCardID;
			m_pPrinter->Printer_AddText((char*)pSSCardInfo->strCardNum.c_str(), pFieldPos->nAngle, pFieldPos->fxPos, pFieldPos->fyPos, (char*)pCardForm->strFont.c_str(), pCardForm->nFontSize, 0, 0, szRCode);
			pFieldPos = &pCardForm->posIssueDate;
			m_pPrinter->Printer_AddText((char*)pSSCardInfo->strReleaseDate.c_str(), pFieldPos->nAngle, pFieldPos->fxPos, pFieldPos->fyPos, (char*)pCardForm->strFont.c_str(), pCardForm->nFontSize, 0, 0, szRCode);
		}

		if (vecExtraText.size())
		{
			for (auto var : vecExtraText)
				m_pPrinter->Printer_AddText((char*)var.strText.c_str(), var.nAngle, var.fxPos, var.fyPos, (char*)var.strFontName.c_str(), var.nFontSize, 0, var.nColor, szRCode);
			vecExtraText.clear();
		}

		ImagePosition& ImgPos = pCardForm->posImage;
		if (!strPhoto.size())
			m_pPrinter->Printer_AddImage((char*)strSSCardPhotoFile.c_str(), ImgPos.nAngle, ImgPos.fxPos, ImgPos.fyPos, ImgPos.fHeight, ImgPos.fWidth, szRCode);
		else
			m_pPrinter->Printer_AddImage((char*)strPhoto.toStdString().c_str(), ImgPos.nAngle, ImgPos.fxPos, ImgPos.fyPos, ImgPos.fHeight, ImgPos.fWidth, szRCode);

		if (QFailed(m_pPrinter->Printer_StartPrint(szRCode)))
		{
			strMessage = QString("Printer_StartPrint失败，错误代码:%1!").arg(szRCode);
			break;
		}

		nResult = 0;

	} while (0);
	return nResult;
}

int DataCenter::MoveCard(QString& strMessage)
{
	//	int nRetry = 0;
	int nPowerOnPos = 4;	// 1-读磁位；2-接触IC位;3-非接IC位;4-打印位， 默认为接触位
	if (pSysConfig->DevConfig.nSSCardReaderPowerOnType == CardPowerType::READER_UNCONTACT)
		nPowerOnPos = 3;
	else if (pSysConfig->DevConfig.nSSCardReaderPowerOnType == CardPowerType::READER_CONTACT)
		nPowerOnPos = 2;
	char szRCode[1024] = { 0 };

	CloseSSCardReader();
	if (QFailed(OpenSSCardReader(strMessage)))
	{
		gInfo() << gQStr(strMessage);
		return -1;
	}

	// 移动到打印位
	if (m_pPrinter->Printer_Dispense(pSysConfig->DevConfig.nDepenseBox - 1, 4, szRCode))
	{
		strMessage = QString("Printer_Dispense(4)失败，错误代码:%1!").arg(szRCode);
		gInfo() << gQStr(strMessage);
		return -1;
	}
	// 移动到上电位
	if (m_pPrinter->Printer_Dispense(pSysConfig->DevConfig.nDepenseBox - 1, nPowerOnPos, szRCode))
	{
		strMessage = QString("Printer_Dispense(%1)失败，错误代码:%2!").arg(nPowerOnPos).arg(szRCode);
		gInfo() << gQStr(strMessage);
		return -1;
	}
	return 0;
}

int DataCenter::ReadCard(SSCardBaseInfoPtr& pSSCardInfo, QString& strMessage)
{
	if (!m_pSSCardReader)
	{
		strMessage = "读卡器未就绪!";
		gInfo() << strMessage.toLocal8Bit().data();
		return -1;
	}
	int nResult = -1;
	DeviceConfig& DevConfig = pSysConfig->DevConfig;
	char szRCode[32] = { 0 };
	char szCardBaseRead[1024] = { 0 };
	char szCardATR[1024] = { 0 };
	int nCardATRLen = 0;
	QString strInfo;

	do
	{
		if (!m_pSSCardReader)
		{
			strMessage = "读卡器未就绪!";
			break;
		}
		gInfo() << "Try to Reader_PowerOn";
		if (QFailed(nResult = g_pDataCenter->GetSSCardReader()->Reader_PowerOn(DevConfig.nSSCardReaderPowerOnType, szCardATR, nCardATRLen, szRCode)))
		{
			strMessage = QString("IC卡上电失败,PowerOnType:%1,nResult:%2").arg((int)DevConfig.nSSCardReaderPowerOnType).arg(nResult);
			nResult = -4;
			break;
		}
		gInfo() << "CardATR:" << szCardATR;
		pSSCardInfo->strCardATR = szCardATR;
		char szBankNum[64] = { 0 };
		gInfo() << "Try to iReadBankNumber";
		if (QFailed(nResult = iReadBankNumber(DevConfig.nSSCardReaderPowerOnType, szBankNum)))
		{
			strMessage = QString("读银行卡信息失败,PowerOnType:%1,nResult:%2").arg((int)DevConfig.nSSCardReaderPowerOnType).arg(nResult);
			break;
		}
		gInfo() << "BankNum:" << szBankNum;
		pSSCardInfo->strBankNum = szBankNum;
		gInfo() << "Try to iReadCardBas";
		if (QFailed(nResult = iReadCardBas(DevConfig.nSSCardReaderPowerOnType, szCardBaseRead)))
		{
			strMessage = QString("读取卡片基本信息失败,PowerOnType:%1,resCode:%2").arg((int)DevConfig.nSSCardReaderPowerOnType).arg(szRCode);
			break;
		}
		/*
		发卡地区行政区划代码（卡识别码前6位）、社会保障号码、 卡号、 卡识别码、 姓名、 卡复位信息（仅取历史字节）、 规范版本、 发卡日期、 卡有效期、终端机编号、终端设备号。各数据项之间以 “ | ” 分割，且最后一个数据项以 “ I,, 结尾。
		639900|l111111198101011110|X00000019|639900D15600000500BF7C7A48FB4966|张三|00814E43238697159900BF7C7A|1.00|20101001
		区号|社会保障号码(空)|卡号(空)|卡识别码|姓名(空)|卡复位信息(ATR)|Ver|发卡日期|有效期限|
		411600|||411600D156000005C38275EEFC9B9648||008C544CB386844116018593CA|3.00|20211203|20311202|
		*/
		gInfo() << "CardBase = " << szCardBaseRead;
		QStringList strFieldList = QString(szCardBaseRead).split("|", Qt::KeepEmptyParts);
		int nIndex = 0;
		for (auto var : strFieldList)
		{
			qDebug() << "Field[" << nIndex << "]" << var;
			nIndex++;
		}
		pSSCardInfo->strCardVersion = strFieldList[6].toStdString();
		gInfo() << "CardBaseRead:" << szCardBaseRead;
		char szRespond[1024] = { 0 };
		char szMacKey[4096] = { 0 };
		gInfo() << "Try to iReadCardBas_HSM_Step1";
		if (QFailed(nResult = iReadCardBas_HSM_Step1(szMacKey)))
		{
			strMessage = QString("iReadCardBas_HSM_Step1失败,PowerOnType:%1,resCode:%2").arg((int)DevConfig.nSSCardReaderPowerOnType).arg(szRCode);
			break;
		}

		strFieldList = QString(szMacKey).split("|", Qt::KeepEmptyParts);
		for (auto var : strFieldList)
		{
			qDebug() << "Field[" << nIndex << "]" << var;
			nIndex++;
		}

		HSMInfo hsmInfo;
		pSSCardInfo->strCardIdentity = strFieldList[3].toStdString();
		strcpy(hsmInfo.strCardATR, pSSCardInfo->strCardATR.c_str());

		string strCardRegion = strFieldList[0].toStdString();
		strcpy(hsmInfo.strRegionCode, strCardRegion.c_str());
		strcpy(hsmInfo.stralgoCode, "03");
		strcpy(hsmInfo.strKeyAddr, "RKMFEF0C_370000");
		strcpy(hsmInfo.strIdentifyNum, pSSCardInfo->strCardIdentity.c_str());
		strcpy(hsmInfo.strRandom1, strFieldList[4].toStdString().c_str());
		strcpy(hsmInfo.strRandom2, strFieldList[5].toStdString().c_str());
		strcpy(hsmInfo.strRandom3, strFieldList[6].toStdString().c_str());
		strcpy(hsmInfo.strRandom4, strFieldList[7].toStdString().c_str());

		char szKey[256] = { 0 };
		gInfo() << "Try to InnerAuth";
		if (QFailed(nResult = InnerAuth(hsmInfo, szKey)))
		{
			strMessage = QString("读取医保信息失败,InnerAuth,resCode:%1").arg(nResult);
			break;
		}

		strFieldList = QString(szKey).split("|", Qt::KeepEmptyParts);
		char szStep2Out[256] = { 0 };
		gInfo() << "Try to iReadCardBas_HSM_Step2";
		if (QFailed(nResult = iReadCardBas_HSM_Step2((char*)strFieldList[1].toStdString().c_str(), szStep2Out)))
		{
			strMessage = QString("读取卡信息失败,iReadCardBas_HSM_Step2,PowerOnType:%1,resCode:%2").arg((int)DevConfig.nSSCardReaderPowerOnType).arg(szRCode);
			break;
		}

		//if (QFailed(nResult = g_pDataCenter->GetSSCardReader()->Reader_PowerOn(DevConfig.nSSCardReaderPowerOnType, szCardATR, nCardATRLen, szRCode)))
		//{
		//	strMessage = QString("IC卡上电失败,PowerOnType:%1,nResult:%2").arg((int)DevConfig.nSSCardReaderPowerOnType).arg(nResult);
		//	nResult = -4;
		//	break;
		//}

		gInfo() << "Try to iReadCardBas";
		if (QFailed(nResult = iReadCardBas(DevConfig.nSSCardReaderPowerOnType, szCardBaseRead)))
		{
			strMessage = QString("第二次读取卡片基本信息失败,PowerOnType:%1,resCode:%2").arg((int)DevConfig.nSSCardReaderPowerOnType).arg(szRCode);

			break;
		}

		gInfo() << "Try to iGetMedicalNum_Step1";
		if (QFailed(nResult = iGetMedicalNum_Step1(szRespond)) || strFieldList.size() == 0)
		{
			strMessage = QString("读取医保信息失败,iGetMedicalNum_Step1,resCode:%1").arg(nResult);
			break;
		}
		strFieldList = QString(szRespond).split("|", Qt::KeepEmptyParts);
		strcpy(hsmInfo.strRandom1, strFieldList[0].toStdString().c_str());
		strcpy(hsmInfo.strRandom2, strFieldList[1].toStdString().c_str());
		int step = 2;
		gInfo() << "Try to cardExternalAuth";
		if (QFailed(nResult = cardExternalAuth(hsmInfo, szKey,step)))
		{
			strMessage = QString("读取医保信息失败,cardExternalAuth,resCode:%1").arg(nResult);
			break;
		}

		ZeroMemory(szRespond, 1024);
		gInfo() << "Try to iGetMedicalNum_Step2";
		if (QFailed(nResult = iGetMedicalNum_Step2(szKey, szRespond)))
		{
			strMessage = QString("读取医保信息失败,iGetMedicalNum_Step2,resCode:%1").arg(nResult);
			break;
		}
		gInfo() << "MedicalNum = " << szRespond;
		if (szRespond[0] < '0' || szRespond[0] > '9')
			pSSCardInfo->strChipNum = (char*)&szRespond[1];
		else
			pSSCardInfo->strChipNum = szRespond;
		nResult = 0;
	} while (0);
	if (QFailed(nResult))
		gError() << gQStr(strMessage);
	return nResult;
}

int DataCenter::WriteCard(SSCardBaseInfoPtr& pSSCardInfo, QString& strMessage)
{
	int nResult = -1;
	DeviceConfig& DevConfig = pSysConfig->DevConfig;
	RegionInfo& Region = pSysConfig->Region;
	char szRCode[32] = { 0 };
	//char szCardBaseRead[1024] = { 0 };
	char szCardBaseWrite[1024] = { 0 };
	char szExAuthData[1024] = { 0 };
	char szWHSM1[1024] = { 0 };
	/*char szWHSM2[1024] = { 0 };*/
	char szSignatureKey[1024] = { 0 };
	char szWriteCARes[1024] = { 0 };
	char szCardATR[1024] = { 0 };
	int nCardATRLen = 0;
	QString strInfo;
	HSMInfo  HsmInfo;
	string strStage = "WriteCard";
	string strDatagram;
	do
	{
		if (!m_pSSCardReader)
		{
			strMessage = "读卡器未就绪!";
			break;
		}
		if (!pSScardSerivce)
		{
			strMessage = "卡管服务不可用!";
			break;
		}

		if (pSScardSerivce->GetStageStatus(strStage, nResult, strDatagram) && nResult == 0)
		{// 与之前写入成功的卡片的很行卡号进行核对
			string strLastBankNum;
			CJsonObject json(strDatagram);
			json.Get("BankNum", strLastBankNum);
			if (strLastBankNum == pSSCardInfo->strBankNum)
			{
				strMessage = "写卡操作已经完成,无可重复操作!";
				gInfo() << gQStr(strMessage);
				break;
			}
		}
		nResult = -1;
		gInfo() << "Try to Reader_PowerOn";
		if (QFailed(nResult = m_pSSCardReader->Reader_PowerOn(DevConfig.nSSCardReaderPowerOnType, (char*)szCardATR, nCardATRLen, szRCode)))
		{
			strMessage = QString("IC卡上电失败,PowerOnType:%1,nResult:%2").arg((int)DevConfig.nSSCardReaderPowerOnType).arg(nResult);
			nResult = -4;
			break;
		}
		pSSCardInfo->strCardATR = szCardATR;
		gInfo() << "CardATR:" << pSSCardInfo->strCardATR;
		gInfo() << "Try to iWriteCardBas";
		if (QFailed(nResult = iWriteCardBas(DevConfig.nSSCardReaderPowerOnType, szCardBaseWrite)))
		{
			strMessage = QString("写取卡片基本信息失败,PowerOnType:%1,resCode:%2").arg((int)DevConfig.nSSCardReaderPowerOnType).arg(szRCode);
			nResult = -4;
			break;
		}
		// szCardBaseWrite输出结果为两个随机数
		// random1|random2
		gInfo() << "CardBaseRead:" << szCardBaseWrite;

		QStringList strRomdom = QString(szCardBaseWrite).split("|", Qt::KeepEmptyParts);

		//strcpy(HsmInfo.strSystemID, "410700006");
		strcpy(HsmInfo.strRegionCode, Region.strCityCode.c_str());
		strcpy(HsmInfo.strCardNum, pSSCardInfo->strCardNum.c_str()); //用最新的卡号	   
		//strcpy(HsmInfo.strTerminalCode, DevConfig.strTerminalCode.c_str());	// 
		strcpy(HsmInfo.strCardID, pSSCardInfo->strIdentity.c_str());
		strcpy(HsmInfo.strBusinessType, "026");	// 写卡为026，读卡为028
		strcpy(HsmInfo.strIdentifyNum, pSSCardInfo->strCardIdentity.c_str());
		strcpy(HsmInfo.strName, pSSCardInfo->strName.c_str());
		strcpy(HsmInfo.strCardATR, pSSCardInfo->strCardATR.c_str());
		strcpy(HsmInfo.stralgoCode, "03");
		strcpy(HsmInfo.strKeyAddr, "0094");
		strcpy(HsmInfo.strRandom1, strRomdom[1].toStdString().c_str());
		strcpy(HsmInfo.strRandom2, strRomdom[2].toStdString().c_str());
		gInfo() << "Try to cardExternalAuth";
		if (QFailed(nResult = cardExternalAuth(HsmInfo, szExAuthData,2)))
		{
			strMessage = QString("卡版外部信息认证失败,Result:%1").arg(nResult);
			break;
		}
		int nLen = strlen(szExAuthData);
		if (nLen)
			szExAuthData[nLen - 1] = '\0';
		gInfo() << "szExAuthData = " << szExAuthData;
		string strCardInfo = MakeCardInfo(pSSCardInfo);
		gInfo() << "strCardInfo = " << strCardInfo;
		gInfo() << "Try to iWriteCardBas_HSM_Step1";
		if (QFailed(nResult = iWriteCardBas_HSM_Step1((char*)szExAuthData, (char*)strCardInfo.c_str(), szWHSM1)))
		{
			strMessage = QString("iWriteCardBas_HSM_Step1,PowerOnType:%1,resCode:%2").arg((int)DevConfig.nSSCardReaderPowerOnType).arg(szWHSM1);
			nResult = -4;
			break;
		}
		gInfo() << "szWHSM1 = " << szWHSM1;
		gInfo() << "Try to iReadSignatureKey";
		if (QFailed(nResult = iReadSignatureKey(DevConfig.nSSCardReaderPowerOnType, szSignatureKey)))
		{
			strMessage = QString("iReadSignatureKey,PowerOnType:%1,resCode:%2").arg((int)DevConfig.nSSCardReaderPowerOnType).arg(szRCode);
			nResult = -4;
			break;
		}
		gInfo() << "szSignatureKey = " << szSignatureKey;
		string strPublicKey = szSignatureKey;
		string strAlgorithm = "03";

		// CardID,Name,City,CardNum,SignatureKey

		CJsonObject jsonIn;
		jsonIn.Add("CardID", pSSCardInfo->strIdentity);
		jsonIn.Add("Name", pSSCardInfo->strName);
		jsonIn.Add("City", pSSCardInfo->strCity);
		jsonIn.Add("CardNum", pSSCardInfo->strCardNum);
		jsonIn.Add("SignatureKey", strPublicKey);
		string strJsonIn = jsonIn.ToString();
		string strJsonOut;
		gInfo() << "Try to GetCA";
		if (QFailed(nResult = pSScardSerivce->GetCA(strJsonIn, strJsonOut)))
		{
			CJsonObject jsonOut(strJsonOut);
			string strErrText;
			jsonOut.Get("Message", strErrText);
			strMessage = QString("GetCA失败:%1,PowerOnType:%2").arg(strErrText.c_str()).arg((int)DevConfig.nSSCardReaderPowerOnType);
			break;
		}
		CJsonObject jsonOut(strJsonOut);
		string strQMZS, strJMZS, strJMMY, strGLYPIN, strZKMY;
		jsonOut.Get("JMZS", strJMZS);
		jsonOut.Get("QMZS", strQMZS);
		jsonOut.Get("JMMY", strJMMY);
		jsonOut.Get("GLYPIN", strGLYPIN);
		jsonOut.Get("ZKMY", strZKMY);
		if (strJMZS.empty() || strQMZS.empty() || strJMMY.empty() || strGLYPIN.empty())
		{
			strMessage = QString("签名证书,加密证书,加密密钥或管理密码中有一项或多项为空!");
			break;
		}

		int nWriteCardCount = 0;
		nResult = -1;
		while (nWriteCardCount < 3)
		{
			strInfo = QString("尝试第%1次CA!").arg(nWriteCardCount + 1);
			gInfo() << gQStr(strInfo);
			//QMZS：签名证书 ,JMZS：加密证书 ,JMMY：加密密钥 ,GLYPIN：管理员pin ,ZKMY：主控密钥 ,pOutInfo 传出信息
			gInfo() << "Try to iWriteCA";
			nResult = iWriteCA(DevConfig.nSSCardReaderPowerOnType, (char*)strQMZS.c_str(), (char*)strJMZS.c_str(), (char*)strJMMY.c_str(), (char*)strGLYPIN.c_str(), (char*)strZKMY.c_str(), szWriteCARes);
			if (QSucceed(nResult))
			{
				break;
			}
			else
			{
				strMessage = QString("写CA信息失败,PowerOnType:%1,nResult:%2").arg((int)DevConfig.nSSCardReaderPowerOnType).arg(nResult);
				gInfo() << gQStr(strMessage);
				nWriteCardCount++;
				strInfo = "写卡上电失败,尝试移动卡片!";
				gInfo() << gQStr(strInfo);
				g_pDataCenter->MoveCard(strMessage);
				continue;
			}
		}
		if (QFailed(nResult))
		{
			break;
		}

		gInfo() << "szWriteCA Out = " << szWriteCARes;
		CJsonObject json;
		json.Add("BankNum", pSSCardInfo->strBankNum);
		pSScardSerivce->UpdateStage(strStage, 0, json.ToFormattedString());
		nResult = 0;

	} while (0);
	return nResult;
}

int DataCenter::WriteCardTest(SSCardBaseInfoPtr& pSSCardInfo, QString& strMessage, QWidget* pParent)
{
	int nResult = -1;
	DeviceConfig& DevConfig = pSysConfig->DevConfig;
	RegionInfo& Region = pSysConfig->Region;
	char szRCode[32] = { 0 };
	char szCardBaseRead[1024] = { 0 };
	char szCardBaseWrite[1024] = { 0 };
	char szExAuthData[1024] = { 0 };
	char szWHSM1[1024] = { 0 };
	//char szWriteCARes[1024] = { 0 };
	/*char szWHSM2[1024] = { 0 };*/
	char szSignatureKey[1024] = { 0 };
	char szCardATR[1024] = { 0 };
	int nCardATRLen = 0;
	QString strInfo;
	HSMInfo  HsmInfo;
	string strStage = "WriteCard";
	string strDatagram;
	do
	{
		if (!m_pSSCardReader)
		{
			strMessage = "读卡器未就绪!";
			break;
		}

		nResult = -1;
		gInfo() << "Try to Power_on";
		if (QFailed(nResult = m_pSSCardReader->Reader_PowerOn(DevConfig.nSSCardReaderPowerOnType, (char*)szCardATR, nCardATRLen, szRCode)))
		{
			strMessage = QString("IC卡上电失败,PowerOnType:%1,nResult:%2").arg((int)DevConfig.nSSCardReaderPowerOnType).arg(nResult);
			nResult = -4;
			break;
		}
		pSSCardInfo->strCardATR = szCardATR;
		gInfo() << "CardATR:" << pSSCardInfo->strCardATR;

		gInfo() << "Try to iReadCardBas";
		if (QFailed(nResult = iReadCardBas(DevConfig.nSSCardReaderPowerOnType, szCardBaseRead)))
		{
			strMessage = QString("读取卡片基本信息失败,PowerOnType:%1,resCode:%2").arg((int)DevConfig.nSSCardReaderPowerOnType).arg(szRCode);
			break;
		}
		int nIndex = 0;
		QStringList strFieldList = QString(szCardBaseRead).split("|", Qt::KeepEmptyParts);
		for (auto var : strFieldList)
		{
			qDebug() << "Field[" << nIndex << "]" << var;
			nIndex++;
		}

		HSMInfo hsmInfo;
		pSSCardInfo->strCardIdentity = strFieldList[3].toStdString();
		strcpy(hsmInfo.strCardATR, pSSCardInfo->strCardATR.c_str());
		strCardVersion = strFieldList[6].toStdString();
		gInfo() << "Try to iWriteCardBas";
		if (QFailed(nResult = iWriteCardBas(DevConfig.nSSCardReaderPowerOnType, szCardBaseWrite)))
		{
			strMessage = QString("读取卡片基本信息失败,PowerOnType:%1,resCode:%2").arg((int)DevConfig.nSSCardReaderPowerOnType).arg(szRCode);
			break;
		}
		// szCardBaseWrite输出结果为两个随机数
		// random1|random2
		gInfo() << "CardBaseRead:" << szCardBaseWrite;

		QStringList strRomdom = QString(szCardBaseWrite).split("|", Qt::KeepEmptyParts);
		//strcpy(HsmInfo.strSystemID, "410700006");
		strcpy(HsmInfo.strRegionCode, Region.strCityCode.c_str());
		strcpy(HsmInfo.strCardNum, pSSCardInfo->strCardNum.c_str()); //用最新的卡号	   
		//strcpy(HsmInfo.strTerminalCode, DevConfig.strTerminalCode.c_str());	// 
		strcpy(HsmInfo.strCardID, pSSCardInfo->strIdentity.c_str());
		strcpy(HsmInfo.strBusinessType, "026");	// 写卡为026，读卡为028
		strcpy(HsmInfo.strIdentifyNum, pSSCardInfo->strCardIdentity.c_str());
		strcpy(HsmInfo.strName, pSSCardInfo->strName.c_str());
		strcpy(HsmInfo.strCardATR, pSSCardInfo->strCardATR.c_str());
		strcpy(HsmInfo.stralgoCode, "03");
		strcpy(HsmInfo.strKeyAddr, "0094");
		strcpy(HsmInfo.strRandom1, strRomdom[1].toStdString().c_str());
		strcpy(HsmInfo.strRandom2, strRomdom[2].toStdString().c_str());

		gInfo() << "Try to cardExternalAuth";
		if (QFailed(nResult = cardExternalAuth(HsmInfo, szExAuthData,2)))
		{
			strMessage = QString("卡片外部信息认证失败,Result:%1").arg(nResult);
			break;
		}
		if (strstr(szExAuthData, "HSM_SERVER_ERROR"))
		{
			strMessage = "加密服务器错误:HSM_SERVER_ERROR";
			break;
		}
		int nLen = strlen(szExAuthData);
		if (nLen)
			szExAuthData[nLen - 1] = '\0';
		gInfo() << "szExAuthData = " << szExAuthData;
		string strCardInfo = MakeCardInfo(pSSCardInfo);
		gInfo() << "strCardInfo = " << strCardInfo;

		gInfo() << "Try to iWriteCardBas_HSM_Step1";
		if (QFailed(nResult = iWriteCardBas_HSM_Step1((char*)szExAuthData, (char*)strCardInfo.c_str(), szWHSM1)))
		{
			strMessage = QString("iWriteCardBas_HSM_Step1,PowerOnType:%1,resCode:%2").arg((int)DevConfig.nSSCardReaderPowerOnType).arg(szWHSM1);
			break;
		}
		gInfo() << "szWHSM1 = " << szWHSM1;

		gInfo() << "Try to iReadSignatureKey";
		if (QFailed(nResult = iReadSignatureKey(DevConfig.nSSCardReaderPowerOnType, szSignatureKey)))
		{
			strMessage = QString("iReadSignatureKey,PowerOnType:%1,resCode:%2").arg((int)DevConfig.nSSCardReaderPowerOnType).arg(szRCode);
			break;
		}

		gInfo() << "szSignatureKey = " << szSignatureKey;
		string strPublicKey = szSignatureKey;
		string strAlgorithm = "03";

		CJsonObject jsonIn;
		jsonIn.Add("CardID", pSSCardInfo->strIdentity);
		jsonIn.Add("Name", pSSCardInfo->strName);
		jsonIn.Add("City", pSSCardInfo->strCity);
		jsonIn.Add("CardNum", pSSCardInfo->strCardNum);
		jsonIn.Add("SignatureKey", strPublicKey);
		string strJsonIn = jsonIn.ToString();
		string strJsonOut;
		gInfo() << "Try to GetCA";
		//if (QFailed(nResult = pSScardSerivce->GetCA(strJsonIn, strJsonOut)))
		//{
		//	CJsonObject jsonOut(strJsonOut);
		//	string strErrText;
		//	jsonOut.Get("Message", strErrText);
		//	strMessage = QString("GetCA失败:%1,PowerOnType:%2").arg(QString::fromLocal8Bit(strErrText.c_str())).arg((int)DevConfig.nSSCardReaderPowerOnType);
		//	break;
		//}
		//CJsonObject jsonOut(strJsonOut);
		//string strQMZS, strJMZS, strJMMY, strGLYPIN, strZKMY;
		//jsonOut.Get("JMZS", strJMZS);
		//jsonOut.Get("QMZS", strQMZS);
		//jsonOut.Get("JMMY", strJMMY);
		//jsonOut.Get("GLYPIN", strGLYPIN);
		//jsonOut.Get("ZKMY", strZKMY);
		//if (strJMZS.empty() || strQMZS.empty() || strJMMY.empty() || strGLYPIN.empty())
		//{
		//	strMessage = QString("签名证书,加密证书,加密密钥或管理密码中有一项或多项为空!");
		//	break;
		//}

		//gInfo() << gQStr(strInfo);
		////QMZS：签名证书 ,JMZS：加密证书 ,JMMY：加密密钥 ,GLYPIN：管理员pin ,ZKMY：主控密钥 ,pOutInfo 传出信息
		//gInfo() << "Try to iWriteCA";
		//nResult = iWriteCA(DevConfig.nSSCardReaderPowerOnType, (char*)strQMZS.c_str(), (char*)strJMZS.c_str(), (char*)strJMMY.c_str(), (char*)strGLYPIN.c_str(), (char*)strZKMY.c_str(), szWriteCARes);
		//if (QSucceed(nResult))
		//	break;
		//else
		//{
		//	strMessage = QString("写CA失败:%1,PowerOnType:%2,nResult:%3").arg(szWriteCARes).arg((int)DevConfig.nSSCardReaderPowerOnType).arg(nResult);
		//	gInfo() << gQStr(strMessage);
		//	break;
		//}

		nResult = 0;

	} while (0);
	return nResult;
}

int g_index = 0;

int WINAPI LDCam_VIS_FrameCallback(PVOID context, PUCHAR pFrameData, int nFrameLen)
{
	g_index++;
	return 0;
}

int WINAPI LDCam_NIR_FrameCallback(PVOID context, PUCHAR pFrameData, int nFrameLen)
{
	if (g_index == 10)
	{
		//WriteBMPFile("d:\\nir.bmp", pFrameData, 640, 480);
	}

	return 0;
}


int WINAPI LDCam_EventCallback(int nEventID, PVOID context, int nFrameStatus)
{
	QStackPage* pWidget = (QStackPage*)context;
	pWidget->emit LiveDetectStatusEvent(nEventID, nFrameStatus);
	return 0;
}

BOOL WriteBMPFile(const char* strFileName, PUCHAR pData, int nWidth, int nHeight)
{
	BITMAPFILEHEADER   bmfHdr;
	BITMAPINFOHEADER   bi;
	int nLen;

	FILE* f = NULL;

	BOOL bRet;

	nLen = nWidth * nHeight * 3;
	bi.biSize = sizeof(BITMAPINFOHEADER);
	bi.biWidth = nWidth;
	bi.biHeight = nHeight;
	bi.biPlanes = 1;
	bi.biBitCount = 24;
	bi.biCompression = BI_RGB;
	bi.biSizeImage = nLen;
	bi.biXPelsPerMeter = 0;
	bi.biYPelsPerMeter = 0;
	bi.biClrUsed = 0;
	bi.biClrImportant = 0;
	bmfHdr.bfType = 0x4D42;  // "BM"
	bmfHdr.bfSize = sizeof(BITMAPFILEHEADER) + bi.biSize + nLen;
	bmfHdr.bfReserved1 = 0;
	bmfHdr.bfReserved2 = 0;
	bmfHdr.bfOffBits = sizeof(BITMAPFILEHEADER) + bi.biSize;

	f = fopen(strFileName, "wb");
	if (NULL == f)
	{
		bRet = FALSE;
		goto done;
	}

	if (1 != fwrite(&bmfHdr, sizeof(BITMAPFILEHEADER), 1, f))
	{
		bRet = FALSE;
		goto done;
	}

	if (1 != fwrite(&bi, sizeof(BITMAPINFOHEADER), 1, f))
	{
		bRet = FALSE;
		goto done;
	}


	if (1 != fwrite(pData, nLen, 1, f))
	{
		bRet = FALSE;
		goto done;
	}

	bRet = TRUE;

done:

	if (f)
		fclose(f);

	return bRet;
}


//int DataCenter::FaceCamera_EventCallback(int nEventID, int nFrameStatus)
//{
//	if (nEventID == EVENT_OK)
//	{
//		ZeroMemory(pImageBuffer, nBufferSize);
//		int nFaceW, nFaceH;
//		int nDataLen = 0;
//		int nRet = 0;
//		do
//		{
//			nRet = DVTGKLDCam_GetImage(m_hCamera, IMG_RGB24, pImageBuffer, nBufferSize, &nDataLen);
//			if (nRet != LD_RET_OK)
//				break;
//
//			WriteBMPFile(strFullCapture.c_str(), pImageBuffer, m_nWidth, m_nHeight);
//			ZeroMemory(pImageBuffer, nBufferSize);
//			nRet = DVTGKLDCam_GetFaceImage(m_hCamera, IMG_RGB24, pImageBuffer, nBufferSize, &nFaceW, &nFaceH, &nDataLen);
//			if (nRet != LD_RET_OK)
//				break;
//			WriteBMPFile(strFaceCapture.c_str(), pImageBuffer, nFaceW, nFaceH);
//
//
//		} while (0);
//
//	}
//	else if (nEventID == EVENT_ONGOING)
//	{
//		qDebug("Detecting, event = %d iFrameStatus = %d \n", nEventID, nFrameStatus);
//	}
//	else
//	{
//		qDebug("Detect error, event = %d iFrameStatus = %d", nEventID, nFrameStatus);
//	}
//}

bool DataCenter::IsVideoStart()
{
	return m_bVideoStarted;
}

bool DataCenter::OpenCamera()
{
	int nRet = -1;
	do
	{
		if (m_hCamera)
		{
			nRet = LD_RET_OK;
			break;
		}

		nBufferSize = m_nWidth * m_nHeight * 4;
		pImageBuffer = new byte[nBufferSize];
		if (!pImageBuffer)
			break;

		nRet = DVTGKLDCam_Open(&m_hCamera);
		if (nRet != LD_RET_OK)
			break;

		char szProductInfo[8192] = { 0 };
		nRet = DVTGKLDCam_GetProduct(szProductInfo, 8192);
		qDebug() << szProductInfo;
		nRet = DVTGKLDCam_SetFrameCallback(m_hCamera, 0, LDCam_VIS_FrameCallback, this);
		if (nRet != LD_RET_OK)
			break;

		nRet = DVTGKLDCam_SetFrameCallback(m_hCamera, 1, LDCam_NIR_FrameCallback, this);
		if (nRet != LD_RET_OK)
			break;
	} while (0);
	if (nRet != LD_RET_OK)
		return false;
	else
		return true;
}

bool DataCenter::StartVideo(HWND hWnd)
{
	int nRet = -1;
	do
	{
		if (!m_hCamera)
			break;

		if (m_bVideoStarted)
			return m_bVideoStarted;

		if (!IsWindow(hWnd))
			break;
		nRet = DVTGKLDCam_SetCamFormat(m_hCamera, 0, LD_FMT_MJPG, m_nWidth, m_nHeight);
		if (nRet != LD_RET_OK)
			break;

		nRet = DVTGKLDCam_SetCamFormat(m_hCamera, 1, LD_FMT_MJPG, m_nWidth, m_nHeight);
		if (nRet != LD_RET_OK)
			break;
		RECT rtWnd;
		GetWindowRect(hWnd, &rtWnd);
		int nWndWidth = rtWnd.right - rtWnd.left;
		int nWndHeight = rtWnd.bottom - rtWnd.top;
		nRet = DVTGKLDCam_StartVideo(m_hCamera, (HWND)hWnd, 0, 0, nWndWidth, nWndHeight);
		if (nRet != LD_RET_OK)
			break;
		QString strText[] = { "人脸检测正在进行" ,"人脸检测失败，超时" ,"人脸检测成功" };
		DVTGKLDCam_SetDetectText(m_hCamera, TEXT_ONGOING, 30, (char*)strText[0].toLocal8Bit().toStdString().c_str());
		DVTGKLDCam_SetDetectText(m_hCamera, TEXT_FAIL_TIMEOUT, 30, (char*)strText[1].toLocal8Bit().toStdString().c_str());
		DVTGKLDCam_SetDetectText(m_hCamera, TEXT_SUCCESS, 30, (char*)strText[2].toLocal8Bit().toStdString().c_str());
		m_bVideoStarted = true;
	} while (0);
	return m_bVideoStarted;
}

void DataCenter::StopVideo()
{
	if (!m_hCamera)
		return;

	if (!m_bVideoStarted)
		return;

	DVTGKLDCam_StopVideo(m_hCamera);
	m_bVideoStarted = false;
}

void DataCenter::CloseCamera()
{
	if (m_hCamera)
	{
		StopFaceDetect();
		DVTGKLDCam_Close(m_hCamera);
		delete[]pImageBuffer;
		pImageBuffer = nullptr;
		m_hCamera = nullptr;
	}
}

bool DataCenter::StartFaceDetect(void* pContext, int nDetectMilliSeconds, int nTimeoutMilliSeconds)
{
	if (!m_hCamera)
		return false;

	if (LD_RET_OK != DVTGKLDCam_SetEventCallback(m_hCamera, LDCam_EventCallback, pContext))
		return false;

	if (LD_RET_OK != DVTGKLDCam_StartDetection(m_hCamera, nDetectMilliSeconds, nTimeoutMilliSeconds))
		return false;
	m_bDetectStarted = true;
	return true;
}

bool DataCenter::SaveFaceImage(string strPhotoFile, bool bFull)
{
	if (!m_hCamera)
		return false;
	int nDataLen = 0;
	bool bSucceed = false;
	ZeroMemory(pImageBuffer, nBufferSize);
	//if (bFull)
	//{
	try
	{
		if (fs::exists(strPhotoFile))
		{
			fs::remove(strPhotoFile);
		}
		int nRes = DVTGKLDCam_Snapshot(m_hCamera, IMG_JPG, pImageBuffer, nBufferSize, &nDataLen);
		if (nRes != LD_RET_OK)
			return false;
		ofstream ofs(strPhotoFile, ios::out | ios::binary);
		//WriteBMPFile(strPhotoFile.c_str(), pImageBuffer, m_nWidth, m_nHeight);
		ofs << string((char*)pImageBuffer, nDataLen);
		ofs.flush();
		bSucceed = true;
	}
	catch (std::exception& e)
	{
		gInfo() << "保存图片发生异常:" << e.what();
		bSucceed = false;
	}

	/*}
	else
	{
		int nFaceWidth = 0, nFaceHeight = 0;
		if (LD_RET_OK == DVTGKLDCam_GetFaceImage(m_hCamera, IMG_JPG, pImageBuffer, nBufferSize, &nFaceWidth, &nFaceHeight, &nDataLen))
		{
			WriteBMPFile(strPhotoFile.c_str(), pImageBuffer, nFaceWidth, nFaceHeight);
			bSucceed = true;
		}
	}*/
	return bSucceed;
}

bool DataCenter::FaceCompareByImage(string strFacePhoto1, string strFacePhoto2, float& dfSimilarity)
{
	if (!m_hCamera)
		return false;
	QFileInfo fi1(strFacePhoto1.c_str());

	if (!fi1.isFile())
		return false;

	QFileInfo fi2(strFacePhoto2.c_str());
	if (!fi2.isFile())
		return false;

	QImage FaceImage1(strFacePhoto1.c_str());
	if (FaceImage1.isNull())
		return false;

	QImage FaceImage2(strFacePhoto2.c_str());
	if (FaceImage1.isNull())
		return false;
	int nRet = DVTGKLDCam_FaceCompFeature(m_hCamera, strFacePhoto1.c_str(), strFacePhoto2.c_str(), FaceImage1.height(), FaceImage1.width(), FaceImage2.height(), FaceImage2.width(), &dfSimilarity);
	if (LD_RET_OK != nRet)
		return false;

	return true;
}

void DataCenter::StopFaceDetect()
{
	if (m_bDetectStarted)
	{
		DVTGKLDCam_StopDetection(m_hCamera);
		m_bDetectStarted = false;
	}
}

bool DataCenter::SwitchVideoWnd(HWND hWnd)
{
	if (!IsWindow(hWnd))
		return false;

	if (!m_hCamera)
		return false;

	RECT rtWnd;
	GetWindowRect(hWnd, &rtWnd);
	int nWndWidth = rtWnd.right - rtWnd.left;
	int nWndHeight = rtWnd.bottom - rtWnd.top;
	return LD_RET_OK == DVTGKLDCam_UpdateWindow(m_hCamera, hWnd, 0, 0, nWndWidth, nWndHeight);
}

bool DataCenter::Snapshot(string strFilePath)
{
	if (m_hCamera)
	{
		int nDataLen = 0;
		ZeroMemory(pImageBuffer, nBufferSize);
		int nRet = DVTGKLDCam_Snapshot(m_hCamera, IMG_RGB24, pImageBuffer, nBufferSize, &nDataLen);
		if (nRet == LD_RET_OK)
		{
			WriteBMPFile(strFilePath.c_str(), pImageBuffer, m_nWidth, m_nHeight);
			return true;
		}
		else
		{
			return false;
		}
	}
	else
		return false;
}

bool GetModuleVersion(QString strModulePath, WORD& nMajorVer, WORD& nMinorVer, WORD& nBuildNum, WORD& nRevsion)
{
	DWORD dwHnd;
	DWORD dwVerInfoSize;
	wchar_t pszVersion[2048] = { 0 };
	wchar_t szModulePath[4096] = { 0 };
	bool bResult = false;
	wcscpy_s(szModulePath, 4096, strModulePath.toStdWString().c_str());
	do
	{
		if (0 >= (dwVerInfoSize = GetFileVersionInfoSizeW(szModulePath, &dwHnd)))
		{
			break;
		}

		// get file version info
		if (!GetFileVersionInfoW(szModulePath, dwHnd, dwVerInfoSize, pszVersion))
		{
			break;
		}

		// Read the list of languages and code pages.
		struct LANGANDCODEPAGE
		{
			WORD    wLanguage;
			WORD    wCodePage;
		}*lpTranslate;
		unsigned int cbTranslate;
		if (!VerQueryValueW(pszVersion, L"\\VarFileInfo\\Translation", (void**)&lpTranslate, &cbTranslate))
		{
			break;
		}

		// get FileVersion string from resource
		VS_FIXEDFILEINFO* p_version;
		unsigned int version_len = 0;
		if (!VerQueryValue(pszVersion, L"\\", (void**)&p_version, &version_len))
		{
			break;
		}

		nMajorVer = (p_version->dwFileVersionMS >> 16) & 0x0000FFFF;
		nMinorVer = p_version->dwFileVersionMS & 0x0000FFFF;
		nBuildNum = (p_version->dwFileVersionLS >> 16) & 0x0000FFFF;
		nRevsion = p_version->dwFileVersionLS & 0x0000FFFF;
		qDebug() << szModulePath << "Version=" << nMajorVer << "." << nMinorVer << "." << nBuildNum << "." << nRevsion;
		bResult = true;
	} while (0);
	return bResult;
}

void EnableWidgets(QWidget* pUIObj, bool bEnable)
{
	if (!pUIObj)
		return;

	QObjectList list = pUIObj->children();
	if (pUIObj->inherits("QWidget"))
	{
		QWidget* pWidget = qobject_cast<QWidget*>(pUIObj);
		pWidget->setEnabled(bEnable);
		return;
	}
	else if (list.isEmpty())
	{
		return;
	}
	foreach(QObject * pObj, list)
	{
		qDebug() << pObj->metaObject()->className();
		EnableWidgets((QWidget*)pObj, bEnable);
	}
	pUIObj->setEnabled(true);
}

void ShowWidgets(QWidget* pUIObj, bool bShow)
{
	if (!pUIObj)
		return;

	QObjectList list = pUIObj->children();
	qDebug() << "Name = " << pUIObj->objectName();

	foreach(QObject * pObj, list)
	{
		qDebug() << pObj->metaObject()->className();
		ShowWidgets((QWidget*)pObj, bShow);
	}
	if (pUIObj->inherits("QWidget"))
	{
		QWidget* pWidget = qobject_cast<QWidget*>(pUIObj);
		pWidget->setVisible(bShow);
		return;
	}
}

char VerifyCardID(const char* pszSrc)
{
	int iS = 0;
	int iW[] = { 7, 9, 10, 5, 8, 4, 2, 1, 6, 3, 7, 9, 10, 5, 8, 4, 2 };
	static char szVerCode[] = "10X98765432";
	int i;
	for (i = 0; i < 17; i++)
	{
		iS += (int)(pszSrc[i] - '0') * iW[i];
	}
	int iY = iS % 11;
	return szVerCode[iY];
}

QString CardStatusString(CardStatus nCardStratus)
{
	switch (nCardStratus)
	{
	case CardStatus::Card_Release:
		return "放号";
		break;
	case CardStatus::Card_Normal:
		return  "正常";
		break;
	case CardStatus::Card_RegisterLost:
		return "挂失";
		break;
	case CardStatus::Card_RegisgerTempLost:
		return "临时挂失";
		break;
	case CardStatus::Card_CanceledOnRegisterLost:
		return "挂失后注销";
		break;
	case CardStatus::Card_Canceled:
		return "注销";
		break;
	case CardStatus::Card_DeActived:
		return "未启用";
		break;
	case CardStatus::Card_Making:
		return "制卡中";
		break;
	default:
	case CardStatus::Card_Unknow:
		return "未知";
		break;
	}
}

int  DataCenter::PremakeCard(QString& strMessage)
{
	LOG(INFO) << __FUNCTION__;
	int nResult = -1;
	string strJsonIn, strJsonOut;
	RegionInfo* pInfo = &(pSysConfig->Region);
	do
	{
		if (!pSSCardInfo || !pSScardSerivce)
			break;
		CJsonObject jsonIn;
		jsonIn.Add("Name", pSSCardInfo->strName);
		jsonIn.Add("CardID", pSSCardInfo->strIdentity);
		jsonIn.Add("City", pSSCardInfo->strCity);
		jsonIn.Add("BankCode", pSSCardInfo->strBankCode);
		jsonIn.Add("DocType", pSSCardInfo->strCardType);
		jsonIn.Add("DeviceSN", pInfo->strDeviceSN);
		jsonIn.Add("BankNodeCode", pInfo->strBankNodeCode);
		//2024 添加证件有效期和手机号字段
		string jStrPersonInfo;
		g_pDataCenter->GetSSCardService()->GetPersonInfoFromStage(jStrPersonInfo);
		CJsonObject jsonPersonInfo;
		jsonPersonInfo.Parse(jStrPersonInfo);
		string strPaperIssueDate{}, strPaperExpireDate{};
		jsonPersonInfo.Get("PaperIssuedate", strPaperIssueDate);
		jsonPersonInfo.Get("PaperExpiredate", strPaperExpireDate);
		jsonIn.Add("IssueDate", strPaperIssueDate);

		if (strPaperExpireDate == "长期")
			jsonIn.Add("ExpireDate", "99991231");
		else
			jsonIn.Add("ExpireDate", strPaperExpireDate);
		jsonIn.Add("Mobile", pSSCardInfo->strMobile);
		strJsonIn = jsonIn.ToString();
		gInfo() << "JsonIn = " << strJsonIn;
		bool bskip = false;

		if (!g_pDataCenter->bSwitchBank) {

			if (QFailed(pSScardSerivce->PreMakeCard(strJsonIn, strJsonOut)))
			{
				CJsonObject jsonOut(strJsonOut);
				string strErrText;
				jsonOut.Get("Message", strErrText);
				strMessage = QString("预制卡开户失败:%1").arg(QString::fromLocal8Bit(strErrText.c_str()));
				break;
			}
		}
		else {

			if (QFailed(pSScardSerivce->PreMakeCardHk(strJsonIn, strJsonOut)))
			{
				CJsonObject jsonOut(strJsonOut);
				string strErrText;
				jsonOut.Get("Message", strErrText);
				strMessage = QString("跨行换卡预制卡开户失败:%1").arg(QString::fromLocal8Bit(strErrText.c_str()));
				break;
			}
		}
		
		gInfo() << "JsonOut = " << strJsonOut;
		CJsonObject jsonOut(strJsonOut);
		if (!jsonOut.KeyExist("CardNum") ||
			!jsonOut.KeyExist("Photo"))
		{
			LOG(INFO) << "未能获取社保卡号码和照片数据!";
			if (QFailed(pSScardSerivce->QueryCardInfo0(strJsonIn, strJsonOut)))
			{
				CJsonObject jsonOut(strJsonOut);
				string strErrText;
				jsonOut.Get("Message", strErrText);
				strMessage = QString("查询预制卡信息失败:%1").arg(QString::fromLocal8Bit(strErrText.c_str()));
				break;
			}
			CJsonObject tmpJson1(strJsonOut);
			tmpJson1.Get("CardNum", pSSCardInfo->strCardNum);
			string strJsonQuery = jsonIn.ToString();
			string strCommand = "QueryPersonPhoto";

			if (QFailed(pSScardSerivce->SetExtraInterface(strCommand, strJsonQuery, strJsonOut)))
			{
				CJsonObject jsonOut(strJsonOut);
				string strText;
				int nErrCode = -1;
				jsonOut.Get("Result", nErrCode);
				jsonOut.Get("Message", strText);
				strMessage = QString("获取个人照片失败:%1").arg(QString::fromLocal8Bit(strText.c_str()));
				return -1;
			}
			CJsonObject tmpJson2(strJsonOut);
			tmpJson1.Get("Photo", pSSCardInfo->strCardNum);
		}
		else {
			string strPhoto;
			string strSSCardNum;
			jsonOut.Get("CardNum", pSSCardInfo->strCardNum);
			jsonOut.Get("Photo", pSSCardInfo->strPhoto);
		}
		//jsonOut.Get("NationalityCode", pSSCardInfo->strNationCode);
		jsonIn.Add("Nation", pSSCardInfo->strNationCode);
		SaveSSCardPhoto(strMessage, pSSCardInfo->strPhoto.c_str());

		stringstream date;
		auto tNow = chrono::system_clock::to_time_t(std::chrono::system_clock::now());
		// date = "2022-01-26 09:51:00"
		// date << std::put_time(std::localtime(&tNow), "%Y-%m-%d %X");
		date << std::put_time(std::localtime(&tNow), "%Y%m%d");
		pSSCardInfo->strReleaseDate = date.str();
		nResult = 0;
	} while (0);
	return nResult;
}

int  DataCenter::LoadPhoto(string& strBase64Photo, QString& strMessage, PhotoType nType)
{
	LOG(INFO) << __FUNCTION__;
	string stPhotoFile = "";
	if (nType == PhotoType::Photo_CardID)
	{
		if (strSSCardPhotoFile.empty() ||
			strSSCardPhotoFile.find((char*)pIDCard->szIdentity) == string::npos)
		{
			gInfo() << "用户照片为空或照片文本名与当前用户身份证号码不符,准备下载照片!";
			return DownloadPhoto(strBase64Photo, strMessage);
		}
		else
			stPhotoFile = strSSCardPhotoFile;
	}
	else if (nType == PhotoType::Photo_Guardian)
		stPhotoFile = strGuardianPhotoFile;
	else
		return -1;

	if (stPhotoFile.empty() || !fs::exists(stPhotoFile))
		return -1;

	try
	{
		ifstream ifs(strSSCardPhotoFile, ios::in | ios::binary);
		stringstream ss;
		ss << ifs.rdbuf();
		// 读取jpg图片并转base64
		QByteArray ba(ss.str().c_str(), ss.str().size());
		QByteArray baBase64 = ba.toBase64();
		strBase64Photo = string(baBase64.data(), baBase64.size());
		return 0;
	}
	catch (std::exception& e)
	{
		gInfo() << e.what();
		return -1;
	}
}

int DataCenter::modifyPersonInfo(string& strBase64Photo, QString& strMessage)
{
	LOG(INFO) << __FUNCTION__;
	if (!pSScardSerivce)
	{
		strMessage = "卡管服务不可用!";
		return -1;
	}

	CJsonObject jsonQuery;
	jsonQuery.Clear();

	jsonQuery.Add("CardID", pSSCardInfo->strIdentity);
	jsonQuery.Add("Name", pSSCardInfo->strName);
	jsonQuery.Add("City", pSSCardInfo->strCity);

	string strJsonQuery = jsonQuery.ToString();
	string strJsonOut;
	string strCommand = "ModifyPersonInfo";

	if (QFailed(pSScardSerivce->SetExtraInterface(strCommand, strJsonQuery, strJsonOut)))
	{
		CJsonObject jsonOut(strJsonOut);
		string strText;
		int nErrCode = -1;
		jsonOut.Get("Result", nErrCode);
		jsonOut.Get("Message", strText);
		strMessage = QString("获取个人照片失败:%1").arg(QString::fromLocal8Bit(strText.c_str()));
		return -1;
	}
	CJsonObject jsonOut(strJsonOut);

	if (jsonOut.Get("Photo", strBase64Photo))
	{
		SaveSSCardPhoto(strMessage, strBase64Photo.c_str());
		if (QFailed(SaveSSCardPhotoBase64(strMessage, strBase64Photo.c_str())))
		{
			strMessage = QString("保存照片数据失败!");
			return -1;
		}
	}
	else
	{
		strMessage = QString("社保后台未返回个人照片!");
		return 1;
	}
	return 0;

}

int DataCenter::DownloadPhoto(string& strBase64Photo, QString& strMessage)
{
	LOG(INFO) << __FUNCTION__;
	if (!pSScardSerivce)
	{
		strMessage = "卡管服务不可用!";
		return -1;
	}

	CJsonObject jsonQuery;
	jsonQuery.Clear();

	jsonQuery.Add("CardID", pSSCardInfo->strIdentity);
	jsonQuery.Add("Name", pSSCardInfo->strName);
	jsonQuery.Add("City", pSSCardInfo->strCity);
	jsonQuery.Add("DocType", pSSCardInfo->strCardType);

	string strJsonQuery = jsonQuery.ToString();
	string strJsonOut;
	string strCommand = "QueryPersonPhoto";

	if (QFailed(pSScardSerivce->SetExtraInterface(strCommand, strJsonQuery, strJsonOut)))
	{
		CJsonObject jsonOut(strJsonOut);
		string strText;
		int nErrCode = -1;
		jsonOut.Get("Result", nErrCode);
		jsonOut.Get("Message", strText);
		strMessage = QString("获取个人照片失败:%1").arg(QString::fromLocal8Bit(strText.c_str()));
		return -1;
	}
	CJsonObject jsonOut(strJsonOut);

	if (jsonOut.Get("Photo", strBase64Photo))
	{
		SaveSSCardPhoto(strMessage, strBase64Photo.c_str());
		if (QFailed(SaveSSCardPhotoBase64(strMessage, strBase64Photo.c_str())))
		{
			strMessage = QString("保存照片数据失败!");
			return -1;
		}
	}
	else
	{
		strMessage = QString("社保后台未返回个人照片!");
		return 1;
	}
	return 0;

}

int DataCenter::RegisterLost(QString& strMessage)
{
	LOG(INFO) << __FUNCTION__;
	if (!pSSCardInfo || !pSScardSerivce)
	{
		strMessage = "卡管服务或社保卡信息不可用!";
		return -1;
	}
	RegionInfo& RegInfo = pSysConfig->Region;
	CJsonObject jsonIn;

	jsonIn.Add("CardID", (const char*)pIDCard->szIdentity);
	jsonIn.Add("Name", (const char*)pIDCard->szName);
	jsonIn.Add("City", RegInfo.strCityCode);
	jsonIn.Add("BankCode", RegInfo.strBankCode);
	jsonIn.Add("CardNum", pSSCardInfo->strCardNum);
	jsonIn.Add("Operator", RegInfo.strOperator);
	jsonIn.Add("DocType", pSSCardInfo->strCardType);
	string strJsonIn = jsonIn.ToString();
	string strJsonout;
	int nResult = 0;

	if (QFailed(pSScardSerivce->RegisterLost(strJsonIn, strJsonout)))
	{
		CJsonObject jsonOut(strJsonout);
		string strText;
		jsonOut.Get("Result", nResult);
		jsonOut.Get("Message", strText);
		strMessage = QString("挂失失败:%1").arg(QString::fromLocal8Bit(strText.c_str()));
		gError() << gQStr(strMessage);
		return -1;
	}
	return 0;
}

int DataCenter::GetCardStatus(QString& strMessage)
{
	LOG(INFO) << __FUNCTION__;
	CJsonObject jsonIn;
	jsonIn.Add("CardID", (char*)pIDCard->szIdentity);
	jsonIn.Add("Name", (const char*)pIDCard->szName);
	jsonIn.Add("City", g_pDataCenter->GetSysConfigure()->Region.strCityCode);
	jsonIn.Add("BankCode", g_pDataCenter->GetSysConfigure()->Region.strBankCode);
	string strJsonIn = jsonIn.ToString();
	string strJsonOut;
	if (!pSScardSerivce)
	{
		strMessage = "卡管服务不可用!";
		return -1;
	}
	if (QFailed(pSScardSerivce->QueryCardStatus(strJsonIn, strJsonOut)))
	{
		CJsonObject jsonOut(strJsonOut);
		string strText, strErrcode;
		int nErrCode = -1;
		//jsonOut.Get("Result", nErrCode);
		jsonOut.Get("Message", strText);
		jsonOut.Get("errcode", strErrcode);
		nErrCode = strtol(strErrcode.c_str(), nullptr, 10);
		if ((nErrCode == 3) || (nErrCode == 4 && QString::fromStdString(strText).contains("放号")))	// 已经申请过,则继续制卡
		{
			strMessage = QString::fromLocal8Bit(strText.c_str());
			return 0;
		}
		else
		{
			strMessage = QString("查询制卡状态失败:%1").arg(QString::fromLocal8Bit(strText.c_str()));
		}
	}
	return 0;
}

int  DataCenter::CommitPersionInfo(QString& strMessage)
{
	LOG(INFO) << __FUNCTION__;
	int nResult = -1;
	RegionInfo* pInfo = &(pSysConfig->Region);
	pSSCardInfo->strBankCode = pSysConfig->Region.strBankCode;
	pSSCardInfo->strOperator = pSysConfig->Region.strOperator;
	string strJsonIn, strJsonOut;
	do
	{
		if (!pSSCardInfo || !pSScardSerivce)
		{
			strMessage = "卡管服务或社保卡信息不可用!";
			break;
		}
		CJsonObject jsonIn;
		string jStrPersonInfo;
		g_pDataCenter->GetSSCardService()->GetPersonInfoFromStage(jStrPersonInfo);
		CJsonObject jsonPersonInfo;
		jsonPersonInfo.Parse(jStrPersonInfo);
		string strPaperIssueDate{}, strPaperExpireDate{};
		jsonPersonInfo.Get("PaperIssuedate", strPaperIssueDate);
		jsonPersonInfo.Get("PaperExpiredate", strPaperExpireDate);
		jsonIn.Add("IssueDate", strPaperIssueDate);
		
		if (strPaperExpireDate == "长期")
			jsonIn.Add("ExpireDate", "99991231");
		else
			jsonIn.Add("ExpireDate", strPaperExpireDate);

		jsonIn.Add("Name", pSSCardInfo->strName);
		jsonIn.Add("CardID", pSSCardInfo->strIdentity);
		jsonIn.Add("Gender", pSSCardInfo->strGender);
		jsonIn.Add("City", pSSCardInfo->strCity);

		jsonIn.Add("Mobile", pSSCardInfo->strMobile);
		jsonIn.Add("Birthday", pSSCardInfo->strBirthday);
		jsonIn.Add("Nation", pSSCardInfo->strNationCode);
		//jsonIn.Add("Nation", (char*)pIDCardInfo->szNationaltyCode);
		jsonIn.Add("Address", pSSCardInfo->strAddress);
		jsonIn.Add("BankCode", pSSCardInfo->strBankCode);
		jsonIn.Add("Occupation", pSSCardInfo->strOccupType);
		jsonIn.Add("AuthorizeType", "07");		// 验证授权方式，01 人脸识别，02电子签名 03上传纸质授权 04读取身份证 05上传身份证正反面，06短信验证 07 授权文字声明勾选
		jsonIn.Add("JJRName", pInfo->strJJRName);
		jsonIn.Add("JJRID", pInfo->strJJRId);
		jsonIn.Add("JJRMobile", pInfo->strJJRMobile);
		jsonIn.Add("BankNodeName", pInfo->strBankNodeName);
		jsonIn.Add("DocType", pSSCardInfo->strCardType);

		//存放授权的截图
		QString imageString;
		string PS = "本人委托授权" + pSSCardInfo->strOperator + "为我委托办理社会保障卡，同意受托人在其权限范围对采集的信息进行使用。本人承认所提供制作社会保障卡制卡所需的信息、照片真实准确，且事先已明确告知并同意由本人承担委托办理社会保障卡资料收集、领卡业务的一切风险责任";
		AuthorizeMessage msg;
		msg.setWindowTitle("请确认是否授权");
		msg.setText(PS.c_str());
		msg.setStandardButtons(QMessageBox::Yes | QMessageBox::No);
		msg.setDefaultButton(QMessageBox::No);
		int ret = msg.exec();
		if (ret == QMessageBox::No)
		{
			strMessage = "用户拒绝授权，制卡终止";
			return 1;
		}
		else if(ret == QMessageBox::Ok)
		{
			bool bAuthor = msg.includeDetails();
			if (bAuthor)
			{
				QPixmap screenshot = msg.grab();
				QByteArray byteArray;
				QBuffer buffer(&byteArray);
				buffer.open(QIODevice::WriteOnly);
				screenshot.save(&buffer, "JPG");
				imageString = byteArray.toBase64();
			}
			else
			{
				strMessage = "用户未授权，制卡终止";
				return 1;
			}
		}
		//CJsonObject jsonAuthorize;
		//jsonAuthorize.AddEmptySubArray("AuthorizeData");
		jsonIn.AddEmptySubArray("AuthorizeData");
		if (g_pDataCenter->bGuardian)
		{
			jsonIn.Add("ByGuardian", 1);	// 启用监护人信息
			jsonIn.Add("Guardian", pSSCardInfo->strGuardianName);
			jsonIn.Add("GuardianShip", pSSCardInfo->strGuardianShip);
			jsonIn.Add("GuardianCardID", pSSCardInfo->strGuardianIDentity);
			jsonIn.Add("GuardianDocType", pSSCardInfo->strGuardianDocType);
			////加载监护人照片
			//string strGuaidianBase64;
			//
			//if QFailed(LoadPhoto(strGuaidianBase64, strMessage, PhotoType::Photo_Guardian))
			//	break;
			//jsonIn["AuthorizeData"].Add(strGuaidianBase64);
		}

		if QFailed(LoadPhoto(pSSCardInfo->strPhoto, strMessage))
			break;
		/*QString s = QString::fromUtf8(PS.c_str(), PS.size());
		QTextCodec* gb2312Codec = QTextCodec::codecForName("GB18030");
		QByteArray gb2312Arr = gb2312Codec->fromUnicode(s);
		
		jsonIn["AuthorizeData"].Add(gb2312Arr.toStdString());*/

		
		jsonIn["AuthorizeData"].Add(msg.text().toLocal8Bit().toStdString());
		jsonIn["AuthorizeData"].Add(imageString.toStdString());
		gInfo() << "JsonIn(without photo) = " << jsonIn.ToFormattedString();
		jsonIn.Add("Photo", pSSCardInfo->strPhoto);

		switch (g_pDataCenter->nCardServiceType)
		{
		case ServiceType::Service_NewCard:
		{
			LOG(INFO) << "newcard:  switch bank :" << g_pDataCenter->bSwitchBank;
			
			if (g_pDataCenter->bSwitchBank)
			{
				string yuan{ "" }, fen{ "" };
				//获取社保卡号
				strJsonIn = jsonIn.ToString();
				if (QFailed(pSScardSerivce->getCardInfo(strJsonIn, strJsonOut)))
				{
					CJsonObject jsonOut(strJsonOut);
					string strText, strErrcode;
					int nErrCode = -1;
					jsonOut.Get("Result", nErrCode);
					jsonOut.Get("Message", strText);
					jsonOut.Get("errcode", strErrcode);
					strMessage = QString("查询制卡状态失败:%1").arg(QString::fromLocal8Bit(strText.c_str()));
					return -1;
				}
				//查询余额，余额为零才可继续换卡流程
				if (QFailed(pSScardSerivce->QueryCardBalance(strJsonIn, strJsonOut)))
				{
					CJsonObject jsonOut(strJsonOut);
					string strText, strErrcode;
					int nErrCode = -1;
					//jsonOut.Get("Result", nErrCode);
					jsonOut.Get("Message", strText);
					jsonOut.Get("errcode", strErrcode);


					if (strErrcode == "1")
					{
						strMessage = QString("查询卡余额失败:%1").arg(QString::fromLocal8Bit(strText.c_str()));
						LOG(ERROR) << strMessage.toStdString();
						return -1;
					}
				}
				CJsonObject jsonOut(strJsonOut);
				jsonOut.Get("Yuan", yuan);
				jsonOut.Get("Fen", fen);
				if (QString(yuan.c_str()).contains("三要素"))
				{
					strMessage = "证件三要素输入不全";
					LOG(INFO) << strMessage.toStdString();
					return -1;
				}
				if ((QString(yuan.c_str()).toInt() != 0) || QString(fen.c_str()).toInt() != 0)
				{
					//存在余额，提示返回
					strMessage = "使用赛顺平台转出余额后办卡";
					LOG(INFO) << strMessage.toStdString();
					return -1;
				}
			}
		}
		break;
		case ServiceType::Service_ReplaceCard:
		{
			jsonIn.Add("CardNum", pSSCardInfo->strCardNum);
		}
		break;
		case ServiceType::Service_RegisterLost:
		{

			
		}
		break;
		case ServiceType::Service_ReplaceCardSwitchBank:
		{


		}
		break;
		default:
		{
			strMessage = "不支持的制卡服务!";
			gInfo() << gQStr(strMessage);
			break;
		}
		}

		strJsonIn = jsonIn.ToString();
		
		if (QFailed(pSScardSerivce->CommitPersonInfo(strJsonIn, strJsonOut)))
		{
			gInfo() << "JsonOut = " << strJsonOut;
			CJsonObject jsonOut(strJsonOut);
			string strErrText;
			string strErrcode;
			jsonOut.Get("Message", strErrText);
			jsonOut.Get("errcode", strErrcode);
			QString qstrErrText = QString::fromLocal8Bit(strErrText.c_str());
			if (qstrErrText.contains("申卡人存在有效的制卡申请")||
				qstrErrText.contains("已提交制卡申请") ||
				qstrErrText.contains("放号"))
			{
				nResult = 0;
				break;
			}
			strMessage = QString("提交用户信息失败:%1").arg(qstrErrText);
			nResult = -1;
			break;
		}
		gInfo() << "JsonOut = " << strJsonOut;
		nResult = 0;
	} while (0);
	return nResult;
}

int	 DataCenter::SafeReadCard(QString& strMessage)
{
	int nResult = -1;
	string strJsonIn, strJsonOut;
	QString strInfo;
	int nReadCardCount = 0;

	while (nReadCardCount < 3)
	{
		strInfo = QString("尝试第%1次读卡!").arg(nReadCardCount + 1);
		gInfo() << gQStr(strInfo);
		nResult = g_pDataCenter->ReadCard(pSSCardInfo, strMessage);
		if (QSucceed(nResult))
		{
			break;
		}
		else if (nResult == -4)
		{
			nReadCardCount++;
			strInfo = "读卡上电失败,尝试移动卡片!";
			gInfo() << gQStr(strInfo);
			g_pDataCenter->MoveCard(strMessage);
			continue;
		}
		else if (QFailed(nResult))
		{
			strMessage = "读卡失败!";
			break;
		}
		nResult = 0;
	}
	return nResult;
}

int	 DataCenter::SafeWriteCard(QString& strMessage)
{
	LOG(INFO) << __FUNCTION__;
	int nResult = -1;
	string strJsonIn, strJsonOut;
	QString strInfo;
	int nWriteCardCount = 0;

	while (nWriteCardCount < 3)
	{
		strInfo = QString("尝试第%1次写卡!").arg(nWriteCardCount + 1);
		gInfo() << gQStr(strInfo);
		nResult = g_pDataCenter->WriteCard(pSSCardInfo, strMessage);
		if (QSucceed(nResult))
		{
			break;
		}
		else if (nResult == -4)
		{
			nWriteCardCount++;
			strInfo = "写卡上电失败,尝试移动卡片!";
			gInfo() << gQStr(strInfo);
			g_pDataCenter->MoveCard(strMessage);
			continue;
		}
		else if (QFailed(nResult))
		{
			strMessage = "写卡失败!";
			break;
		}
		nResult = 0;
	}
	return nResult;
}

int	 DataCenter::SafeWriteCardTest(QString& strMessage)
{
	LOG(INFO) << __FUNCTION__;
	int nResult = -1;
	string strJsonIn, strJsonOut;
	QString strInfo;
	int nWriteCardCount = 0;

	while (nWriteCardCount < 3)
	{
		strInfo = QString("尝试第%1次写卡!").arg(nWriteCardCount + 1);
		gInfo() << gQStr(strInfo);
		nResult = g_pDataCenter->WriteCardTest(pSSCardInfo, strMessage);
		if (QSucceed(nResult))
			break;
		else if (nResult == -4)
		{
			nWriteCardCount++;
			strInfo = "写卡失败,尝试移动卡片!";
			gInfo() << gQStr(strInfo);
			g_pDataCenter->MoveCard(strMessage);
			continue;
		}
		else if (QFailed(nResult))
		{
			break;
		}
		nResult = 0;
	}
	return nResult;
}

int  DataCenter::EnsureData(QString& strMessage)
{
	LOG(INFO) << __FUNCTION__;
	int nResult = -1;
	QString strInfo;
	string strJsonIn, strJsonOut;
	string strBankNodeCode = "";
	RegionInfo* pInfo = &(pSysConfig->Region);
	gInfo() << __FUNCTION__;
	do
	{
		if (!pSSCardInfo || !pSScardSerivce)
		{
			strMessage = "卡管服务或社保卡信息不可用!";
			break;
		}
		CJsonObject jsonIn;
		jsonIn.Add("CardID", pSSCardInfo->strIdentity);
		jsonIn.Add("Name", pSSCardInfo->strName);
		jsonIn.Add("BankCode", pSSCardInfo->strBankCode);
		jsonIn.Add("City", pSSCardInfo->strCity);
		jsonIn.Add("CardNum", pSSCardInfo->strCardNum);
		jsonIn.Add("CardATR", pSSCardInfo->strCardATR);
		jsonIn.Add("CardIdentity", pSSCardInfo->strCardIdentity);
		jsonIn.Add("ChipNum", pSSCardInfo->strChipNum);
		jsonIn.Add("MagNum", pSSCardInfo->strBankNum);
		jsonIn.Add("CardVersion", pSSCardInfo->strCardVersion);
		jsonIn.Add("DocType", pSSCardInfo->strCardType);
		jsonIn.Add("ChipType", "80");

		
		jsonIn.Add("BankNodeName", pInfo->strBankNodeName);

		strJsonIn = jsonIn.ToString();
		gInfo() << "JsonIn = " << strJsonIn;
		if (QFailed(pSScardSerivce->ReturnData(strJsonIn, strJsonOut)))
		{
			gInfo() << "JsonOut = " << strJsonOut;
			CJsonObject jsonOut(strJsonOut);
			string strErrText;
			jsonOut.Get("Message", strErrText);
			strMessage = QString::fromLocal8Bit(strErrText.c_str());
			break;
		}
		gInfo() << "JsonOut = " << strJsonOut;
		strJsonOut.clear();
		if (QFailed(pSScardSerivce->QueryLKId(strJsonIn, strJsonOut)))
		{
			gInfo() << "JsonOut = " << strJsonOut;
			CJsonObject jsonOut(strJsonOut);
			string strErrText;
			jsonOut.Get("Message", strErrText);
			strMessage = "QueryLKId" + QString::fromLocal8Bit(strErrText.c_str());
			break;
		}
		
		CJsonObject jsonOut(strJsonOut);
		string strds;
		//jsonOut.Get("ds", strds);
		gInfo() << strds;
		CJsonObject jsonds = jsonOut["ds"];
			
		jsonds.Get("fkdid", strBankNodeCode);
		gInfo() << "bankNodeCode : " << strBankNodeCode;
		jsonIn.Add("BankNodeCode", strBankNodeCode);
		
		gInfo() << "JsonOut = " << strJsonOut;
		strJsonOut.clear();
		jsonIn.Add("JJRName", pSSCardInfo->strName);
		jsonIn.Add("JJRID", pSSCardInfo->strIdentity);

		jsonIn.Add("JJRMobile", pSSCardInfo->strMobile);
		strJsonIn = jsonIn.ToString();
		if (QFailed(pSScardSerivce->handToPerson(strJsonIn, strJsonOut)))
		{
			gInfo() << "JsonOut = " << strJsonOut;
			CJsonObject jsonOut(strJsonOut);
			string strErrText;
			jsonOut.Get("Message", strErrText);
			strMessage = "handtoGr" + QString::fromLocal8Bit(strErrText.c_str());
			break;
		}
		gInfo() << "JsonOut = " << strJsonOut;
		nResult = 0;
	} while (0);
	return nResult;
}



int  DataCenter::ActiveCard(QString& strMessage)
{
	LOG(INFO) << __FUNCTION__;
	int nResult = -1;
	QString strInfo;
	string strJsonIn, strJsonOut;

	do
	{
		if (!pSSCardInfo || !pSScardSerivce)
		{
			strMessage = "卡管服务或社保卡信息不可用!";
			break;
		}
		CJsonObject jsonIn;

		jsonIn.Add("CardID", pSSCardInfo->strIdentity);
		jsonIn.Add("Name", pSSCardInfo->strName);
		jsonIn.Add("BankCode", pSSCardInfo->strBankCode);
		jsonIn.Add("City", pSSCardInfo->strCity);
		jsonIn.Add("CardNum", pSSCardInfo->strCardNum);
		jsonIn.Add("MagNum", pSSCardInfo->strBankNum);
		strJsonIn = jsonIn.ToString();
		gInfo() << "JsonIn = " << strJsonIn;
		if (QFailed(pSScardSerivce->ActiveCard(strJsonIn, strJsonOut)))
		{
			gInfo() << "JsonOut = " << strJsonOut;
			CJsonObject jsonOut(strJsonOut);
			string strErrText;
			jsonOut.Get("Message", strErrText);
			strMessage = QString::fromLocal8Bit(strErrText.c_str());
			break;
		}
		gInfo() << "JsonOut = " << strJsonOut;
		nResult = 0;
	} while (0);
	return 0;
}

void DataCenter::RemoveTempPersonInfo()
{
	try
	{
		string strAppPath = QApplication::applicationDirPath().toLocal8Bit().data();
		strAppPath += "/data/TempPerson.json";
		if (fs::exists(strAppPath))
			fs::remove(strAppPath);
	}
	catch (std::exception& e)
	{
		gInfo() << "发生异常:" << e.what();
	}
}
// 需提前把要处理的图片放在./PhotoProcess目录下，并命名为1.jpg
int ProcessHeaderImage(QString& strHeaderPhoto, QString& strMessage)
{
	QWaitCursor Wait;
	QString strAppPath = QApplication::applicationDirPath();
	QString strCurrentPath = strAppPath + "/PhotoProcess";
	QString strProcessPath = strAppPath + "/PhotoProcess/MattingTool.bin";

	try
	{
		if (!fs::exists(strProcessPath.toStdString()))
		{
			strMessage = "找不到人像处理组件:MattingTool.bin";
			return -1;
		}

		QString strPhotoPath2 = strAppPath + "/PhotoProcess/2.jpg";

		if (fs::exists(strPhotoPath2.toStdString()))
			fs::remove(strPhotoPath2.toStdString());

		wchar_t wszCurrentPath[1024] = { 0 };
		wcscpy_s(wszCurrentPath, 1024, strCurrentPath.toStdWString().c_str());
		QProcess tProcess;
		tProcess.setCreateProcessArgumentsModifier([&](QProcess::CreateProcessArguments* args)
			{
				//args->flags |= CREATE_NEW_CONSOLE;
				args->startupInfo->dwFlags &= ~STARTF_USESTDHANDLES;
				args->startupInfo->dwFlags |= STARTF_USEFILLATTRIBUTE;
				args->startupInfo->dwFillAttribute = BACKGROUND_BLUE | FOREGROUND_RED | FOREGROUND_INTENSITY;
				args->currentDirectory = wszCurrentPath;
			});
		tProcess.start(strProcessPath);
		tProcess.waitForFinished();

		QFileInfo fi2(strPhotoPath2);
		if (!fi2.isFile())
		{
			strMessage = "人像处理失败!";
			return -1;
		}
		strHeaderPhoto = strPhotoPath2;
		return 0;

	}
	catch (std::exception& e)
	{
		gInfo() << "发生异常" << e.what();
	}
	return -1;
}

bool  DataCenter::InitializeDB(QString& strMessage)
{
	LOG(INFO) << __FUNCTION__;
	SQLiteDB = QSqlDatabase::addDatabase("QSQLITE");
	//设置数据库
	SQLiteDB.setDatabaseName("./Data/ChinaRegion.db");
	QFileInfo fi("./Data/ChinaRegion.db");

	if (!fi.isFile())
	{
		strMessage = QString("找不到文件:'%1'").arg(fi.absoluteFilePath());
		return false;
	}
	if (!SQLiteDB.open())
	{
		gInfo() << "SQL Driver:" << SQLiteDB.lastError().driverText().toStdString();
		gInfo() << "SQL Database:" << SQLiteDB.lastError().databaseText().toStdString();
		gInfo() << "SQL ErrorText:" << SQLiteDB.lastError().text().toStdString();
		strMessage = "打开地址数据库失败,请检查地址数据库!";
		return false;
	}
	return true;
}

#include <TlHelp32.h>
/*
*@brief 根据进程名获取进程ID
@param lpProcessName进程名称
*/
DWORD GetProcessHandle(LPCWSTR lpProcessName)//根据进程名查找进程PID 
{
	DWORD dwRet = 0;
	HANDLE hSnapShot = ::CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
	if (hSnapShot == INVALID_HANDLE_VALUE)
	{
		gInfo() << "获得进程快照失败,返回的GetLastError():" << ::GetLastError();
		return 0;
	}

	PROCESSENTRY32 pe32;//声明进程入口对象 
	pe32.dwSize = sizeof(PROCESSENTRY32);//填充进程入口对象大小 
	::Process32First(hSnapShot, &pe32);//遍历进程列表 
	do
	{
		if (!lstrcmp(pe32.szExeFile, lpProcessName))//查找指定进程名的PID 
		{
			dwRet = pe32.th32ProcessID;
			break;
		}
	} while (::Process32Next(hSnapShot, &pe32));
	::CloseHandle(hSnapShot);
	return dwRet;//返回 
}

typedef struct
{
	HWND    hMainWnd;     // 窗口句柄
	DWORD   dwProcessID;    // 进程ID
}EnumWindowsArg;

///< 枚举窗口回调函数
BOOL CALLBACK EnumWindowsProc(HWND hWnd, LPARAM lParam)
{
	EnumWindowsArg* pArg = (EnumWindowsArg*)lParam;
	DWORD  dwProcessID = 0;
	// 通过窗口句柄取得进程ID
	::GetWindowThreadProcessId(hWnd, &dwProcessID);
	if ((GetWindowLong(hWnd, GWL_STYLE) & WS_VISIBLE) &&
		GetParent(hWnd) == NULL &&
		(dwProcessID == pArg->dwProcessID))
	{
		pArg->hMainWnd = hWnd;
		return false;
	}
	// 没找到，继续找，返回TRUE
	return TRUE;
}

HWND GetWindowHwndByPID(DWORD dwProcessID)
{
	HWND hwndRet = NULL;
	EnumWindowsArg ewa;
	ewa.dwProcessID = dwProcessID;
	ewa.hMainWnd = NULL;
	EnumWindows(EnumWindowsProc, (LPARAM)&ewa);
	if (ewa.hMainWnd)
	{
		hwndRet = ewa.hMainWnd;
	}
	return hwndRet;
}

HWND GetTouchKeybroadWnd()
{
	static const WCHAR* szProcessName = L"TabTip.exe";
	DWORD nPID = GetProcessHandle(szProcessName);
	if (!nPID)
		return nullptr;
	return GetWindowHwndByPID(nPID);
}
