#include "sys_manualmakecard.h"
#include "ui_sys_manualmakecard.h"
#include "Sys_dialogidcardinfo.h"
#include <QFileInfo>
#include <QButtonGroup>

Sys_ManualMakeCard::Sys_ManualMakeCard(QWidget* parent) :
	QWidget(parent),
	ui(new Ui::Sys_ManualMakeCard)
{
	ui->setupUi(this);
	connect(this, &Sys_ManualMakeCard::ShowIDCardInfo, this, &Sys_ManualMakeCard::on_ShowIDCardInfo, Qt::QueuedConnection);
	QString strAppPath = QCoreApplication::applicationDirPath();
	strAppPath += "/Image/FaceCaptureSample.jpg";
	QFileInfo fi(strAppPath);
	if (fi.isFile())
	{
		QString strStyle = QString("border-image: url(./Image/FaceCaptureSample.jpg);");
		ui->label_Photo->setStyleSheet(strStyle);
	}
	if (!g_pDataCenter->bDebug)
		ui->checkBox_Debug->hide();

	pButtonGrpServiceType = new QButtonGroup(this);
	pButtonGrpServiceType->addButton(ui->radioButton_NewCard, 0);
	pButtonGrpServiceType->addButton(ui->radioButton_ReplaceCard, 1);
	pButtonGrpServiceType->setExclusive(true);

	pButtonGrpBusiness = new QButtonGroup(this);
	pButtonGrpBusiness->addButton(ui->radioButton_commitInfo, 0);
	pButtonGrpBusiness->addButton(ui->radioButton_PremakeCard, 1);
	pButtonGrpBusiness->addButton(ui->radioButton_WriteCard, 2);
	pButtonGrpBusiness->addButton(ui->radioButton_PrintCard, 3);
	pButtonGrpBusiness->addButton(ui->radioButton_EnableCard, 4);
	pButtonGrpBusiness->setExclusive(true);

	pButtonGrpGender = new QButtonGroup(this);
	pButtonGrpGender->addButton(ui->radioButton_Male, 0);
	pButtonGrpGender->addButton(ui->radioButton_Female, 1);
	pButtonGrpGender->setExclusive(true);
	ui->radioButton_Male->setChecked(true);

	for (auto var : g_vecNationCode)
	{
		ui->comboBox_Nationality->addItem(var.strNationalty.c_str(), var.strCode.c_str());
	}

	ui->comboBox_Nationality->setCurrentIndex(0);
}

Sys_ManualMakeCard::~Sys_ManualMakeCard()
{
	delete ui;
}

void Sys_ManualMakeCard::on_ShowIDCardInfo(bool bSucceed, QString strMessage)
{
	if (bSucceed)
	{
		DialogIDCardInfo W(CardInfo, this);
		W.exec();
		IDCardInfoPtr pIDCard = make_shared<IDCardInfo>(CardInfo);
		g_pDataCenter->SetIDCardInfo(pIDCard);
		ui->lineEdit_CardID->setText((char*)pIDCard->szIdentity);
		ui->lineEdit_Name->setText(QString::fromLocal8Bit((const char*)pIDCard->szName));
	}
	else
		QMessageBox_CN(QMessageBox::Critical, tr("提示"), strMessage, QMessageBox::Ok, this);
	EnableUI(this, true);
}

void Sys_ManualMakeCard::fnThreadReadIDCard(string strPort)
{
	int nRetry = 30;
	bool bSucceed = false;
	while (bThreadReadIDCardRunning)
	{
		if (ReaderIDCard(strPort.c_str(), CardInfo) == IDCard_Status::IDCard_Succeed)
		{
			bSucceed = true;
			break;
		}
		nRetry--;
		if (nRetry <= 0)
		{
			break;
		}
		Sleep(100);
	}
	if (bSucceed)
	{
		emit ShowIDCardInfo(bSucceed, "");
	}
	else
		emit ShowIDCardInfo(bSucceed, "读取身份证信息失败,请稍后重试!");

}
#define    Error_Not_IDCARD         (-1)

int Sys_ManualMakeCard::ReaderIDCard(const char* szPort, IDCardInfo& CardInfo)
{
	int nResult = IDCard_Status::IDCard_Succeed;
	do
	{
		nResult = ::OpenReader(szPort);
		//nResult = OpenReader(nullptr);;
		if (nResult != IDCard_Status::IDCard_Succeed)
		{
			break;
		}
		nResult = FindIDCard();
		if (nResult != IDCard_Status::IDCard_Succeed)
		{
			break;
		}

		nResult = ::ReadIDCard(CardInfo);
		if (nResult != IDCard_Status::IDCard_Succeed)
		{
			break;
		}
		CloseReader();
	} while (0);
	return nResult;
}

void Sys_ManualMakeCard::EnableUI(QObject* pUIObj, bool bEnable)
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
		EnableUI(pObj, bEnable);
	}
	this->setEnabled(true);
}

void Sys_ManualMakeCard::on_pushButton_ReadID_clicked()
{
	//if (!ui->checkBox_Debug->isChecked())
	{
		DeviceConfig& devCfg = g_pDataCenter->GetSysConfigure()->DevConfig;
		if (devCfg.strIDCardReaderPort.empty())
		{
			QMessageBox_CN(QMessageBox::Question, tr("提示"), "身份证读卡器端口为空,请配置身份证读卡器!", QMessageBox::Ok, this);
			return;
		}
		QMessageBox_CN(QMessageBox::Information, tr("提示"), "请将身份证放置于读卡区,并每隔2秒取走一次,来回放置于读卡区!", QMessageBox::Ok, this);

		EnableUI(this, false);

		bThreadReadIDCardRunning = true;
		pThreadReadIDCard = new std::thread(&Sys_ManualMakeCard::fnThreadReadIDCard, this, devCfg.strIDCardReaderPort);
	}
	//else
	//{
	//	string strCardID, strName, strMobile;
	//	g_pDataCenter->bDebug = true;
	//	IDCardInfoPtr pIDCard = make_shared<IDCardInfo>();
	//	SSCardBaseInfoPtr pSSCardInfo = make_shared<SSCardBaseInfo>();
	//	if (QSucceed(LoadTestIDData(pIDCard, pSSCardInfo)))
	//	{
	//		g_pDataCenter->SetIDCardInfo(pIDCard);
	//		ui->lineEdit_CardID->setText((char*)pIDCard->szIdentity);
	//		ui->lineEdit_Name->setText(QString::fromLocal8Bit((const char*)pIDCard->szName));
	//	}
	//	else
	//	{
	//		QMessageBox_CN(QMessageBox::Information, tr("提示"), "加载测试数据失败,请确认数据是否存在!", QMessageBox::Ok, this);
	//	}
	//	g_pDataCenter->bDebug = false;
	//}
}

int Sys_ManualMakeCard::LoadPersonSSCardData(QString& strMesssage)
{
	IDCardInfoPtr pIDCard = g_pDataCenter->GetIDCardInfo();
	SSCardBaseInfoPtr pSSCardInfo = make_shared<SSCardBaseInfo>();
	RegionInfo& Reginfo = g_pDataCenter->GetSysConfigure()->Region;

	QString strAppPath = QCoreApplication::applicationDirPath();
	strAppPath += "/Debug";
	strAppPath += QString("/Carddata_%1.json").arg((const char*)pIDCard->szIdentity);
	QString strMessage;

	QFileInfo ffile(strAppPath);
	if (!ffile.isFile())
	{
		strMesssage = "制卡数据不存在，可能制卡流程尚未开始!";
		return -1;
	}
	try
	{
		ifstream ifs(strAppPath.toStdString(), ios::in);
		stringstream ss;
		ss << ifs.rdbuf();
		CJsonObject json(ss.str());
		if (!json.KeyExist("ds"))
		{
			strMesssage = "ds域不存在,制卡数据无效!";
			return -1;
		}
		CJsonObject ds = json["ds"];
		if (ds.IsEmpty())
		{
			strMessage = "ds数据域为空,制卡数据无效!";
			return -1;
		}

		if (!ds.KeyExist("xm") ||
			!ds.KeyExist("shbzhm"))
		{
			strMessage = "ds域中没有'姓名'或'社会保障卡号'字段,制卡数据无效";
			return -1;
		}
		ds.Get("sbkh", pSSCardInfo->strCardNum);
		ds.Get("zp", pSSCardInfo->strPhoto);
		ds.Get("mz", pSSCardInfo->strNation);

		pSSCardInfo->strName = (const char*)pIDCard->szName;
		pSSCardInfo->strGender = (const char*)pIDCard->szGender;
		pSSCardInfo->strNation = (const char*)pIDCard->szNationalty;
		pSSCardInfo->strNationCode = (const char*)pIDCard->szNationaltyCode;
		pSSCardInfo->strBirthday = (const char*)pIDCard->szBirthday;
		pSSCardInfo->strIdentity = (const char*)pIDCard->szIdentity;
		pSSCardInfo->strAddress = (const char*)pIDCard->szAddress;

		pSSCardInfo->strOrganID = Reginfo.strAgency;
		pSSCardInfo->strBankCode = Reginfo.strBankCode;
		pSSCardInfo->strCity = Reginfo.strCityCode;
		pSSCardInfo->strSSQX = Reginfo.strCountry;
		pSSCardInfo->strCardVender = Reginfo.strCardVendor;
		pSSCardInfo->strBankCode = Reginfo.strBankCode;

		ui->lineEdit_Name->setText(QString::fromLocal8Bit(pSSCardInfo->strName.c_str()));
		ui->lineEdit_CardID->setText(pSSCardInfo->strIdentity.c_str());
		ui->lineEditl_SSCard->setText(pSSCardInfo->strCardNum.c_str());
		QDateTime dt = QDateTime::currentDateTime();
		QString strIssueDate = dt.toString("yyyy年MM月");
		//ui->lineEdit_Datetime->setText(strIssueDate);
		SaveSSCardPhoto(strMessage, pSSCardInfo->strPhoto.c_str());

		QString strStyle = QString("border-image: url(%1);").arg(g_pDataCenter->strSSCardPhotoFile.c_str());
		ui->label_Photo->setStyleSheet(strStyle);

		g_pDataCenter->SetSSCardInfo(pSSCardInfo);
		return 0;

	}
	catch (std::exception& e)
	{
		gInfo() << e.what();
		return 0;
	}
}

void Sys_ManualMakeCard::on_pushButton_LoadCardData_clicked()
{
	QString strMessage;

	if (QFailed(LoadPersonSSCardData(strMessage)))
	{
		QMessageBox_CN(QMessageBox::Information, tr("提示"), strMessage, QMessageBox::Ok, this);
		return;
	}
	SSCardBaseInfoPtr pSSCardInfo = g_pDataCenter->GetSSCardInfo();
	QString strStyle = QString("border-image: url(%1);").arg(g_pDataCenter->strSSCardPhotoFile.c_str());
	ui->label_Photo->setStyleSheet(strStyle);
	ui->lineEdit_CardID->setText(pSSCardInfo->strIdentity.c_str());
	ui->lineEdit_Name->setText(QString::fromLocal8Bit(pSSCardInfo->strName.c_str()));
	ui->lineEditl_SSCard->setText(pSSCardInfo->strCardNum.c_str());
	//ui->lineEdit_Datetime->setText(pSSCardInfo->strReleaseDate);
	string strBankName;
	g_pDataCenter->GetBankName(pSSCardInfo->strBankCode, strBankName);

	ui->lineEdit_Bank->setText(strBankName.c_str());
}

void Sys_ManualMakeCard::PrintPhoto()
{
	int nResult = -1;
	QString strMessage;
	QString strInfo;
	char szRCode[128] = { 0 };
	do
	{
		if (QFailed(LoadPersonSSCardData(strMessage)))
		{
			break;
		}
		SSCardBaseInfoPtr pSSCardInfo = g_pDataCenter->GetSSCardInfo();

		if (!g_pDataCenter->GetPrinter())
		{
			if (QFailed(g_pDataCenter->OpenPrinter(strMessage)))
				break;
		}
		if (!g_pDataCenter->GetSSCardReader())
		{
			if (QFailed(g_pDataCenter->OpenSSCardReader(strMessage)))
				break;
		}

		if (QFailed(g_pDataCenter->Depense(strMessage)))
			break;
		strInfo = "进卡成功";
		gInfo() << gQStr(strInfo);

		if (QFailed(g_pDataCenter->PrintCard(pSSCardInfo, "", strMessage)))
		{
			strMessage = "片卡打印失败,请稍后重试!";
			strInfo = strMessage;
			gInfo() << gQStr(strInfo);
			break;
		}

		g_pDataCenter->GetPrinter()->Printer_Eject(szRCode);
		strInfo = "卡片打印成功";
		gInfo() << gQStr(strInfo);
		nResult = 0;
	} while (0);

	char* szResCode[128] = { 0 };
	g_pDataCenter->GetPrinter()->Printer_Eject((char*)szResCode);

	if (QFailed(nResult))
		QMessageBox_CN(QMessageBox::Information, tr("提示"), strMessage, QMessageBox::Ok, this);
	else
		QMessageBox_CN(QMessageBox::Information, tr("提示"), "打印成功,请及时取走您的卡片", QMessageBox::Ok, this);
}

void Sys_ManualMakeCard::EnableCard()
{
	int nResult = -1;
	QString strMessage;

	int nStatus = 0;
	QString strInfo;
	char szRCode[128] = { 0 };
	do
	{
		if (QFailed(LoadPersonSSCardData(strMessage)))
		{
			break;
		}
		SSCardBaseInfoPtr pSSCardInfo = g_pDataCenter->GetSSCardInfo();

		if (!g_pDataCenter->GetPrinter())
		{
			if (QFailed(g_pDataCenter->OpenPrinter(strMessage)))
				break;
		}
		if (!g_pDataCenter->GetSSCardReader())
		{
			if (QFailed(g_pDataCenter->OpenSSCardReader(strMessage)))
				break;
		}

		if (QFailed(g_pDataCenter->Depense(strMessage)))
			break;
		strInfo = "进卡成功";
		gInfo() << gQStr(strInfo);

		int nWriteCardCount = 0;
		nResult = -1;
		while (nWriteCardCount < 3)
		{
			strInfo = QString("尝试第%1次读卡!").arg(nWriteCardCount + 1);
			gInfo() << gQStr(strInfo);
			nResult = g_pDataCenter->ReadCard(pSSCardInfo, strMessage);
			if (nResult == 0)
				break;
			if (nResult == -4)
			{
				nWriteCardCount++;
				strMessage = "读卡上电失败!";
				gInfo() << gQStr(strMessage);
				g_pDataCenter->MoveCard(strMessage);
				continue;
			}
			else if (QFailed(nResult))
			{
				strMessage = "读卡失败!";
				break;
			}
		}

		if (QFailed(nResult))
		{
			strInfo = "读卡失败";
			gInfo() << gQStr(strInfo);
			break;
		}


		if (nStatus != 0 && nStatus != 1)
		{
			strMessage = "卡片启用失败,请稍后重试!";
			break;
		}

		g_pDataCenter->GetPrinter()->Printer_Eject(szRCode);
		strInfo = "卡片打印成功";
		gInfo() << gQStr(strInfo);
		nResult = 0;
	} while (0);

	char* szResCode[128] = { 0 };
	g_pDataCenter->GetPrinter()->Printer_Eject((char*)szResCode);

	if (QFailed(nResult))
		QMessageBox_CN(QMessageBox::Information, tr("提示"), strMessage, QMessageBox::Ok, this);
	else
		QMessageBox_CN(QMessageBox::Information, tr("提示"), "启用成功,请及时取走您的卡片", QMessageBox::Ok, this);
}

void Sys_ManualMakeCard::PrintCardData()
{
	int nResult = -1;
	QString strMessage;

	QString strInfo;
	char szRCode[128] = { 0 };
	do
	{
		if (QFailed(LoadPersonSSCardData(strMessage)))
		{
			break;
		}
		SSCardBaseInfoPtr pSSCardInfo = g_pDataCenter->GetSSCardInfo();

		if (!g_pDataCenter->GetPrinter())
		{
			if (QFailed(g_pDataCenter->OpenPrinter(strMessage)))
				break;
		}
		if (!g_pDataCenter->GetSSCardReader())
		{
			if (QFailed(g_pDataCenter->OpenSSCardReader(strMessage)))
				break;
		}

		if (QFailed(g_pDataCenter->Depense(strMessage)))
			break;
		strInfo = "进卡成功";
		gInfo() << gQStr(strInfo);

		int nWriteCardCount = 0;
		nResult = -1;
		while (nWriteCardCount < 3)
		{
			strInfo = QString("尝试第%1次读卡!").arg(nWriteCardCount + 1);
			gInfo() << gQStr(strInfo);
			nResult = g_pDataCenter->ReadCard(pSSCardInfo, strMessage);
			if (nResult == 0)
				break;
			if (nResult == -4)
			{
				nWriteCardCount++;
				strMessage = "读卡上电失败!";
				gInfo() << gQStr(strMessage);
				g_pDataCenter->MoveCard(strMessage);
				continue;
			}
			else if (QFailed(nResult))
			{
				strMessage = "读卡失败!";
				break;
			}
		}

		if (QFailed(nResult))
		{
			strInfo = "读卡失败";
			gInfo() << gQStr(strInfo);
			break;
		}

		if (QFailed(g_pDataCenter->PrintCard(pSSCardInfo, "", strMessage)))
		{
			strMessage = "片卡打印失败,请稍后重试!";
			strInfo = strMessage;
			gInfo() << gQStr(strInfo);
			break;
		}

		g_pDataCenter->GetPrinter()->Printer_Eject(szRCode);
		strInfo = "卡片打印成功";
		gInfo() << gQStr(strInfo);
		nResult = 0;
	} while (0);

	char* szResCode[128] = { 0 };
	g_pDataCenter->GetPrinter()->Printer_Eject((char*)szResCode);

	if (QFailed(nResult))
		QMessageBox_CN(QMessageBox::Information, tr("提示"), strMessage, QMessageBox::Ok, this);
	else
		QMessageBox_CN(QMessageBox::Information, tr("提示"), "打印成功,请及时取走您的卡片", QMessageBox::Ok, this);
}

void Sys_ManualMakeCard::ProcessPowerOnFailed()
{
	int nResult = -1;
	QString strMessage;
	QString strInfo;
	do
	{
		if (QFailed(LoadPersonSSCardData(strMessage)))
		{
			break;
		}
		SSCardBaseInfoPtr pSSCardInfo = g_pDataCenter->GetSSCardInfo();

		if (!g_pDataCenter->GetPrinter())
		{
			if (QFailed(g_pDataCenter->OpenPrinter(strMessage)))
				break;
		}
		if (!g_pDataCenter->GetSSCardReader())
		{
			if (QFailed(g_pDataCenter->OpenSSCardReader(strMessage)))
				break;
		}

		if (QFailed(g_pDataCenter->Depense(strMessage)))
			break;
		strInfo = "进卡成功";
		gInfo() << gQStr(strInfo);
		if (QFailed(g_pDataCenter->WriteCard(pSSCardInfo, strMessage)))
			break;
		strInfo = "写卡成功";
		gInfo() << gQStr(strInfo);
		if (QFailed(g_pDataCenter->PrintCard(pSSCardInfo, "", strMessage)))
		{
			strMessage = "片卡打印失败,请稍后重试!";
			strInfo = strMessage;
			gInfo() << gQStr(strInfo);
			break;
		}
		strInfo = "卡片打印成功";
		gInfo() << gQStr(strInfo);
		nResult = 0;
	} while (0);

	char* szResCode[128] = { 0 };
	g_pDataCenter->GetPrinter()->Printer_Eject((char*)szResCode);

	if (QFailed(nResult))
		QMessageBox_CN(QMessageBox::Information, tr("提示"), strMessage, QMessageBox::Ok, this);
	else
		QMessageBox_CN(QMessageBox::Information, tr("提示"), "制卡成功,请及时取走您的卡片", QMessageBox::Ok, this);
}

void Sys_ManualMakeCard::on_pushButton_MakeCard_clicked()
{
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
	int nIndex = pButtonGrpBusiness->checkedId();
	switch (nIndex)
	{
	case 0:     // 提交数据
		ProcessPowerOnFailed();
		break;
	case 1:		// 预制卡开户
		break;
	case 2:		// 写入数据
		PrintCardData();
		break;
	case 3:		// 打印卡面
	{
		if (QFailed(LoadPersonSSCardData(strMessage)))
		{
			break;
		}

		break;
	}
	case 4:		// 打印卡面
	{
		this->EnableCard();
		break;
	}
	case 5:		// 启用卡片
	{
		break;
	}

	}
}

void Sys_ManualMakeCard::on_pushButton_QueryCardInfo_clicked()
{
	int nSelID = pButtonGrpServiceType->checkedId();
	if (nSelID == -1)
	{
		QMessageBox_CN(QMessageBox::Critical, tr("提示"), "请选择制卡业务类型!", QMessageBox::Ok, this);
		return;
	}
	g_pDataCenter->nCardServiceType = (ServiceType)nSelID;
	SSCardService* pService = nullptr;
	QString strMessage;
	if (QFailed(g_pDataCenter->OpenSSCardService(&pService, strMessage)))
	{
		QMessageBox_CN(QMessageBox::Critical, tr("提示"), strMessage, QMessageBox::Ok, this);
		return;
	}

	if (!pService)
	{
		QMessageBox_CN(QMessageBox::Critical, tr("提示"), "社保卡卡管库不可用!", QMessageBox::Ok, this);
		return;
	}
	int nResult = -1;
	if (ui->checkBox_WithoutIDCard->isChecked())
	{//无证办卡，检查身分证输入状态
		QString strName = ui->lineEdit_Name->text();
		if (strName.isEmpty())
		{
			QMessageBox_CN(QMessageBox::Critical, tr("提示"), "姓名不能为空!", QMessageBox::Ok, this);
			return;
		}
		QString strIDentity = ui->lineEdit_CardID->text();
		if (strIDentity.isEmpty())
		{
			QMessageBox_CN(QMessageBox::Critical, tr("提示"), "身份证号不能为空!", QMessageBox::Ok, this);
			return;
		}

		QString strGender = "男";
		int nID = pButtonGrpGender->checkedId();
		if (nID == 1)
			strGender = "女";
		QString strNationalCode = ui->comboBox_Nationality->currentData().toString();
		QString strNational = ui->comboBox_Nationality->currentText();
		IDCardInfoPtr pIDCard = make_shared<IDCardInfo>();
		// 012345
		// 362331197612164214
		strcpy_s((char*)pIDCard->szName, 32, strName.toLocal8Bit().data());
		strcpy_s((char*)pIDCard->szIdentity, 36, strIDentity.toLocal8Bit().data());
		strcpy_s((char*)pIDCard->szBirthday, 24, strIDentity.toStdString().substr(6, 8).c_str());
		strcpy_s((char*)pIDCard->szGender, 8, strGender.toLocal8Bit().data());
		strcpy_s((char*)pIDCard->szNationalty, 20, strNational.toLocal8Bit().data());
		strcpy_s((char*)pIDCard->szNationaltyCode, 20, strNationalCode.toLocal8Bit().data());
		g_pDataCenter->SetIDCardInfo(pIDCard);
	}
	else
	{
		QString strCardProgress;
		IDCardInfoPtr& pIDCard = g_pDataCenter->GetIDCardInfo();
		if (!pIDCard)
		{
			QMessageBox_CN(QMessageBox::Information, tr("提示"), "身份证数据无效,请先读取身份证或输入身份证信息!", QMessageBox::Ok, this);
			return;
		}
	}
	ui->pushButton_LoadCardData->setEnabled(true);
	ui->pushButton_MakeCard->setEnabled(true);
	IDCardInfoPtr& pIDCard = g_pDataCenter->GetIDCardInfo();
	SSCardBaseInfoPtr pSSCardInfo = make_shared<SSCardBaseInfo>();
	RegionInfo& Reginfo = g_pDataCenter->GetSysConfigure()->Region;
	string strBankName;
	do
	{
		pSSCardInfo->strName = (const char*)pIDCard->szName;
		pSSCardInfo->strIdentity = (const char*)pIDCard->szIdentity;
		pSSCardInfo->strOrganID = Reginfo.strAgency;
		pSSCardInfo->strBankCode = Reginfo.strBankCode;
		pSSCardInfo->strTransType = "5";
		pSSCardInfo->strCity = Reginfo.strCityCode;
		pSSCardInfo->strSSQX = Reginfo.strCountry;
		pSSCardInfo->strCardVender = Reginfo.strCardVendor;

		g_pDataCenter->SetSSCardInfo(pSSCardInfo);
		SSCardBaseInfoPtr pTempSSCardInfo = make_shared<SSCardBaseInfo>();
		pTempSSCardInfo->strName = (const char*)pIDCard->szName;
		pTempSSCardInfo->strIdentity = (const char*)pIDCard->szIdentity;

		CJsonObject jsonIn;
		jsonIn.Add("CardID", (char*)pIDCard->szIdentity);
		jsonIn.Add("Name", (const char*)pIDCard->szName);
		jsonIn.Add("City", g_pDataCenter->GetSysConfigure()->Region.strCityCode);
		jsonIn.Add("BankCode", g_pDataCenter->GetSysConfigure()->Region.strBankCode);
		string strJsonIn = jsonIn.ToString();
		string strJsonOut;
		if (QFailed(pService->QueryCardInfo(strJsonIn, strJsonOut)))
		{
			CJsonObject jsonOut(strJsonOut);
			string strText, strErrcode;
			int nErrCode = -1;
			jsonOut.Get("Result", nErrCode);
			jsonOut.Get("Message", strText);
			jsonOut.Get("errcode", strErrcode);
			ui->textEdit->setText(strText.c_str());
			strMessage = QString("查询制卡状态失败:%1").arg(QString::fromLocal8Bit(strText.c_str()));
			break;
		}
		CJsonObject jsonOut(strJsonOut);
		/*
			outJson.Add("Name", CardInfo.strName);
			outJson.Add("CardID", CardInfo.strCardID);
			outJson.Add("CardNum", CardInfo.strCardNum);
			outJson.Add("BankName", strBankName);
			outJson.Add("BankCode", CardInfo.strBankCode);
			outJson.Add("CardStatus", (int)nCardStatus);
			outJson.Add("Mobile", CardInfo.strMobile);
		*/
		jsonOut.Get("CardNum", pSSCardInfo->strCardNum);
		string strBankName;
		jsonOut.Get("BankName", strBankName);

		jsonOut.Get("Mobile", pSSCardInfo->strMobile);

		const char* szStatus[] = {
				"放号",
				"正常",
				"挂失",
				"临时挂失",
				"挂失后注销",
				"注销",
				"未启用"
		};
		int nCardStatus = -1;
		jsonOut.Get("CardStatus", nCardStatus);
		if (nCardStatus >= 0 && nCardStatus <= sizeof(szStatus) / sizeof(char*))
			pSSCardInfo->strCardStatus = szStatus[(int)nCardStatus];
		else
			pSSCardInfo->strCardStatus = "未知";
		ui->lineEdit_Status->setText(pSSCardInfo->strCardStatus.c_str());
		ui->lineEdit_Bank->setText(QString::fromLocal8Bit(strBankName.c_str()));
		ui->lineEditl_SSCard->setText(pSSCardInfo->strCardNum.c_str());
		ui->lineEdit_Mobile->setText(pSSCardInfo->strMobile.c_str());
		ui->lineEdit_Status->setText(pSSCardInfo->strCardStatus.c_str());

		nResult = 0;
	} while (0);

	if (QFailed(nResult))
	{
		ui->textEdit->setText(strMessage);
		gError() << gQStr(strMessage);
		QMessageBox_CN(QMessageBox::Information, tr("提示"), strMessage, QMessageBox::Ok, this);
	}
}

void Sys_ManualMakeCard::on_checkBox_WithoutIDCard_stateChanged(int arg1)
{
	bool bStatus = ui->checkBox_WithoutIDCard->isChecked();
	ui->lineEdit_Name->setReadOnly(!bStatus);
	ui->lineEdit_CardID->setReadOnly(!bStatus);
	ui->lineEdit_IssuedDate->setReadOnly(!bStatus);
	ui->lineEdit_ExpiredDate->setReadOnly(!bStatus);
	ui->pushButton_ReadID->setEnabled(!bStatus);
	ui->lineEdit_Mobile->setReadOnly(!bStatus);
	ui->radioButton_Male->setEnabled(bStatus);
	ui->radioButton_Female->setEnabled(bStatus);
	ui->comboBox_Nationality->setEnabled(bStatus);
}

void Sys_ManualMakeCard::on_radioButton_NewCard_clicked()
{

}


void Sys_ManualMakeCard::on_radioButton_ReplaceCard_clicked()
{

}

void Sys_ManualMakeCard::on_checkBox_Debug_stateChanged(int arg1)
{
	if (arg1)
	{
		QString strMessage;
		IDCardInfoPtr pIDCard = make_shared<IDCardInfo>();
		SSCardBaseInfoPtr pSSCardInfo = make_shared<SSCardBaseInfo>();
		if (QFailed(LoadTestIDData(pIDCard, pSSCardInfo)))
		{
			QMessageBox_CN(QMessageBox::Information, "提示", "加载测试数据失败!", QMessageBox::Ok, this);
			return;
		}
		ui->lineEdit_CardID->setText((char*)pIDCard->szIdentity);
		ui->lineEdit_Mobile->setText(pSSCardInfo->strMobile.c_str());
		ui->lineEdit_Name->setText(QString::fromLocal8Bit((char*)pIDCard->szName));
		QString strGender = QString::fromLocal8Bit((char*)pIDCard->szGender);

		if (strGender == "男")
		{
			ui->radioButton_Male->setChecked(true);
			ui->radioButton_Female->setChecked(false);
		}
		else if (strGender == "女")
		{
			ui->radioButton_Male->setChecked(false);
			ui->radioButton_Female->setChecked(true);
		}
		else
		{
			ui->radioButton_Male->setChecked(false);
			ui->radioButton_Female->setChecked(false);
		}
		QString strNationality = QString::fromLocal8Bit((char*)pIDCard->szNationalty);
		ui->comboBox_Nationality->setCurrentText(strNationality);
		ui->lineEdit_IssuedDate->setText(QString::fromLocal8Bit((char*)pIDCard->szExpirationDate1));
		ui->lineEdit_ExpiredDate->setText(QString::fromLocal8Bit((char*)pIDCard->szExpirationDate2));

		g_pDataCenter->SetSSCardInfo(pSSCardInfo);
		g_pDataCenter->SetIDCardInfo(pIDCard);
		//g_pDataCenter->OpenSSCardReader(strMessage);
		//g_pDataCenter->OpenPrinter(strMessage);
	}
}
