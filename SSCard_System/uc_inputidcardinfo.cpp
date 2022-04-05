#include "uc_inputidcardinfo.h"
#include "ui_uc_inputidcardinfo.h"
#include "Gloabal.h"
//#include "qnationwidget.h"
#include "uc_readidcard.h"
#include "Payment.h"
#include "sys_dialogreadidcard.h"
#include "mainwindow.h"
#include "Sys_dialogcameratest.h"

uc_InputIDCardInfo::uc_InputIDCardInfo(QLabel* pTitle, QString strStepImage, Page_Index nIndex, QWidget* parent) :
	QStackPage(pTitle, strStepImage, nIndex, parent),
	ui(new Ui::uc_InputIDCardInfo)
{
	ui->setupUi(this);

	ShowGuardianWidget(false);
	ui->lineEdit_CardID->setValidator(new QRegExpValidator(QRegExp("^[1-9]\\d{5}(18|19|([23]\\d))\\d{2}((0[1-9])|(10|11|12))(([0-2][1-9])|10|20|30|31)\\d{3}[0-9Xx]$")));   //只能输入数字
	ui->lineEdit_GuardianMobile->setValidator(new QRegExpValidator(QRegExp("^((13[0-9])|(14[5|7])|(15([0-3]|[5-9]))|(18[0,5-9]))\\d{8}$")));   //只能输入数字
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

	ui->checkBox_Agency->setStyleSheet("QCheckBox::indicator {width: 48px;height: 48px;}\
            QCheckBox::indicator:unchecked{image:url(./Image/CheckBox_UnCheck.png);}\
            QCheckBox::indicator:checked{image:url(./Image/CheckBox_Checked.png);}\
            QCheckBox{font-weight:normal;line-height:49px;letter-spacing:1px;color:#707070;font:42px \"思源黑体 CN Medium\";border-radius: 24px;}");
	ui->comboBox_Career->clear();
	for (auto& var : vecCareer)
	{
		ui->comboBox_Career->addItem(var.strName, var.strCode);
	}

	//ui->comboBox_Nationality->setCurrentIndex(0);
	//connect(&ui->lineEdit_Nation,&QClickableLineEdit::clicked,this,)
	QString strMessage;
	QString strQSS = "QComboBox{padding: 1px 15px 1px 3px; border:1px solid rgba(228,228,228,1);}"
		"QComboBox::drop-down { subcontrol-origin: padding;subcontrol-position: top right; width: 36px;border:none;}"
		"QComboBox::drop-down {	image: url(:/Image/down_arrow.png);}"
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
	ui->comboBox_Guardianship->setStyleSheet(strQSS);
	ui->comboBox_Nationality->setStyleSheet(strQSS);
	ui->comboBox_Career->setStyleSheet(strQSS);
	InitializeDB(strMessage);
	connect(this, &uc_InputIDCardInfo::AddNewIDCard, this, &uc_InputIDCardInfo::on_AddNewIDCard, Qt::QueuedConnection);
	qDebug() << geometry();
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

int uc_InputIDCardInfo::ProcessBussiness()
{
	MainWindow* pMainWind = (MainWindow*)qApp->activeWindow();
	if (pMainWind && pMainWind->pLastStackPage)
	{
		pMainWind->pLastStackPage->ResetTimer(true, this);
	}
	ResetPage();
	return 0;
}
void uc_InputIDCardInfo::OnTimeout()
{
	gInfo() << __FUNCTION__;
	ShutDown();
}
void  uc_InputIDCardInfo::ShutDown()
{
	gInfo() << __FUNCTION__;
	if (pDialogReadIDCard)
	{
		pDialogReadIDCard->reject();
		delete pDialogReadIDCard;
		pDialogReadIDCard = nullptr;
	}

	StopDetect();
}
void uc_InputIDCardInfo::timerEvent(QTimerEvent* event)
{

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
	gInfo() << __FUNCTION__;
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
	gInfo() << __FUNCTION__;
	ui->lineEdit_Name->setText("");
	//ui->lineEdit_Mobile->setText("");
	ui->lineEdit_CardID->setText("");
	ui->lineEdit_Address->setText("");
	ui->lineEdit_GuardianMobile->setText("");
	ui->comboBox_Nationality->setCurrentIndex(0);
	ui->radioButton_Male->setChecked(false);
	ui->radioButton_Female->setChecked(false);
	ui->pushButton_OK->setEnabled(true);
	ui->checkBox_Agency->setChecked(false);
	pIDCard = nullptr;
	pSSCardInfo = nullptr;
	ui->label_Photo->setStyleSheet("border-image: url();");
	ui->label_Photo->setText("照片");
	ShowGuardianWidget(false);
	ClearGuardianInfo();
}
#include <ctime>

int GetAge(string strBirthday)
{
	if (strBirthday.empty())
		return -1;

	int nBirthYear, nBirthMonth, nBirthDay;
	int nCount = sscanf_s(strBirthday.c_str(), "%04d%02d%02d", &nBirthYear, &nBirthMonth, &nBirthDay);
	if (nCount != 3)
		return -1;

	time_t tNow = time(nullptr);
	tm* tmNow = localtime(&tNow);
	tmNow->tm_year += 1900;
	tmNow->tm_mon += 1;

	int nAge = tmNow->tm_year - nBirthYear;
	if (tmNow->tm_mon < nBirthMonth)
	{
		nAge--;
	}
	else if (tmNow->tm_mon == nBirthMonth)
	{
		if (tmNow->tm_mday < nBirthDay)
			nAge--;
	}
	return nAge;
}


int	uc_InputIDCardInfo::GetSSCardInfo(/*IDCardInfoPtr &pIDCard,*/QString& strMessage)
{
	gInfo() << __FUNCTION__;
	RegionInfo& Reginfo = g_pDataCenter->GetSysConfigure()->Region;
	IDCardInfoPtr& pIDCard = g_pDataCenter->GetIDCardInfo();
	if (!pIDCard)
	{
		strMessage = "身份证信息无效!";
		return -1;
	}
	QString strMobile = ui->lineEdit_GuardianMobile->text();
	if (strMobile.isEmpty())
	{
		strMessage = "手机号码不能为空!";
		return -1;
	}

	SSCardBaseInfoPtr pSSCardInfo = make_shared<SSCardBaseInfo>();
	pSSCardInfo->strName = (const char*)pIDCard->szName;
	pSSCardInfo->strGender = (const char*)pIDCard->szGender;
	pSSCardInfo->strNation = (const char*)pIDCard->szNationalty;
	pSSCardInfo->strNationCode = (const char*)pIDCard->szNationaltyCode;
	pSSCardInfo->strBirthday = (const char*)pIDCard->szBirthday;
	pSSCardInfo->strIdentity = (const char*)pIDCard->szIdentity;
	pSSCardInfo->strAddress = (const char*)pIDCard->szAddress;

	pSSCardInfo->strOrganID = Reginfo.strAgency;
	pSSCardInfo->strCity = Reginfo.strCityCode;
	pSSCardInfo->strSSQX = Reginfo.strCountry;
	pSSCardInfo->strCardVender = Reginfo.strCardVendor;
	pSSCardInfo->strBankCode = Reginfo.strBankCode;
	pSSCardInfo->strOccupType = ui->comboBox_Career->currentData().toString().toStdString();
	pSSCardInfo->strMobile = strMobile.toStdString();
	if (ui->checkBox_Agency->isChecked())
	{
		QString strGuardian = ui->lineEdit_Guardian->text();
		QString strGuardianCardID = ui->lineEdit_GuardianCardID->text();
		if (strGuardian.isEmpty() || strGuardianCardID.isEmpty())
		{
			strMessage = "监护人或监护人证件不能为空!";
			return -1;
		}

		if (strGuardianCardID.size() < 18)
		{
			strMessage = "监护人证件号码位数不足,必须为18位的数字!";
			return -1;
		}
		char nX = VerifyCardID(strGuardianCardID.toStdString().c_str());
		if (nX != strGuardianCardID.toStdString().at(17))
		{
			strMessage = "监护人证件号码有误，请检查后重新输入!";
			return -1;
		}

		QString strGuandianShip = ui->comboBox_Guardianship->currentText();
		pSSCardInfo->strGuardianName = strGuardian.toStdString();
		pSSCardInfo->strGuardianIDentity = strGuardianCardID.toStdString();
		pSSCardInfo->strGuardianShip = strGuandianShip.toStdString();
	}
	g_pDataCenter->strCardVersion = "3.00";

	g_pDataCenter->SetSSCardInfo(pSSCardInfo);
	return 0;
}

int uc_InputIDCardInfo::GetCardInfo(/*IDCardInfoPtr& pIDCard,*/ QString& strMessage)
{
	gInfo() << __FUNCTION__;
	QString strName = ui->lineEdit_Name->text();
	//QString strMobile = ui->lineEdit_Mobile->text();
	QString strCardID = ui->lineEdit_CardID->text();
	QString strNation = ui->comboBox_Nationality->currentText();
	QString strNationCode = ui->comboBox_Nationality->currentData().toString();
	QString strIssuedate = ui->dateEdit_Issued->dateTime().toString("yyyyMMdd");
	QString strExpireDate = ui->dateEdit_Expired->dateTime().toString("yyyyMMdd");
	QString strAddress = ui->lineEdit_Address->text();
	QString strProvince = ui->comboBox_Province->currentText();
	QString strCity = ui->comboBox_City->currentText();
	QString strCounty = ui->comboBox_County->currentText();
	QString strTown = ui->comboBox_Town->currentText();
	QString strStreet = ui->comboBox_Street->currentText();
	int nGender = pButtonGrp->checkedId();
	QString strGender = "";
	if (nGender == 0)
		strGender = "男";
	else if (nGender == 1)
		strGender = "女";

	if (strName.isEmpty() ||
		strCardID.isEmpty() ||
		strNation.isEmpty() ||
		strGender.isEmpty() ||
		strAddress.isEmpty())
	{
		strMessage = "姓名、手机、民族、性别、身份证号码,住址等信息不能为空!";
		return -1;
	}
	char nX = VerifyCardID(strCardID.toStdString().c_str());
	if (nX != strCardID.toStdString().at(17))
	{
		strMessage = "身份证号码有误，请检查后重新输入!";
		return -1;
	}
	QString strBirthday = strCardID.mid(6, 8);
	int nAge = GetAge(strBirthday.toStdString());
	if (nAge >= 16)
	{
		if (strIssuedate.isEmpty() ||
			strExpireDate.isEmpty())
		{
			strMessage = "16周岁以上的人员必须输入证件有效期!";
			return -1;
		}
	}
	else
	{
		strIssuedate = "";
		strExpireDate = "";
	}

	pIDCard = make_shared<IDCardInfo>();
	if (!pIDCard)
	{
		strMessage = "内存不足,无法生成身份证信息!";
		return -1;
	}
	strcpy((char*)pIDCard->szName, strName.toLocal8Bit().data());
	strcpy((char*)pIDCard->szIdentity, strCardID.toLocal8Bit().data());
	strncpy((char*)pIDCard->szBirthday, (char*)&pIDCard->szIdentity[6], 8);
	strcpy((char*)pIDCard->szGender, strGender.toLocal8Bit().data());
	strcpy((char*)pIDCard->szNationalty, strNation.toLocal8Bit().data());
	strcpy((char*)pIDCard->szNationaltyCode, strNationCode.toStdString().c_str());
	strcpy((char*)pIDCard->szExpirationDate1, strIssuedate.toLocal8Bit().data());
	strcpy((char*)pIDCard->szExpirationDate2, strExpireDate.toLocal8Bit().data());
	QString strFullAddress = strProvince + strCity + strCounty + strTown + strStreet + strAddress;
	strcpy((char*)pIDCard->szAddress, strFullAddress.toLocal8Bit().data());
	g_pDataCenter->SetIDCardInfo(pIDCard);

	return 0;
}

void uc_InputIDCardInfo::on_AddNewIDCard(IDCardInfo* pIDCard)
{
	gInfo() << __FUNCTION__;
	shared_ptr<IDCardInfo> IDCardPtr(pIDCard);
	ui->lineEdit_Guardian->setText(QString::fromLocal8Bit((char*)IDCardPtr->szName));
	ui->lineEdit_GuardianCardID->setText(QString::fromLocal8Bit((char*)IDCardPtr->szIdentity));
	if (pDialogReadIDCard)
	{
		pDialogReadIDCard->accept();
	}
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

void uc_InputIDCardInfo::ShowGuardianWidget(bool bShow)
{
	gInfo() << __FUNCTION__;
	ui->label_Guardian->setVisible(bShow);
	ui->label_Guardianship->setVisible(bShow);
	ui->label_GuardianIDCard->setVisible(bShow);
	ui->lineEdit_Guardian->setVisible(bShow);
	ui->comboBox_Guardianship->setVisible(bShow);
	ui->lineEdit_GuardianCardID->setVisible(bShow);
	if (bShow)
	{
		ui->label_GuardianMobile->setMinimumSize(QSize(200, 40));
		ui->label_GuardianMobile->setMaximumSize(QSize(200, 16777215));

		horizontalSpacer_Mobile = new QSpacerItem(40, 20, QSizePolicy::Fixed, QSizePolicy::Minimum);
		ui->horizontalLayout_Guardian->insertSpacerItem(2, horizontalSpacer_Mobile);
	}
	else
	{
		ui->label_GuardianMobile->setMinimumSize(QSize(220, 40));
		ui->label_GuardianMobile->setMaximumSize(QSize(220, 16777215));
		ui->horizontalLayout_Guardian->removeItem(horizontalSpacer_Mobile);
	}
}

void	uc_InputIDCardInfo::ClearGuardianInfo()
{
	gInfo() << __FUNCTION__;
	ui->lineEdit_Guardian->setText("");
	ui->comboBox_Guardianship->setCurrentIndex(0);
	ui->lineEdit_GuardianCardID->setText("");
}

void uc_InputIDCardInfo::on_checkBox_Agency_stateChanged(int arg1)
{
	qDebug() << __FUNCTION__ << "arg1 =" << arg1;

}

void uc_InputIDCardInfo::StartDetect()
{
	gInfo() << __FUNCTION__;
	if (!m_pWorkThread)
	{
		m_bWorkThreadRunning = true;
		m_pWorkThread = new std::thread(&uc_InputIDCardInfo::ThreadWork, this);
		if (!m_pWorkThread)
		{
			QString strError = QString("内存不足,创建读卡线程失败!");
			gError() << strError.toLocal8Bit().data();
			emit ShowMaskWidget("严重错误", strError, Fetal, Return_MainPage);
			return;
		}
	}
}

void uc_InputIDCardInfo::ThreadWork()
{
	gInfo() << __FUNCTION__;
	auto tLast = high_resolution_clock::now();
	QString strError;
	IDCardInfo* pIDCard = new IDCardInfo();
	while (m_bWorkThreadRunning)
	{
		auto tDuration = duration_cast<milliseconds>(high_resolution_clock::now() - tLast);
		if (tDuration.count() >= 1000)
		{
			tLast = high_resolution_clock::now();
			int nResult = g_pDataCenter->ReaderIDCard(pIDCard);
			if (nResult == IDCard_Status::IDCard_Succeed)
			{
				emit AddNewIDCard(pIDCard);
				pIDCard = new IDCardInfo();
				continue;
			}
			else
			{
				char szText[256] = { 0 };
				GetErrorMessage((IDCard_Status)nResult, szText, sizeof(szText));
				strError = QString("读取身份证失败:%1").arg(szText);
				gInfo() << gQStr(strError);
			}
		}
		else
		{
			this_thread::sleep_for(chrono::milliseconds(200));
		}
	}
	if (pIDCard)
		delete pIDCard;
}

void uc_InputIDCardInfo::StopDetect()
{
	m_bWorkThreadRunning = false;
	if (m_pWorkThread && m_pWorkThread->joinable())
	{
		m_pWorkThread->join();
		m_pWorkThread = nullptr;
	}
	CloseReader();
}

void uc_InputIDCardInfo::on_pushButton_TakePhoto_clicked()
{
	gInfo() << __FUNCTION__;
	QString strMessage;
	int nResult = -1;
	do
	{
		if (QFailed(nResult = GetCardInfo(strMessage)))
		{
			break;
		}
		if (QFailed(nResult = GetSSCardInfo(strMessage)))
		{
			break;
		}
		nResult = 0;
	} while (0);
	if (QFailed(nResult))
	{
		//QMessageBox_CN(QMessageBox::Critical, "提示", strMessage, QMessageBox::Ok, this);
		emit ShowMaskWidget("提示", strMessage, Fetal, Stay_CurrentPage);
		return;
	}
	DialogCameraTest dialog;
	if (dialog.exec() == QDialog::Accepted)
	{
		QString strQSS = QString("border-image: url(%1);").arg(g_pDataCenter->strSSCardPhotoFile.c_str());
		ui->label_Photo->setStyleSheet(strQSS);
		ui->label_Photo->setText("");
	}
}

void uc_InputIDCardInfo::on_pushButton_SelectPhoto_clicked()
{
	gInfo() << __FUNCTION__;
	QString strMessage;
	int nResult = -1;

	try
	{
		do
		{
			if (QFailed(nResult = GetCardInfo(strMessage)))
			{
				break;
			}
			if (QFailed(nResult = GetSSCardInfo(strMessage)))
			{
				break;
			}

			QString strPhoto = QFileDialog::getOpenFileName(this, tr("请选择照片(必须包含完整的头像,尺寸大于或等于384x441像素)"), ".", tr("图像文件(*.jpg)"));
			if (strPhoto.isEmpty())
				return;
			if (!fs::exists(strPhoto.toLocal8Bit().data()))
				return;
			//QImage  ImageRead(strPhoto);
			QString strAppPath = QApplication::applicationDirPath();
			QString strPhotoPath1 = strAppPath + "/PhotoProcess/1.jpg";
			QString strPhotoPath2 = strAppPath + "/PhotoProcess/2.jpg";

			if (fs::exists(strPhotoPath1.toLocal8Bit().data()))
				fs::remove(strPhotoPath1.toLocal8Bit().data());

			if (fs::exists(strPhotoPath2.toLocal8Bit().data()))
				fs::remove(strPhotoPath2.toLocal8Bit().data());

			fs::copy(strPhoto.toLocal8Bit().data(), strPhotoPath1.toLocal8Bit().data());
			//ImageRead.save(strPhotoPath1, "jpg", 95);

			QString strHeaderImage;
			if (QFailed(ProcessHeaderImage(strHeaderImage, strMessage)))
				break;

			if (fs::exists(strPhotoPath2.toLocal8Bit().data()))
			{
				string strSSCardPhoto;
				if (QSucceed(GetImageStorePath(strSSCardPhoto, 1)))
				{
					if (fs::exists(strSSCardPhoto))
						fs::remove(strSSCardPhoto);
					fs::copy(strPhotoPath2.toLocal8Bit().data(), strSSCardPhoto);
					g_pDataCenter->strSSCardPhotoFile = strSSCardPhoto;
				}
				fs::remove(strPhotoPath2.toLocal8Bit().data());
			}

			QString strQSS = QString("border-image: url(%1);").arg(g_pDataCenter->strSSCardPhotoFile.c_str());
			ui->label_Photo->setStyleSheet(strQSS);
			ui->label_Photo->setText("");

			nResult = 0;
		} while (0);
		if (QFailed(nResult))
		{
			emit ShowMaskWidget("提示", strMessage, Fetal, Stay_CurrentPage);
			//QMessageBox_CN(QMessageBox::Critical, "提示", strMessage, QMessageBox::Ok, this);
			return;
		}
	}
	catch (std::exception& e)
	{
		//QMessageBox_CN(QMessageBox::Critical, "提示", QString::fromLocal8Bit(e.what()), QMessageBox::Ok, this);
		emit ShowMaskWidget("提示", QString::fromLocal8Bit(e.what()), Fetal, Stay_CurrentPage);
	}
}
void uc_InputIDCardInfo::on_pushButton_OK_clicked()
{
	gInfo() << __FUNCTION__;
	WaitCursor();
	QString strMessage;
	int nResult = -1;
	int nOperation = -1;
	int nPage = 0;
	MaskStatus nStatus = Success;
	QString strTips = "操作成功";
	SSCardService* pService = nullptr;
	do
	{
		if (QFailed(GetCardInfo(strMessage)))
			break;

		if (QFailed(GetSSCardInfo(strMessage)))
			break;

		if (g_pDataCenter->nCardServiceType == ServiceType::Service_NewCard)
		{
			if (QFailed(g_pDataCenter->OpenSSCardService(&pService, strMessage)))
				break;

			if (!pService)
				break;

			CJsonObject jsonIn;
			jsonIn.Add("CardID", (char*)pIDCard->szIdentity);
			jsonIn.Add("Name", (const char*)pIDCard->szName);
			jsonIn.Add("City", g_pDataCenter->GetSysConfigure()->Region.strCityCode);
			jsonIn.Add("BankCode", g_pDataCenter->GetSysConfigure()->Region.strBankCode);
			string strJsonIn = jsonIn.ToString();
			string strJsonOut;
			if (QFailed(pService->QueryCardStatus(strJsonIn, strJsonOut)))
			{
				CJsonObject jsonOut(strJsonOut);
				string strText, strErrcode;
				int nErrCode = -1;
				//jsonOut.Get("Result", nErrCode);
				jsonOut.Get("Message", strText);
				jsonOut.Get("errcode", strErrcode);
				nErrCode = strtol(strErrcode.c_str(), nullptr, 10);
				if (nErrCode == 3)	// 已经申请过,则继续制卡
				{
					strMessage = QString::fromLocal8Bit(strText.c_str());
					break;
					//emit ShowMaskWidget("操作成功", strMessage, Success, Goto_Page, Page_MakeCard);
				}
				else
				{
					nStatus = Fetal;
					strMessage = QString("查询制卡状态失败:%1").arg(QString::fromLocal8Bit(strText.c_str()));
					break;
				}
			}

			nOperation = Goto_Page;
			nPage = Page_MakeCard;
			strMessage = "人员身份信息已经确认,稍后开始制卡!";
		}
		else
		{
			nOperation = Goto_Page;
			nPage = Page_EnsureInformation;
			strMessage = "人员身份信息已经确认,稍后请确认社保卡信息!";
		}

		nResult = 0;
	} while (0);

	if (QFailed(nResult))
	{
		nStatus = Failed;
		nOperation = Stay_CurrentPage;
		strTips = "操作失败";
	}

	emit ShowMaskWidget(strTips, strMessage, nStatus, nOperation, nPage);
}

void uc_InputIDCardInfo::on_checkBox_Agency_clicked()
{
	bool bChecked = ui->checkBox_Agency->isChecked();
	ShowGuardianWidget(bChecked);
	g_pDataCenter->bGuardian = bChecked;
	if (bChecked)
	{

		StartDetect();
		pDialogReadIDCard = new Sys_DialogReadIDCard("请刷监护人身份证");
		if (pDialogReadIDCard->exec() != QDialog::Accepted)
		{
			ui->checkBox_Agency->setChecked(false);
			ShowGuardianWidget(false);
			g_pDataCenter->bGuardian = false;
		}
		delete pDialogReadIDCard;
		pDialogReadIDCard = nullptr;
		StopDetect();
	}

}
