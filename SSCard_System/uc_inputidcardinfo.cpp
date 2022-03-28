#include "uc_inputidcardinfo.h"
#include "ui_uc_inputidcardinfo.h"
#include "Gloabal.h"
//#include "qnationwidget.h"
#include "uc_readidcard.h"
#include "Payment.h"

uc_InputIDCardInfo::uc_InputIDCardInfo(QWidget* parent) :
	QWidget(parent),
	ui(new Ui::uc_InputIDCardInfo)
{
	ui->setupUi(this);
	ui->lineEdit_CardID->setValidator(new QRegExpValidator(QRegExp("^[1-9]\\d{5}(18|19|([23]\\d))\\d{2}((0[1-9])|(10|11|12))(([0-2][1-9])|10|20|30|31)\\d{3}[0-9Xx]$")));   //只能输入数字
	//ui->lineEdit_Mobile->setValidator(new QRegExpValidator(QRegExp("^((13[0-9])|(14[5|7])|(15([0-3]|[5-9]))|(18[0,5-9]))\\d{8}$")));   //只能输入数字
	//ui->lineEdit_ExpiredDate->setValidator(new QRegExpValidator(QRegExp("((\\d{3}[1-9]|\\d{2}[1-9]\\d|\\d[1-9]\\d{2}|[1-9]\\d{3})(((0[13578]|1[02])(0[1-9]|[12]\\d|3[01]))|((0[469]|11)(0[1-9]|[12]\\d|30))|(02(0[1-9]|[1]\\d|2[0-8]))))|(((\\d{2})(0[48]|[2468][048]|[13579][26])|((0[48]|[2468][048]|[3579][26])00))0229)")));
	//ui->lineEdit_IssuedDate->setValidator(new QRegExpValidator(QRegExp("((\\d{3}[1-9]|\\d{2}[1-9]\\d|\\d[1-9]\\d{2}|[1-9]\\d{3})(((0[13578]|1[02])(0[1-9]|[12]\\d|3[01]))|((0[469]|11)(0[1-9]|[12]\\d|30))|(02(0[1-9]|[1]\\d|2[0-8]))))|(((\\d{2})(0[48]|[2468][048]|[13579][26])|((0[48]|[2468][048]|[3579][26])00))0229)")));
	for (auto var : g_vecNationCode)
	{
		ui->comboBox_Nationality->addItem(var.strNationalty.c_str(), var.strCode.c_str());
	}

	pButtonGrp = new QButtonGroup(this);
	pButtonGrp->addButton(ui->radioButton_Male, 0);
	pButtonGrp->addButton(ui->radioButton_Female, 1);
	pButtonGrp->setExclusive(true);
	if (g_pDataCenter->bDebug)
	{
		SSCardBaseInfoPtr pSSCardInfo = make_shared<SSCardBaseInfo>();
		IDCardInfoPtr pIDCard = make_shared<IDCardInfo>();
		if (QSucceed(LoadTestIDData(pIDCard, pSSCardInfo)))
		{
			ui->lineEdit_Name->setText(QString::fromLocal8Bit((char*)pIDCard->szName));
			ui->lineEdit_CardID->setText(QString::fromLocal8Bit((char*)pIDCard->szIdentity));
			ui->comboBox_Nationality->setCurrentText(QString::fromLocal8Bit((char*)pIDCard->szNationalty));
			QDateTime IssuedDate = QDateTime::fromString((char*)pIDCard->szExpirationDate1, "yyyyMMdd");
			QDateTime ExpireDate = QDateTime::fromString((char*)pIDCard->szExpirationDate2, "yyyyMMdd");
			ui->dateEdit_Issued->setDateTime(IssuedDate);
			ui->dateEdit_Expired->setDateTime(ExpireDate);
			if (QString::fromLocal8Bit((char*)pIDCard->szGender) == "男")
				ui->radioButton_Male->setChecked(true);
			else
				ui->radioButton_Female->setChecked(true);
		}
	}


	ui->comboBox_Career->clear();
	for (auto& var : vecCareer)
		ui->comboBox_Career->addItem(var.strName, var.strCode);
	//ui->comboBox_Nationality->setCurrentIndex(0);
	//connect(&ui->lineEdit_Nation,&QClickableLineEdit::clicked,this,)
	QString strMessage;
	QString strQSS = "QComboBox{padding: 1px 15px 1px 3px; border:1px solid rgba(228,228,228,1);}"
		"QComboBox::drop-down {border-style:none;}"
		"QComboBox QAbstractItemView{margin-top: 8px;margin-left: 6px;margin-bottom: 6px;margin-right: 6px;background:rgba(255,255,255,1);"
		"border:1px solid rgba(228,228,228,1);outline: 2px; padding-left: 16px;padding-top: 12px;padding-bottom:12px; }"
		"QComboBox::QAbstractItemView::item {height:56px;margin-top: 8px;margin-bottom: 8px; }"
		"QComboBox QScrollBar::vertical{width:36px;background:rgb(255,255,255);border:none;}"
		"QComboBox QScrollBar::handle::vertical{border-radius:3px;background-color: rgba(30, 34, 43, 0.2);}"
		"QComboBox QScrollBar::handle::vertical::hover{background-color: rgba(30, 34, 43, 0.2);}"
		"QComboBox QScrollBar::add-line::vertical{border:none;}"
		"QComboBox QScrollBar::sub-line::vertical{border:none;}";
	ui->comboBox_Province->setStyleSheet(strQSS);
	ui->comboBox_City->setStyleSheet(strQSS);
	ui->comboBox_County->setStyleSheet(strQSS);
	ui->comboBox_Town->setStyleSheet(strQSS);
	ui->comboBox_Street->setStyleSheet(strQSS);
	InitializeDB(strMessage);
}

bool  uc_InputIDCardInfo::InitializeDB(QString& strMessage)
{
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
		strMessage = "打开地址数据库失败,请检查地址数据库!";
		return false;
	}

	QSqlQuery query = SQLiteDB.exec("SELECT ID,Name FROM Area_2022 as t where t.LevelType=1 ORDER BY ID");
	while (query.next())
	{
		QString strID = query.value("ID").toString();
		QString strProvince = query.value("Name").toString();
		ui->comboBox_Province->addItem(strProvince, strID);
	}
	bInitialized = true;

	QString strCity = g_pDataCenter->GetSysConfigure()->Region.strCityCode.c_str();
	QString strQuery = QString("SELECT t.ID,t.Province,t.Name from Area_2022 as t where t.id=%1").arg(strCity);

	query = SQLiteDB.exec(strQuery);
	if (query.next())
	{
		QString strProvince = query.value("Province").toString();
		QString strName = query.value("Name").toString();
		int nProvince = ui->comboBox_Province->findText(strProvince);
		if (nProvince != -1)
			ui->comboBox_Province->setCurrentIndex(nProvince);
		else
			ui->comboBox_Province->setCurrentIndex(0);

		int nCity = ui->comboBox_City->findText(strName);
		if (nCity != -1)
			ui->comboBox_City->setCurrentIndex(nCity);
		else
			ui->comboBox_City->setCurrentIndex(0);
	}
	else
	{
		on_comboBox_Province_currentIndexChanged(0);
		on_comboBox_City_currentIndexChanged(0);
		on_comboBox_County_currentIndexChanged(0);
		on_comboBox_Town_currentIndexChanged(0);
	}

	return true;
}
uc_InputIDCardInfo::~uc_InputIDCardInfo()
{
	delete ui;
}

void uc_InputIDCardInfo::on_comboBox_Province_currentIndexChanged(int index)
{
	if (!bInitialized)
		return;
	ui->comboBox_City->clear();
	int nProvinces = ui->comboBox_Province->count();
	if (index >= nProvinces)
		return;
	QString strID = ui->comboBox_Province->itemData(index).toString();
	QSqlQuery query = SQLiteDB.exec(QString("SELECT id,Name FROM Area_2022 as t where   t.LevelType = 2 and t.ParentId = %1 ORDER BY ID").arg(strID));
	while (query.next())
	{
		QString strID = query.value("ID").toString();
		QString strName = query.value("Name").toString();
		ui->comboBox_City->addItem(strName, strID);
	}
}

void uc_InputIDCardInfo::on_comboBox_City_currentIndexChanged(int index)
{
	if (!bInitialized)
		return;
	ui->comboBox_County->clear();
	int nProvinces = ui->comboBox_City->count();
	if (index >= nProvinces)
		return;
	QString strID = ui->comboBox_City->itemData(index).toString();
	QSqlQuery query = SQLiteDB.exec(QString("SELECT id,Name FROM Area_2022 as t where   t.LevelType = 3 and t.ParentId = %1 ORDER BY ID").arg(strID));
	while (query.next())
	{
		QString strID = query.value("ID").toString();
		QString strName = query.value("Name").toString();
		ui->comboBox_County->addItem(strName, strID);
	}
}

void uc_InputIDCardInfo::on_comboBox_County_currentIndexChanged(int index)
{
	if (!bInitialized)
		return;
	ui->comboBox_Town->clear();
	int nProvinces = ui->comboBox_County->count();
	if (index >= nProvinces)
		return;
	QString strID = ui->comboBox_County->itemData(index).toString();
	QSqlQuery query = SQLiteDB.exec(QString("SELECT id,Name FROM Area_2022 as t where   t.LevelType = 4 and t.ParentId = %1 ORDER BY ID").arg(strID));
	while (query.next())
	{
		QString strID = query.value("ID").toString();
		QString strName = query.value("Name").toString();
		ui->comboBox_Town->addItem(strName, strID);
	}
}

void uc_InputIDCardInfo::on_comboBox_Town_currentIndexChanged(int index)
{
	if (!bInitialized)
		return;
	ui->comboBox_Street->clear();
	int nProvinces = ui->comboBox_Town->count();
	if (index >= nProvinces)
		return;
	QString strID = ui->comboBox_Town->itemData(index).toString();
	QSqlQuery query = SQLiteDB.exec(QString("SELECT id,Name FROM Area_2022 as t where   t.LevelType = 5 and t.ParentId = %1 ORDER BY ID").arg(strID));
	while (query.next())
	{
		QString strID = query.value("ID").toString();
		QString strName = query.value("Name").toString();
		ui->comboBox_Street->addItem(strName, strID);
	}
}


void uc_InputIDCardInfo::ResetPage()
{
	ui->lineEdit_Name->setText("");
	//ui->lineEdit_Mobile->setText("");
	ui->lineEdit_CardID->setText("");
	ui->comboBox_Nationality->setCurrentIndex(0);
	ui->radioButton_Male->setChecked(false);
	ui->radioButton_Female->setChecked(false);
	ui->pushButton_OK->setEnabled(false);
}

int	uc_InputIDCardInfo::GetSSCardInfo(/*IDCardInfoPtr &pIDCard,*/QString& strMessage)
{
	RegionInfo& Reginfo = g_pDataCenter->GetSysConfigure()->Region;
	IDCardInfoPtr& pIDCard = g_pDataCenter->GetIDCardInfo();
	if (!pIDCard)
	{
		strMessage = "身份证信息无效!";
		return -1;
	}
	//QString strMobile = ui->lineEdit_Mobile->text();

	SSCardBaseInfoPtr pSSCardInfo = make_shared<SSCardBaseInfo>();
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
	pSSCardInfo->strOccupType = ui->comboBox_Career->currentData().toString().toStdString();
	//pSSCardInfo->strMobile = strMobile.toStdString();

	g_pDataCenter->strCardVersion = "3.00";
	return 0;
}

int uc_InputIDCardInfo::GetCardInfo(/*IDCardInfoPtr& pIDCard,*/ QString& strMessage)
{
	QString strName = ui->lineEdit_Name->text();
	//QString strMobile = ui->lineEdit_Mobile->text();
	QString strCardID = ui->lineEdit_CardID->text();
	QString strNation = ui->comboBox_Nationality->currentText();
	QString strNationCode = ui->comboBox_Nationality->currentData().toString();
	QString strIssuedate = ui->dateEdit_Issued->dateTime().toString("yyyyMMdd");
	QString strExpireDate = ui->dateEdit_Expired->dateTime().toString("yyyyMMdd");
	QString strAddress = ui->lineEdit_Address->text();
	int nGender = pButtonGrp->checkedId();
	QString strGender = "";
	if (nGender == 0)
		strGender = "男";
	else if (nGender == 1)
		strGender = "女";

	if (strName.isEmpty() ||
		strCardID.isEmpty() ||
		strNation.isEmpty() ||
		strIssuedate.isEmpty() ||
		strExpireDate.isEmpty() ||
		strGender.isEmpty())
	{
		strMessage = "姓名、手机、民族、性别、身份证号码、证件有效期等信息不能为空!";
		return -1;
	}
	if (strCardID.size() < 18)
	{
		strMessage = "身份证号码位数不足，必须为18位的数字!";
		return -1;
	}
	char nX = VerifyCardID(strCardID.toStdString().c_str());
	if (nX != strCardID.toStdString().at(17))
	{
		strMessage = "身份证号码有误，请检查后重新输入!";
		return -1;
	}

	IDCardInfoPtr pIDCard = make_shared<IDCardInfo>();
	if (!pIDCard)
	{
		strMessage = "内存不足,无法生成身份证信息!";
		return -1;
	}
	strcpy((char*)pIDCard->szName, strName.toLocal8Bit().data());
	strcpy((char*)pIDCard->szIdentity, strCardID.toLocal8Bit().data());
	strcpy((char*)pIDCard->szGender, strGender.toLocal8Bit().data());
	strcpy((char*)pIDCard->szNationalty, strNation.toLocal8Bit().data());
	strcpy((char*)pIDCard->szNationaltyCode, strNationCode.toStdString().c_str());
	strcpy((char*)pIDCard->szExpirationDate1, strIssuedate.toLocal8Bit().data());
	strcpy((char*)pIDCard->szExpirationDate2, strExpireDate.toLocal8Bit().data());
	strcpy((char*)pIDCard->szAddress, strAddress.toLocal8Bit().data());
	g_pDataCenter->SetIDCardInfo(pIDCard);

	return 0;
}

void uc_InputIDCardInfo::on_pushButton_OK_clicked()
{
	WaitCursor();
	QString strMessage;
	RegionInfo Reginfo = g_pDataCenter->GetSysConfigure()->Region;
	do
	{
		if (QFailed(GetCardInfo(strMessage)))
		{
			break;
		}
		if (QFailed(GetSSCardInfo(strMessage)))
		{
			break;
		}
	} while (0);
	uc_ReadIDCard* pReadIDCard = ((uc_ReadIDCard*)parent());
	int nOperation = Page_EnsureInformation - Page_ReaderIDCard + Switch_NextPage - 1;
	MaskStatus nStatus = Success;
	QString strTips = "操作成功";
	if (strMessage.size())
	{
		if (g_pDataCenter->nCardServiceType == ServiceType::Service_NewCard)
			strMessage = "人员身份信息已经确认,稍后开始制卡!";
		else
			strMessage = "人员身份信息已经确认,稍后请确认社保卡信息!";
	}
	else
	{
		nStatus = Failed;
		nOperation = Stay_CurrentPage;
		strTips = "操作失败";
	}

	pReadIDCard->emit ShowMaskWidget(strTips, strMessage, Success, nOperation);
}


//void uc_InputIDCardInfo::on_lineEdit_Nation_clicked()
//{
//	((uc_ReadIDCard*)parent())->emit ShowNationWidget(true);
//}

void uc_InputIDCardInfo::on_ShowWidgets(QWidget* pWidget, bool bShow)
{
	ShowWidgets(this, bShow);
	/*if (bShow && pNationWidget)
		pNationWidget->hide();*/
}

//void uc_InputIDCardInfo::showEvent(QShowEvent* event)
//{
//	bool bShowWideget = false;
//	if (g_pDataCenter->nCardServiceType == ServiceType::Service_NewCard)
//	{
//		ui->label_Address->setStyleSheet("color:rgb(255, 0, 0);");
//	}
//	else
//	{
//		bShowWideget = true;
//		ui->label_Address->setStyleSheet("");
//	}
//	//ui->label_SSCard->setVisible(bShowWideget);
//	//ui->lineEditl_SSCard->setVisible(bShowWideget);
//	//ui->label_CardStatus->setVisible(bShowWideget);
//	//ui->lineEdit_Status->setVisible(bShowWideget);
//	//ui->label_Bank->setVisible(bShowWideget);
//	//ui->lineEdit_Bank->setVisible(bShowWideget);
//	//ui->pushButton_Query->setVisible(bShowWideget);
//
//}

//void uc_InputIDCardInfo::on_pushButton_Query_clicked()
//{
//	QString strMessage;
//	int nResult = -1;
//	QString strCardProgress;
//	RegionInfo& Reginfo = g_pDataCenter->GetSysConfigure()->Region;
//	int nErrCode = -1;
//	do
//	{
//		if (QFailed(GetCardInfo(strMessage)))
//		{
//			break;
//		}
//
//		SSCardService* pService = nullptr;
//		if (QFailed(g_pDataCenter->OpenSSCardService(&pService, strMessage)))
//		{
//			break;
//		}
//
//		if (!pService)
//		{
//			strMessage = "社保卡卡管服务不可用!";
//			break;
//		}
//
//		CJsonObject jsonIn;
//		string strCardID = ui->lineEdit_CardID->text().toStdString();
//		string strName = ui->lineEdit_Name->text().toLocal8Bit().data();
//
//		jsonIn.Add("CardID", strCardID);
//		jsonIn.Add("Name", strName);
//		jsonIn.Add("City", g_pDataCenter->GetSysConfigure()->Region.strCityCode);
//		jsonIn.Add("BankCode", g_pDataCenter->GetSysConfigure()->Region.strBankCode);
//		string strJsonIn = jsonIn.ToString();
//		string strJsonOut;
//		if (QFailed(pService->QueryCardInfo(strJsonIn, strJsonOut)))
//		{
//			CJsonObject jsonOut(strJsonOut);
//			string strText;
//
//			jsonOut.Get("Result", nErrCode);
//			jsonOut.Get("Message", strText);
//			strMessage = QString("查询制卡信息失败:%1").arg(QString::fromLocal8Bit(strText.c_str()));
//			break;
//		}
//
//		CJsonObject jsonOut(strJsonOut);
//		jsonOut.Get("Result", nErrCode);
//		if (QFailed(nErrCode))
//		{
//			string strText;
//			jsonOut.Get("Result", nErrCode);
//			jsonOut.Get("Message", strText);
//			strMessage = QString("查询制卡信息失败:%1").arg(QString::fromLocal8Bit(strText.c_str()));
//			break;
//		}
//		string strSSCardNum, strBankName;
//		int nCardStatus;
//		jsonOut.Get("CardNum", strSSCardNum);
//		jsonOut.Get("CardStatus", nCardStatus);
//		jsonOut.Get("BankName", strBankName);
//
//		//ui->lineEditl_SSCard->setText(QString::fromLocal8Bit(strSSCardNum.c_str()));
//		//ui->lineEdit_Status->setText(CardStatusString((CardStatus)nCardStatus));
//		//ui->lineEdit_Bank->setText(QString::fromLocal8Bit(strBankName.c_str()));
//
//		jsonIn.Clear();
//		jsonIn.Add("CardID", strCardID);
//		jsonIn.Add("Name", strName);
//		jsonIn.Add("City", Reginfo.strCityCode);
//
//		strJsonIn = jsonIn.ToString();
//		string strJsonout;
//		string strCommand = "QueryPersonPhoto";
//
//		if (QFailed(pService->SetExtraInterface(strCommand, strJsonIn, strJsonOut)))
//		{
//			CJsonObject jsonOut(strJsonOut);
//			string strText;
//			int nErrCode = -1;
//			jsonOut.Get("Result", nErrCode);
//			jsonOut.Get("Message", strText);
//			strMessage = QString("获取个人照片失败:%1").arg(QString::fromLocal8Bit(strText.c_str()));
//			break;
//		}
//		jsonOut.Parse(strJsonOut);
//		string strPhoto;
//		if (jsonOut.Get("Photo", strPhoto))
//		{
//			SaveSSCardPhoto(strMessage, strPhoto.c_str());
//			if (QFailed(SaveSSCardPhotoBase64(strMessage, strPhoto.c_str())))
//			{
//				strMessage = QString("保存照片数据失败!");
//				break;
//			}
//		}
//		else
//		{
//			strMessage = QString("社保后台未返回个人照片!");
//			break;
//		}
//
//		strCommand = "QueryPersonInfo";
//		if (QFailed(pService->SetExtraInterface(strCommand, strJsonIn, strJsonOut)))
//		{
//			CJsonObject jsonOut(strJsonOut);
//			string strText;
//			int nErrCode = -1;
//			jsonOut.Get("Result", nErrCode);
//			jsonOut.Get("Message", strText);
//			strMessage = QString("获取个人照片失败:%1").arg(QString::fromLocal8Bit(strText.c_str()));
//			break;
//		}
//		jsonOut.Parse(strJsonOut);
//		string strAddress;
//		jsonOut.Get("Address", strAddress);
//		/*
//		jsonOut.Get("CardID", CardInfo.strCardID);
//		jsonOut.Get("CardNum", CardInfo.strCardNum);
//		jsonOut.Get("Name", CardInfo.strName);
//		jsonOut.Get("Gender", CardInfo.strSex);
//		jsonOut.Get("Birthday", CardInfo.strBirthday);
//		jsonOut.Get("Nation", CardInfo.strNation);
//		jsonOut.Get("Address", CardInfo.strAdress);
//		jsonOut.Get("Mobile", CardInfo.strMobile);
//		jsonOut.Get("OccupType", CardInfo.strOccupType);
//		*/
//		ui->lineEdit_Address->setText(QString::fromLocal8Bit(strAddress.c_str()));
//
//		QString strStyle = QString("border-image: url(%1);").arg(g_pDataCenter->strSSCardPhotoFile.c_str());
//		ui->label_Photo->setStyleSheet(strStyle);
//		nResult = 0;
//	} while (0);
//
//	if (QFailed(nResult))
//	{
//		((uc_ReadIDCard*)parent())->emit ShowMaskWidget("操作失败", strMessage, Fetal, Stay_CurrentPage);
//	}
//	ui->pushButton_OK->setEnabled(true);
//	return;
//}
