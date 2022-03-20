#include "Sys_sscardservicet.h"
#include "ui_Sys_sscardservicet.h"
#include "Gloabal.h"
#include "Payment.h"
#include <memory>
using namespace std;

SSCardServiceT::SSCardServiceT(QWidget* parent) :
	QWidget(parent),
	ui(new Ui::Sys_SSCardServiceT)
{
	ui->setupUi(this);
	pBtnGroup = new QButtonGroup(this);
	pBtnGroup->addButton(ui->radioButton_New, 0);
	pBtnGroup->addButton(ui->radioButton_ReplaceCard, 1);
}

SSCardServiceT::~SSCardServiceT()
{
	delete ui;
}

void SSCardServiceT::on_pushButton_LoadCardID_clicked()
{
	QString strMessage;
	IDCardInfoPtr pIDCard = make_shared<IDCardInfo>();
	SSCardBaseInfoPtr pSSCardInfo = make_shared<SSCardBaseInfo>();
	if (QFailed(LoadTestIDData(pIDCard, pSSCardInfo)))
	{
		QMessageBox_CN(QMessageBox::Information, "提示", "加载测试数据失败!", QMessageBox::Ok, this);
		return;
	}
	ui->label_Identity->setText((char*)pIDCard->szIdentity);
	ui->label_Name->setText(QString::fromLocal8Bit((char*)pIDCard->szName));
	g_pDataCenter->SetSSCardInfo(pSSCardInfo);
	g_pDataCenter->SetIDCardInfo(pIDCard);
	g_pDataCenter->OpenSSCardReader(strMessage);
	g_pDataCenter->OpenPrinter(strMessage);
}


void SSCardServiceT::on_pushButton_QueryCardStatus_clicked()
{
	CJsonObject jsonIn;
	SSCardService* pService = g_pDataCenter->GetSSCardService();
	IDCardInfoPtr& pIDCard = g_pDataCenter->GetIDCardInfo();
	SSCardBaseInfoPtr& pSSCardInfo = g_pDataCenter->GetSSCardInfo();
	jsonIn.Add("CardID", (char*)pIDCard->szIdentity);
#pragma  Warning("姓名是用什么编码?UTF-8 or GBK?")
	jsonIn.Add("Name", (const char*)pIDCard->szName);
	jsonIn.Add("City", g_pDataCenter->GetSysConfigure()->Region.strCityCode);
	jsonIn.Add("BankCode", g_pDataCenter->GetSysConfigure()->Region.strBankCode);
	string strJsonIn = jsonIn.ToString();
	string strJsonOut;
	//pService->SetExtraInterface("QueryPersonInfo", strJsonIn, strJsonOut);		// 查不到职业类型
	//pService->SetExtraInterface("QueryPersonPhoto", strJsonIn, strJsonOut);
	int nResult = 0;
	QString strMessage;
	string strText;
	do
	{
		jsonIn.Add("CardID", pSSCardInfo->strIdentity);
		jsonIn.Add("Name", pSSCardInfo->strName);
		jsonIn.Add("City", pSSCardInfo->strCity);
		jsonIn.Add("BankCode", pSSCardInfo->strBankCode);

		if (QFailed(pService->QueryCardStatus(strJsonIn, strJsonOut)))					// 查不到职业类型
		{
			CJsonObject jsonOut(strJsonOut);
			jsonOut.Get("Result", nResult);
			jsonOut.Get("Message", strText);

			//strMessage = QString("查询制卡信息失败:%1").arg(QString::fromLocal8Bit(strText.c_str()));
			break;
		}
		CJsonObject jsonOut(strJsonOut);


		if (!jsonOut.Get("Result", nResult) ||
			!jsonOut.Get("Message", strText))
		{
			gInfo() << strJsonOut;
			break;
		}

	} while (0);
	QMessageBox_CN(QMessageBox::Information, "提示", QString::fromLocal8Bit(strText.c_str()), QMessageBox::Ok, this);
}


void SSCardServiceT::on_pushButton_CommitInfo_clicked()
{
	QString strMessage;

	SSCardBaseInfoPtr& pSSCardInfo = g_pDataCenter->GetSSCardInfo();
	IDCardInfoPtr& pIDCardInfo = g_pDataCenter->GetIDCardInfo();
	RegionInfo& Reginfo = g_pDataCenter->GetSysConfigure()->Region;
	pSSCardInfo->strOrganID = Reginfo.strAgency;
	pSSCardInfo->strOrganID = Reginfo.strAgency;
	pSSCardInfo->strBankCode = Reginfo.strBankCode;
	pSSCardInfo->strTransType = "5";
	pSSCardInfo->strCity = Reginfo.strCityCode;
	pSSCardInfo->strSSQX = Reginfo.strCountry;
	pSSCardInfo->strCardVender = Reginfo.strCardVendor;
	pSSCardInfo->strBankCode = Reginfo.strBankCode;

	QString strInfo;
	SSCardService* pService = g_pDataCenter->GetSSCardService();
	string strJsonIn, strJsonOut;
	ServiceType nSvr = (ServiceType)pBtnGroup->checkedId();

	do
	{
		CJsonObject jsonIn;
		switch (nSvr)
		{
		case ServiceType::Service_NewCard:
		{
			if (g_pDataCenter->strSSCardPhotoFile.empty())
			{
				QMessageBox_CN(QMessageBox::Information, "提示", "未找到照片数据,请先下载照片", QMessageBox::Ok, this);
				return;
			}
			jsonIn.Add("Name", (char*)pIDCardInfo->szName);
			jsonIn.Add("CardID", (char*)pIDCardInfo->szIdentity);
			jsonIn.Add("City", Reginfo.strCityCode);
			jsonIn.Add("Mobile", pSSCardInfo->strMobile);
			jsonIn.Add("IssueDate", (char*)pIDCardInfo->szExpirationDate1);

			QString strExpireDate = QString::fromLocal8Bit((const char*)pIDCardInfo->szExpirationDate2);
			if (strExpireDate == "长期")
				jsonIn.Add("ExpireDate", "99991231");
			else
				jsonIn.Add("ExpireDate", (char*)pIDCardInfo->szExpirationDate2);

			jsonIn.Add("Mobile", pSSCardInfo->strMobile);
			jsonIn.Add("Birthday", (char*)pIDCardInfo->szBirthday);
			jsonIn.Add("Gender", (char*)pIDCardInfo->szGender);
			jsonIn.Add("Nation", (char*)pIDCardInfo->szNationaltyCode);
			jsonIn.Add("Address", (char*)pIDCardInfo->szAddress);
			jsonIn.Add("BankCode", Reginfo.strBankCode);
			ifstream ifs(g_pDataCenter->strSSCardPhotoFile, ios::in | ios::binary);
			stringstream ss;
			ss << ifs.rdbuf();
			QByteArray ba(ss.str().c_str(), ss.str().size());
			QByteArray baBase64 = ba.toBase64();
			jsonIn.Add("Photo", baBase64.data());
		}
		break;
		case ServiceType::Service_ReplaceCard:
		{
			jsonIn.Add("Name", (char*)pIDCardInfo->szName);
			jsonIn.Add("CardID", (char*)pIDCardInfo->szIdentity);
			jsonIn.Add("City", Reginfo.strCityCode);
			jsonIn.Add("Mobile", pSSCardInfo->strMobile);
			//jsonIn.Add("Operator", Reginfo.strOperator);
			jsonIn.Add("IssueDate", (char*)pIDCardInfo->szExpirationDate1);
			QString strExpireDate = QString::fromLocal8Bit((const char*)pIDCardInfo->szExpirationDate2);
			if (strExpireDate == "长期")
				jsonIn.Add("ExpireDate", "99991231");
			else
				jsonIn.Add("ExpireDate", (char*)pIDCardInfo->szExpirationDate2);

			jsonIn.Add("Birthday", (char*)pIDCardInfo->szBirthday);
			jsonIn.Add("Gender", (char*)pIDCardInfo->szGender);
			jsonIn.Add("Nation", (char*)pIDCardInfo->szNationaltyCode);
			jsonIn.Add("Address", (char*)pIDCardInfo->szAddress);
			jsonIn.Add("BankCode", Reginfo.strBankCode);
			jsonIn.Add("CardNum", "M82492669");
		}
		break;
		case ServiceType::Service_RegisterLost:
		default:
		{
			QMessageBox_CN(QMessageBox::Information, "提示", "不支持的服务！", QMessageBox::Ok, this);
		}
		break;

		}

		strJsonIn = jsonIn.ToString();

		if (QFailed(pService->CommitPersonInfo(strJsonIn, strJsonOut)))
			break;
	} while (0);
}

void SSCardServiceT::on_pushButton_CreateService_clicked()
{
	SSCardService* pService = nullptr;
	QString strMessage;

	g_pDataCenter->nCardServiceType = (ServiceType)pBtnGroup->checkedId();

	if (QFailed(g_pDataCenter->OpenSSCardService(&pService, strMessage)))
	{
		QMessageBox::information(nullptr, "Information", "Failed in OpenSSCardService", QMessageBox::Ok);
		return;
	}

	if (!pService)
	{
		//strMessage = "社保卡卡管服务不可用!";
		QMessageBox::information(nullptr, "Information", "SSCardService is invalid!", QMessageBox::Ok);
		return;
	}
}

void SSCardServiceT::on_pushButton_QueryCardInfo_clicked()
{
	int nResult = -1;
	QString strMessage;

	IDCardInfoPtr pIDCardInfo = g_pDataCenter->GetIDCardInfo();
	SSCardBaseInfoPtr& pSSCardInfo = g_pDataCenter->GetSSCardInfo();
	RegionInfo& Reginfo = g_pDataCenter->GetSysConfigure()->Region;

	pSSCardInfo->strOrganID = Reginfo.strAgency;
	pSSCardInfo->strOrganID = Reginfo.strAgency;
	pSSCardInfo->strBankCode = Reginfo.strBankCode;
	pSSCardInfo->strTransType = "5";
	pSSCardInfo->strCity = Reginfo.strCityCode;
	pSSCardInfo->strSSQX = Reginfo.strCountry;
	pSSCardInfo->strCardVender = Reginfo.strCardVendor;
	pSSCardInfo->strBankCode = Reginfo.strBankCode;


	QString strInfo;
	SSCardService* pService = g_pDataCenter->GetSSCardService();
	string strJsonIn, strJsonOut;

	do
	{
		CJsonObject jsonIn;
		nResult = -1;
		strJsonIn = jsonIn.ToString();
		auto BankList = { "96588","95566","95533","95559","95580","95599","95588","96288","95555","95558","96688","96699" };
		for (auto var : BankList)
		{
			jsonIn.Clear();
			jsonIn.Add("Name", (char*)pIDCardInfo->szName);
			jsonIn.Add("CardID", (char*)pIDCardInfo->szIdentity);
			jsonIn.Add("City", Reginfo.strCityCode);
			jsonIn.Add("BankCode", var);
			strJsonIn = jsonIn.ToString();
			if (QSucceed(pService->QueryCardInfo(strJsonIn, strJsonOut)))
				//if (QSucceed(pService->SetExtraInterface("QueryPersonInfo", strJsonIn, strJsonOut)))
			{
				nResult = 0;
				break;
			}
		}
		if (QFailed(nResult))
		{
			string strText;
			CJsonObject jsonOut(strJsonOut);
			jsonOut.Get("Result", nResult);
			jsonOut.Get("Message", strText);
			QMessageBox_CN(QMessageBox::Information, "提示", QString::fromLocal8Bit(strText.c_str()), QMessageBox::Ok, this);
			break;
		}

		CJsonObject jsonOut(strJsonOut);
		if (!jsonOut.KeyExist("CardNum"))
		{
			QMessageBox::information(nullptr, "information", "Failed in get carddata!");
			break;
		}
		jsonOut.Get("CardNum", pSSCardInfo->strCardNum);

	} while (0);
}

void SSCardServiceT::on_pushButton_PremakeCard_clicked()
{

	QString strMessage;

	SSCardBaseInfoPtr& pSSCardInfo = g_pDataCenter->GetSSCardInfo();
	IDCardInfoPtr& pIDCardInfo = g_pDataCenter->GetIDCardInfo();
	RegionInfo& Reginfo = g_pDataCenter->GetSysConfigure()->Region;
	pSSCardInfo->strOrganID = Reginfo.strAgency;
	pSSCardInfo->strOrganID = Reginfo.strAgency;
	pSSCardInfo->strBankCode = Reginfo.strBankCode;
	pSSCardInfo->strTransType = "5";
	pSSCardInfo->strCity = Reginfo.strCityCode;
	pSSCardInfo->strSSQX = Reginfo.strCountry;
	pSSCardInfo->strCardVender = Reginfo.strCardVendor;
	pSSCardInfo->strBankCode = Reginfo.strBankCode;


	QString strInfo;
	SSCardService* pService = g_pDataCenter->GetSSCardService();
	string strJsonIn, strJsonOut;

	do
	{
		CJsonObject jsonIn;
		jsonIn.Add("Name", (char*)pIDCardInfo->szName);
		jsonIn.Add("CardID", (char*)pIDCardInfo->szIdentity);
		jsonIn.Add("City", Reginfo.strCityCode);
		jsonIn.Add("BankCode", Reginfo.strBankCode);

		strJsonIn = jsonIn.ToString();

		if (QFailed(pService->PreMakeCard(strJsonIn, strJsonOut)))
			break;
		CJsonObject jsonOut(strJsonOut);
		if (!jsonOut.KeyExist("CardNum") ||
			!jsonOut.KeyExist("Photo"))
		{
			QMessageBox::information(nullptr, "information", "Failed in get carddata!");
			break;
		}
		string strPhoto;
		string strSSCardNum;
		jsonOut.Get("CardNum", strSSCardNum);
		jsonOut.Get("Photo", strPhoto);
		QString strMessage;
		SaveSSCardPhoto(strMessage, strPhoto.c_str());

	} while (0);
}


void SSCardServiceT::on_pushButton_WriteCard_clicked()
{
	int nResult = -1;
	QString strMessage;


	SSCardBaseInfoPtr& pSSCardInfo = g_pDataCenter->GetSSCardInfo();
	RegionInfo& Reginfo = g_pDataCenter->GetSysConfigure()->Region;
	pSSCardInfo->strOrganID = Reginfo.strAgency;
	pSSCardInfo->strOrganID = Reginfo.strAgency;
	pSSCardInfo->strBankCode = Reginfo.strBankCode;
	pSSCardInfo->strTransType = "5";
	pSSCardInfo->strCity = Reginfo.strCityCode;
	pSSCardInfo->strSSQX = Reginfo.strCountry;
	pSSCardInfo->strCardVender = Reginfo.strCardVendor;
	pSSCardInfo->strBankCode = Reginfo.strBankCode;

	QString strInfo;
	string strJsonIn, strJsonOut;

	int nWriteCardCount = 0;
	nResult = -1;
	while (nWriteCardCount < 3)
	{
		strInfo = QString("尝试第%1次写卡!").arg(nWriteCardCount + 1);
		gInfo() << gQStr(strInfo);
		nResult = g_pDataCenter->WriteCard(pSSCardInfo, strMessage);
		if (nResult == 0)
			break;
		if (nResult == -4)
		{
			nWriteCardCount++;
			strMessage = "写卡上电失败!";
			gInfo() << gQStr(strMessage);
			g_pDataCenter->MoveCard(strMessage);
			continue;
		}
		else if (QFailed(nResult))
		{
			strMessage = "写卡失败!";
			break;
		}
	}

}


void SSCardServiceT::on_pushButton_ReturnCard_clicked()
{
	char szRCode[32] = { 0 };
	int nResult = -1;
	QString strMessage;

	SSCardBaseInfoPtr& pSSCardInfo = g_pDataCenter->GetSSCardInfo();
	RegionInfo& Reginfo = g_pDataCenter->GetSysConfigure()->Region;
	pSSCardInfo->strOrganID = Reginfo.strAgency;
	pSSCardInfo->strOrganID = Reginfo.strAgency;
	pSSCardInfo->strBankCode = Reginfo.strBankCode;
	pSSCardInfo->strTransType = "5";
	pSSCardInfo->strCity = Reginfo.strCityCode;
	pSSCardInfo->strSSQX = Reginfo.strCountry;
	pSSCardInfo->strCardVender = Reginfo.strCardVendor;
	pSSCardInfo->strBankCode = Reginfo.strBankCode;

	DeviceConfig& DevConfig = g_pDataCenter->GetSysConfigure()->DevConfig;
	QString strInfo;
	SSCardService* pService = g_pDataCenter->GetSSCardService();
	string strJsonIn, strJsonOut;
	char szCardATR[1024] = { 0 };
	char szCardBaseRead[1024] = { 0 };
	int nCardATRLen = 0;

	do
	{
		if (QFailed(nResult = g_pDataCenter->GetSSCardReader()->Reader_PowerOn(DevConfig.nSSCardReaderPowerOnType, szCardATR, nCardATRLen, szRCode)))
		{
			strMessage = QString("IC卡上电失败,PowerOnType:%1,nResult:%2").arg((int)DevConfig.nSSCardReaderPowerOnType).arg(nResult);
			nResult = -4;
			break;
		}
		gInfo() << "CardATR:" << szCardATR;
		pSSCardInfo->strCardATR = szCardATR;
		char szBankNum[64] = { 0 };
		if (QFailed(nResult = iReadBankNumber(DevConfig.nSSCardReaderPowerOnType, szBankNum)))
		{
			strMessage = QString("读银行卡信息失败,PowerOnType:%1,nResult:%2").arg((int)DevConfig.nSSCardReaderPowerOnType).arg(nResult);
			break;
		}
		gInfo() << "BankNum:" << szBankNum;
		pSSCardInfo->strBankNum = szBankNum;
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
		Field[ 0 ] "371600"
		Field[ 1 ] ""
		Field[ 2 ] ""
		Field[ 3 ] "371600D156000005004B9CF763FBBE5C"
		Field[ 4 ] ""
		Field[ 5 ] "0081544C9686843716008F3D97"
		Field[ 6 ] "3.00"
		Field[ 7 ] "20220224"
		Field[ 8 ] "20320224"
		Field[ 9 ] ""
		*/
		QStringList strFieldList = QString(szCardBaseRead).split("|", Qt::KeepEmptyParts);
		int nIndex = 0;
		for (auto var : strFieldList)
		{
			qDebug() << "Field[" << nIndex << "]" << var;
			nIndex++;
		}
		gInfo() << "CardBaseRead:" << szCardBaseRead;

		pSSCardInfo->strCardIdentity = strFieldList[3].toStdString();

		CJsonObject jsonIn;
		jsonIn.Add("CardID", pSSCardInfo->strIdentity);
		jsonIn.Add("Name", pSSCardInfo->strName);
		jsonIn.Add("BankCode", pSSCardInfo->strBankCode);
		jsonIn.Add("City", pSSCardInfo->strCity);
		jsonIn.Add("CardNum", pSSCardInfo->strCardNum);
		jsonIn.Add("CardATR", pSSCardInfo->strCardATR);
		jsonIn.Add("CardIdentity", pSSCardInfo->strCardIdentity);

		jsonIn.Add("ChipNum", pSSCardInfo->strBankNum);
		jsonIn.Add("MagNum", pSSCardInfo->strBankNum);
		jsonIn.Add("CardVersion", strFieldList[6].toStdString());
		jsonIn.Add("ChipType", "32");

		strJsonIn = jsonIn.ToString();

		if (QFailed(pService->ReturnData(strJsonIn, strJsonOut)))
			break;
		QMessageBox::information(nullptr, "提示", "回盘成功!");
	} while (0);
}


void SSCardServiceT::on_pushButton_ActiveCard_clicked()
{
	char szRCode[32] = { 0 };
	int nResult = -1;
	QString strMessage;

	SSCardBaseInfoPtr& pSSCardInfo = g_pDataCenter->GetSSCardInfo();
	RegionInfo& Reginfo = g_pDataCenter->GetSysConfigure()->Region;
	pSSCardInfo->strOrganID = Reginfo.strAgency;
	pSSCardInfo->strOrganID = Reginfo.strAgency;
	pSSCardInfo->strBankCode = Reginfo.strBankCode;
	pSSCardInfo->strTransType = "5";
	pSSCardInfo->strCity = Reginfo.strCityCode;
	pSSCardInfo->strSSQX = Reginfo.strCountry;
	pSSCardInfo->strCardVender = Reginfo.strCardVendor;
	pSSCardInfo->strBankCode = Reginfo.strBankCode;

	DeviceConfig& DevConfig = g_pDataCenter->GetSysConfigure()->DevConfig;
	QString strInfo;
	SSCardService* pService = g_pDataCenter->GetSSCardService();
	string strJsonIn, strJsonOut;
	char szCardATR[1024] = { 0 };
	int nCardATRLen = 0;

	do
	{

		if (QFailed(nResult = g_pDataCenter->GetSSCardReader()->Reader_PowerOn(DevConfig.nSSCardReaderPowerOnType, szCardATR, nCardATRLen, szRCode)))
		{
			strMessage = QString("IC卡上电失败,PowerOnType:%1,nResult:%2").arg((int)DevConfig.nSSCardReaderPowerOnType).arg(nResult);
			nResult = -4;
			break;
		}
		gInfo() << "CardATR:" << szCardATR;
		pSSCardInfo->strCardATR = szCardATR;
		char szBankNum[64] = { 0 };
		if (QFailed(nResult = iReadBankNumber(DevConfig.nSSCardReaderPowerOnType, szBankNum)))
		{
			strMessage = QString("读银行卡信息失败,PowerOnType:%1,nResult:%2").arg((int)DevConfig.nSSCardReaderPowerOnType).arg(nResult);
			break;
		}
		CJsonObject jsonIn;

		jsonIn.Add("CardID", pSSCardInfo->strIdentity);
		jsonIn.Add("Name", pSSCardInfo->strName);
		jsonIn.Add("BankCode", Reginfo.strBankCode);
		jsonIn.Add("City", Reginfo.strCityCode);
		jsonIn.Add("CardNum", pSSCardInfo->strCardNum);
		jsonIn.Add("MagNum", szBankNum);
		strJsonIn = jsonIn.ToString();

		if (QFailed(pService->ActiveCard(strJsonIn, strJsonOut)))
			break;
		QMessageBox::information(nullptr, "提示", "激活成功!");
	} while (0);
}

void SSCardServiceT::on_pushButton_LoadPreMakeCard_clicked()
{
	if (!g_pDataCenter->GetIDCardInfo())
	{
		QMessageBox::information(nullptr, "提示", "请先加载身份证数据!");
		return;
	}
	IDCardInfoPtr pIDCard = g_pDataCenter->GetIDCardInfo();
	SSCardBaseInfoPtr& pSSCardInfo = g_pDataCenter->GetSSCardInfo();
	RegionInfo& Reginfo = g_pDataCenter->GetSysConfigure()->Region;
	try
	{
		string strOutputFile = "./Debug/Carddata_";
		strOutputFile += (char*)pIDCard->szIdentity;
		strOutputFile += ".json";
		fstream fs(strOutputFile, ios::in);
		stringstream ss;
		ss << fs.rdbuf();
		CJsonObject JsonIn(ss.str());
		CJsonObject ds = JsonIn["ds"];

		/*{
			"csrq": "19881023",
			"fkjgbh": "371699",
			"fkjgmc": "市直社保卡",
			"kid": "184220222122810132",
			"lxdh": "18605432732",
			"mz": "01",
			"sbjgbh": "37860107",
			"sbjgmc": "市直社保卡",
			"sbkh": "M82995194",
			"shbzhm": "372325198810232432",
			"sjhm": "12548096354",
			"txdz": "山东省沾化县冯家镇傅家村736号",
			"xb": "1",
			"xm": "付建力",
			"yxzjhm": "372325198810232432",
			"yxzjlx": "A",
			"zp": "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",
			"zzbh": "3716010958",
			"zzmc": "山东黄三角传媒有限公司"
		}*/

		string strName, strCardNum, strGender, strNatioinality, strBirthday, strIdentity;
		ds.Get("xm", strName);
		ds.Get("shbzhm", strIdentity);
		ds.Get("sbkh", strCardNum);
		ds.Get("xb", strGender);
		ds.Get("mz", strNatioinality);
		ds.Get("csrq", strBirthday);

		if (pSSCardInfo)
		{
			pSSCardInfo->strOrganID = Reginfo.strAgency;
			pSSCardInfo->strBankCode = Reginfo.strBankCode;
			pSSCardInfo->strTransType = "5";
			pSSCardInfo->strCity = Reginfo.strCityCode;
			pSSCardInfo->strSSQX = Reginfo.strCountry;
			pSSCardInfo->strCardVender = Reginfo.strCardVendor;
			pSSCardInfo->strBankCode = Reginfo.strBankCode;
		}

		fs.close();
	}
	catch (std::exception& e)
	{
		string strException = e.what();
	}
}

void SSCardServiceT::on_pushButton_GetPhoto_clicked()
{
	QString strMessage;

	IDCardInfoPtr pIDCard = g_pDataCenter->GetIDCardInfo();
	RegionInfo& Reginfo = g_pDataCenter->GetSysConfigure()->Region;

	QString strInfo;
	SSCardService* pService = g_pDataCenter->GetSSCardService();
	string strJsonIn, strJsonOut;

	do
	{
		CJsonObject jsonIn;

		jsonIn.Add("CardID", (char*)pIDCard->szIdentity);
		jsonIn.Add("Name", (char*)pIDCard->szName);
		jsonIn.Add("City", (char*)Reginfo.strCityCode.c_str());

		strJsonIn = jsonIn.ToString();
		string strJsonout;
		string strCommand = "QueryPersonPhoto";

		if (QFailed(pService->SetExtraInterface(strCommand, strJsonIn, strJsonOut)))
		{
			break;
		}
		CJsonObject jsonOut(strJsonOut);
		string strPhoto;
		if (jsonOut.Get("Photo", strPhoto))
		{
			SaveSSCardPhoto(strMessage, strPhoto.c_str());
		}

		QMessageBox::information(nullptr, "提示", "照片取得成功!");
	} while (0);
}

void SSCardServiceT::on_pushButton_UnRegisterLost_clicked()
{
	QString strMessage;

	IDCardInfoPtr pIDCard = g_pDataCenter->GetIDCardInfo();
	RegionInfo& Reginfo = g_pDataCenter->GetSysConfigure()->Region;

	QString strInfo;
	SSCardService* pService = g_pDataCenter->GetSSCardService();
	string strJsonIn, strJsonOut;

	do
	{
		CJsonObject jsonIn;

		jsonIn.Add("CardID", (const char*)pIDCard->szIdentity);
		jsonIn.Add("Name", (const char*)pIDCard->szName);
		jsonIn.Add("City", Reginfo.strCityCode);
		jsonIn.Add("BankCode", Reginfo.strBankCode);
		jsonIn.Add("CardNum", "M83029393");

		strJsonIn = jsonIn.ToString();
		string strJsonout;
		string strCommand = "QueryPersonPhoto";

		if (QFailed(pService->RegisterLost(strJsonIn, strJsonOut)))
		{
			break;
		}
		CJsonObject jsonOut(strJsonOut);
		string strPhoto;
		if (jsonOut.Get("Photo", strPhoto))
		{
			SaveSSCardPhoto(strMessage, strPhoto.c_str());
		}

		QMessageBox::information(nullptr, "提示", "照片取得成功!");
	} while (0);
}

void SSCardServiceT::on_pushButton_ReadBankNum_clicked()
{
	int nResult = -1;
	QString strMessage;
	DeviceConfig& DevConfig = g_pDataCenter->GetSysConfigure()->DevConfig;


	SSCardBaseInfoPtr& pSSCardInfo = g_pDataCenter->GetSSCardInfo();
	char szRCode[32] = { 0 };
	char szCardBaseRead[1024] = { 0 };
	int nCardATRLen = 0;
	QString strInfo;


	do
	{
		if (QFailed(g_pDataCenter->OpenDevice(strMessage)))
		{
			break;
		}
		KT_Reader* pSSCardReader = g_pDataCenter->GetSSCardReader();
		if (!pSSCardReader)
		{
			strMessage = "读卡器未就绪!";
			break;
		}
		if (QFailed(g_pDataCenter->MoveCard(strMessage)))
			break;

		if (QFailed(nResult = pSSCardReader->Reader_PowerOn(DevConfig.nSSCardReaderPowerOnType, (char*)pSSCardInfo->strCardATR.c_str(), nCardATRLen, szRCode)))
		{
			strMessage = QString("IC卡上电失败,PowerOnType:%1,nResult:%2").arg((int)DevConfig.nSSCardReaderPowerOnType).arg(nResult);
			nResult = -4;
			break;
		}
		gInfo() << "CardATR:" << pSSCardInfo->strCardATR;
		char szBankNum[32] = { 0 };
		if (QFailed(nResult = iReadBankNumber(DevConfig.nSSCardReaderPowerOnType, szBankNum)))
		{
			strMessage = QString("读银行卡信息失败,PowerOnType:%1,nResult:%2").arg((int)DevConfig.nSSCardReaderPowerOnType).arg(nResult);
			break;
		}

		pSSCardInfo->strBankNum = szBankNum;
		gInfo() << "BankNum:" << pSSCardInfo->strBankNum;

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
		Field[ 0 ] "371600"
		Field[ 1 ] ""
		Field[ 2 ] ""
		Field[ 3 ] "371600D156000005004B9CF763FBBE5C"
		Field[ 4 ] ""
		Field[ 5 ] "0081544C9686843716008F3D97"
		Field[ 6 ] "3.00"
		Field[ 7 ] "20220224"
		Field[ 8 ] "20320224"
		Field[ 9 ] ""
		*/
		QStringList strFieldList = QString(szCardBaseRead).split("|", Qt::KeepEmptyParts);
		int nIndex = 0;
		for (auto var : strFieldList)
		{
			qDebug() << "Field[" << nIndex << "]" << var;
			nIndex++;
		}
		gInfo() << "CardBaseRead:" << szCardBaseRead;

		pSSCardInfo->strCardIdentity = strFieldList[3].toStdString();
		nResult = 0;
	} while (0);
	if (QFailed(nResult))
	{
		QMessageBox::information(nullptr, "提示", strMessage);
	}
	else
	{
		QMessageBox::information(nullptr, "提示", pSSCardInfo->strBankNum.c_str());
	}
}
