﻿#include "sys_sscardapitest.h"
#include "ui_sys_sscardapitest.h"
#include "Gloabal.h"
#include "uc_readidcard.h"
#include "Payment.h"
#include "sys_dialogreadidcard.h"
#include "mainwindow.h"
#include "Sys_dialogcameratest.h"
#include <QStringListModel>

Sys_SSCardAPITest::Sys_SSCardAPITest(QWidget* parent) :
	QWidget(parent),
	ui(new Ui::Sys_SSCardAPITest)
{
	ui->setupUi(this);
	ui->lineEdit_CardID->setValidator(new QRegExpValidator(QRegExp("^[1-9]\\d{5}(18|19|([23]\\d))\\d{2}((0[1-9])|(10|11|12))(([0-2][1-9])|10|20|30|31)\\d{3}[0-9Xx]$")));   //只能输入数字
	//ui->lineEdit_PostCode->setValidator(new QRegExpValidator(QRegExp("\\d{6}")));
	//ui->lineEdit_Mobile->setValidator(new QRegExpValidator(QRegExp("^((13[0-9])|(14[5|7])|(15([0-3]|[5-9]))|(18[0,5-9]))\\d{8}$")));   //只能输入数字
	//ui->lineEdit_ExpiredDate->setValidator(new QRegExpValidator(QRegExp("((\\d{3}[1-9]|\\d{2}[1-9]\\d|\\d[1-9]\\d{2}|[1-9]\\d{3})(((0[13578]|1[02])(0[1-9]|[12]\\d|3[01]))|((0[469]|11)(0[1-9]|[12]\\d|30))|(02(0[1-9]|[1]\\d|2[0-8]))))|(((\\d{2})(0[48]|[2468][048]|[13579][26])|((0[48]|[2468][048]|[3579][26])00))0229)")));
	//ui->lineEdit_IssuedDate->setValidator(new QRegExpValidator(QRegExp("((\\d{3}[1-9]|\\d{2}[1-9]\\d|\\d[1-9]\\d{2}|[1-9]\\d{3})(((0[13578]|1[02])(0[1-9]|[12]\\d|3[01]))|((0[469]|11)(0[1-9]|[12]\\d|30))|(02(0[1-9]|[1]\\d|2[0-8]))))|(((\\d{2})(0[48]|[2468][048]|[13579][26])|((0[48]|[2468][048]|[3579][26])00))0229)")));
	for (auto var : g_vecNationCode)
	{
		ui->comboBox_Nationality->addItem(var.strNationalty.c_str(), var.strCode.c_str());
	}

	ui->lineEdit_City->setText(g_pDataCenter->GetSysConfigure()->Region.strCityCode.c_str());
	ui->lineEdit_Country->setText(g_pDataCenter->GetSysConfigure()->Region.strCountry.c_str());
	ui->lineEdit_Agency->setText(g_pDataCenter->GetSysConfigure()->Region.strAgency.c_str());

	pButtonGrp = new QButtonGroup(this);
	pButtonGrp->addButton(ui->radioButton_Male, 0);
	pButtonGrp->addButton(ui->radioButton_Female, 1);
	pButtonGrp->setExclusive(true);
	if (g_pDataCenter->bDebug)
	{
		SSCardInfoPtr pSSCardInfo = make_shared<SSCardInfo>();
		IDCardInfoPtr pIDCard = make_shared<IDCardInfo>();
		if (QSucceed(LoadTestIDData(pIDCard, pSSCardInfo)))
		{
			ui->lineEdit_Name->setText(QString::fromLocal8Bit((char*)pIDCard->szName));
			ui->lineEdit_CardID->setText(QString((char*)pIDCard->szIdentity));
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
	ui->lineEdit_Name->installEventFilter(this);
	ui->lineEdit_Name->setFocusPolicy(Qt::ClickFocus);
	pCompleter = new QCompleter(this);
	if (pCompleter)
	{
		QAbstractItemView* popup = pCompleter->popup();
		if (popup)
			popup->setStyleSheet("font-weight:normal;line-height:49px;letter-spacing:1px;color:#707070;font:42px \"思源黑体 CN Medium\";");
	}

	//ui->checkBox_Agency->setStyleSheet("QCheckBox::indicator {width: 48px;height: 48px;}\
    //        QCheckBox::indicator:unchecked{image:url(./Image/CheckBox_UnCheck.png);}\
    //        QCheckBox::indicator:checked{image:url(./Image/CheckBox_Checked.png);}\
    //        QCheckBox{font-weight:normal;line-height:49px;letter-spacing:1px;color:#707070;font:42px \"思源黑体 CN Medium\";border-radius: 24px;}");
	//ui->comboBox_PersonType->clear();
	//for (auto& var : vecCareer)
	//{
	//	ui->comboBox_Career->addItem(var.strName, var.strCode);
	//}

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
	//ui->comboBox_Province->setStyleSheet(strQSS);
	//ui->comboBox_City->setStyleSheet(strQSS);
	//ui->comboBox_County->setStyleSheet(strQSS);
	///*ui->comboBox_Town->setStyleSheet(strQSS);
	//ui->comboBox_Street->setStyleSheet(strQSS);*/
	//ui->comboBox_Guardianship->setStyleSheet(strQSS);
	ui->comboBox_Nationality->setStyleSheet(strQSS);
	ui->comboBox_PersonType->setStyleSheet(strQSS);
	connect(this, &Sys_SSCardAPITest::AddNewIDCard, this, &Sys_SSCardAPITest::on_AddNewIDCard, Qt::QueuedConnection);
	qDebug() << geometry();
	ui->label_Department->hide();
	ui->lineEdit_Department->hide();
	//ui->label_Class->hide();
	//ui->lineEdit_Class->hide();
	ui->lineEdit_Name->setCompleter(pCompleter);
	connect(ui->lineEdit_Name, &QLineEdit::textChanged, this, &Sys_SSCardAPITest::on_Name_textChanged);
	on_pushButton_ResetPage_clicked();
}

Sys_SSCardAPITest::~Sys_SSCardAPITest()
{
	delete ui;
}

bool Sys_SSCardAPITest::LoadPersonInfo(QString strJson)
{
	try
	{
		string strProgressFile = strJson.toLocal8Bit().data();
		if (!fs::exists(strProgressFile))
			return false;

		ifstream ifs(strProgressFile, ios::in | ios::binary);
		stringstream ss;
		ss << ifs.rdbuf();
		CJsonObject json;
		if (!json.Parse(ss.str()))
			return false;

		char szPersonType[8] = { 0 };
		char szMobile[32] = { 0 };
		char szBankCode[32] = { 0 };
		char szCommunity[64] = { 0 };
		char szCompany[64] = { 0 };
		char szDepartment[64] = { 0 };
		
		IDCardInfoPtr pIDCard = make_shared<IDCardInfo>();

		GetJsonValue(json, "Identity", pIDCard->szIdentity);
		GetJsonValue(json, "Name", pIDCard->szName);		
		GetJsonValue(json, "Gender", pIDCard->szGender);
		GetJsonValue(json, "Birthday", pIDCard->szBirthday);
		GetJsonValue(json, "Nation", pIDCard->szNationalty);
		GetJsonValue(json, "Address", pIDCard->szAddress);
		GetJsonValue(json, "PaperIssuedate", pIDCard->szExpirationDate1);
		GetJsonValue(json, "PaperExpiredate", pIDCard->szExpirationDate2);

		GetJsonValue(json, "Mobile", szMobile);
		GetJsonValue(json, "PersonType", szPersonType);
		GetJsonValue(json, "BankCode", szBankCode);
		GetJsonValue(json, "Community", szCommunity);
		GetJsonValue(json, "Company", szCompany);
		GetJsonValue(json, "Department", szDepartment);
		
		g_pDataCenter->SetIDCardInfo(pIDCard);
		
		ui->lineEdit_Name->setText(QString::fromLocal8Bit((char *)pIDCard->szName));
		ui->lineEdit_CardID->setText((char*)pIDCard->szIdentity);
		ui->lineEdit_Mobile->setText((char*)szMobile);

		int nGender = atoi((char *)pIDCard->szGender);
		if (nGender == 1)
			ui->radioButton_Male->setChecked(true);
		else if (nGender == 2)
			ui->radioButton_Female->setChecked(true);

		ui->dateEdit_Issued->setDateTime(QDateTime::fromString((char *)pIDCard->szExpirationDate1, "yyyyMMdd"));
		ui->dateEdit_Expired->setDateTime(QDateTime::fromString((char *)pIDCard->szExpirationDate2, "yyyyMMdd"));
		ui->lineEdit_Address->setText(QString::fromLocal8Bit((char *)pIDCard->szAddress));
		//ui->lineEdit_GuardianMobile->setText(pSSCardInfo->strMobile.c_str());
		//ui->lineEdit_Address->setText(QString::fromLocal8Bit(pSSCardInfo->strAddress.c_str()));

		int nIndex = ui->comboBox_Nationality->findText(QString::fromLocal8Bit((char *)pIDCard->szName));
		if (nIndex != -1)
			ui->comboBox_Nationality->setCurrentIndex(nIndex);

		int nPersonType = atoi(szPersonType);
		ui->comboBox_PersonType->setCurrentIndex(nPersonType - 1);

		switch (nPersonType)
		{
		case 1:	// 城镇职业
		default:
			//{
			//	ui->lineEdit_Organization->setText(QString::fromLocal8Bit(pSSCardInfo->strCompanyName));
			//	//ui->lineEdit_OrganizationCode->setText(QString::fromLocal8Bit(pSSCardInfo->strLocalNum));
			//	ui->label_Organization->setText("单位名称");
			//	//ui->label_OrganizationCode->setText("单位编号");
			//	ui->label_Organization->show();
			//	ui->lineEdit_Organization->show();
			//	//ui->lineEdit_OrganizationCode->show();
			//	//ui->label_OrganizationCode->show();
			//	ui->label_Department->hide();
			//	ui->lineEdit_Department->hide();
			//	//ui->label_Class->hide();
			//	//ui->lineEdit_Class->hide();
			//}
			//break;
		case 2:	// 城镇居民
		case 3:	// 农村居民
		{
			ui->lineEdit_Organization->setText((char *)szCommunity);
			//ui->lineEdit_OrganizationCode->setText(QString::fromLocal8Bit(pSSCardInfo->strLocalNum));
			ui->label_Organization->setText("社区[村]");
			//ui->label_OrganizationCode->setText("社区(村)编号");
			ui->label_Organization->show();
			ui->lineEdit_Organization->show();
			//ui->lineEdit_OrganizationCode->show();
			//ui->label_OrganizationCode->show();
			ui->label_Department->hide();
			ui->lineEdit_Department->hide();
			//ui->label_Class->hide();
			//ui->lineEdit_Class->hide();
		}
		break;

		//{
		//	ui->label_Organization->hide();
		//	ui->lineEdit_Organization->hide();
		//	ui->label_OrganizationCode->hide();
		//	ui->lineEdit_OrganizationCode->hide();
		//	ui->label_Department->hide();
		//	ui->lineEdit_Department->hide();
		//	ui->label_Class->hide();
		//	ui->lineEdit_Class->hide();

		//}
		//break;
		case 4:	// 在读大学生
		{
			ui->lineEdit_Organization->setText((char *)szCompany);
			ui->lineEdit_Department->setText((char *)szDepartment);
			//ui->lineEdit_Class->setText(QString::fromLocal8Bit(pSSCardInfo->strClassName));

			ui->label_Organization->setText("学校名称");
			ui->label_Organization->show();
			ui->lineEdit_Organization->show();
			//ui->lineEdit_OrganizationCode->hide();
			//ui->label_OrganizationCode->hide();
			ui->label_Department->show();
			ui->lineEdit_Department->show();
			//ui->label_Class->show();
			//ui->lineEdit_Class->show();
		}
		break;
		}

		string strPhoto;
		if (json.KeyExist("Photo"))
		{
			json.Get("Photo", strPhoto);
			if (fs::exists((strPhoto)))
				g_pDataCenter->strSSCardPhotoFile = strPhoto;
			QString strQSS = QString("border-image: url(%1);").arg(g_pDataCenter->strSSCardPhotoFile.c_str());
			ui->label_Photo->setStyleSheet(strQSS);
			ui->label_Photo->setText("");
		}
		return true;
	}
	catch (std::exception& e)
	{
		gError() << "Catch a exception:" << e.what();
		return false;
	}
}

void Sys_SSCardAPITest::SavePersonInfo()
{
	SSCardInfoPtr& pSSCardInfo = g_pDataCenter->GetSSCardInfo();
	if (!pSSCardInfo)
		return;
	IDCardInfoPtr& pIDCard = g_pDataCenter->GetIDCardInfo();
	CJsonObject json;

	/*QString strProvince = ui->comboBox_Province->currentText();
	QString strCity = ui->comboBox_City->currentText();
	QString strCounty = ui->comboBox_County->currentText();
	QString strAddress = ui->lineEdit_Address->text();*/
	json.Add("Name", pSSCardInfo->strName);
	json.Add("Identity", pSSCardInfo->strCardID);
	json.Add("Gender", pSSCardInfo->strSex);
	json.Add("Birthday", pSSCardInfo->strBirthday);
	json.Add("Address", pSSCardInfo->strAddress);
	json.Add("PostCode", pSSCardInfo->strPostalCode);
	json.Add("Nation", (char*)pIDCard->szNationalty);
	json.Add("PaperIssuedate", (const char*)pIDCard->szExpirationDate1);
	json.Add("PaperExpiredate", (const char*)pIDCard->szExpirationDate2);
	json.Add("Mobile", pSSCardInfo->strMobile);
	json.Add("BankCode", pSSCardInfo->strBankCode);
	json.Add("City", pSSCardInfo->strCity);
	json.Add("PersonType", pSSCardInfo->strPersonType);
	json.Add("Company", pSSCardInfo->strCompanyName);
	json.Add("Community", pSSCardInfo->strCommunity);
	json.Add("LocalNum", pSSCardInfo->strLocalNum);
	json.Add("Department", pSSCardInfo->strDepartmentName);
	json.Add("Class", pSSCardInfo->strClassName);
	json.AddEmptySubObject("Progress");
	json["Progress"].Add("Registerlost", 0);
	json["Progress"].Add("InputMobile", 0);
	json["Progress"].Add("Premake", 0);
	json["Progress"].Add("Update", 0);
	json["Progress"].Add("Add", 0);
	json["Progress"].Add("Print", 0);

	/*json.Add("Province", strProvince.toLocal8Bit().data());
	json.Add("CityName", strCity.toLocal8Bit().data());
	json.Add("County", strCounty.toLocal8Bit().data());
	json.Add("Address", strAddress.toLocal8Bit().data());*/

	/*if (ui->checkBox_Agency->isChecked())
	{
		json.Add("Guardian", pSSCardInfo->strGuardianName);
		json.Add("GuardianShip", pSSCardInfo->strGuardianShip);
		json.Add("GuardianCardID", pSSCardInfo->strGuardianIDentity);
	}*/
	QFileInfo fi(g_pDataCenter->strSSCardPhotoFile.c_str());
	if (fi.isFile())
	{
		json.Add("Photo", g_pDataCenter->strSSCardPhotoFile);
	}
	try
	{
		QString strAppPath = QApplication::applicationDirPath();
		QString strJsonPath = QString("%1/data/Person_%2.json").arg(strAppPath).arg(pSSCardInfo->strCardID);

		ofstream ifs(strJsonPath.toLocal8Bit().data(), ios::out | ios::binary);
		ifs << json.ToFormattedString();
	}
	catch (std::exception& e)
	{
		gError() << "Catch a exception:" << e.what();
	}
}

bool Sys_SSCardAPITest::GetPersonName(QString strPersonFile, QString& strName)
{
	try
	{
		if (!fs::exists(strPersonFile.toLocal8Bit().data()))
			return false;

		ifstream ifs(strPersonFile.toLocal8Bit().data(), ios::in | ios::binary);
		stringstream ss;
		ss << ifs.rdbuf();
		CJsonObject json;
		if (!json.Parse(ss.str()))
			return false;

		string strTemp;
		if (!json.Get("Name", strTemp))
			return false;

		strName = QString::fromLocal8Bit(strTemp.c_str());
		return true;
	}
	catch (std::exception& e)
	{
		gError() << "Catch a exception:" << e.what();
		return false;
	}
}

bool Sys_SSCardAPITest::eventFilter(QObject* watched, QEvent* event)
{
	if (watched == ui->lineEdit_Name)
	{
		if (event->type() == QEvent::FocusIn || event->type() == QEvent::MouseButtonDblClick)
		{
			pCompleter->complete();
		}
	}
	return QWidget::eventFilter(watched, event);
}

void Sys_SSCardAPITest::ClearInfo()
{
	ui->lineEdit_Name->setText("");
	//ui->lineEdit_Mobile->setText("");
	ui->lineEdit_CardID->setText("");
	/*ui->lineEdit_Address->setText("");
	ui->lineEdit_GuardianMobile->setText("");*/
	ui->comboBox_Nationality->setCurrentIndex(0);
	ui->radioButton_Male->setChecked(false);
	ui->radioButton_Female->setChecked(false);
	ui->pushButton_OK->setEnabled(true);
	//ui->checkBox_Agency->setChecked(false);
	pIDCard = nullptr;
	pSSCardInfo = nullptr;
	ui->label_Photo->setStyleSheet("border-image: url();background-color: rgb(124, 124, 124);");
	ui->label_Photo->setText("照片");
}

void Sys_SSCardAPITest::ResetPage()
{
	gInfo() << __FUNCTION__;
	mapPersonFile.clear();
	ClearInfo();
}

#define sstrcpy(x,y)	strcpy_s(x,sizeof(x),y);
int	Sys_SSCardAPITest::GetSSCardInfo(/*IDCardInfoPtr &pIDCard,*/QString& strMessage)
{
	gInfo() << __FUNCTION__;
	RegionInfo& Reginfo = g_pDataCenter->GetSysConfigure()->Region;
	IDCardInfoPtr& pIDCard = g_pDataCenter->GetIDCardInfo();
	if (!pIDCard)
	{
		strMessage = "身份证信息无效!";
		return -1;
	}
	QString strMobile = ui->lineEdit_Mobile->text();
	if (strMobile.size() < 11)
	{
		strMessage = "手机号码输入有误,必须为11位数字!";
		return -1;
	}

	int nPersonType = ui->comboBox_PersonType->currentIndex() + 1;
	QString strOrganization = ui->lineEdit_Organization->text();
	//QString strOrganizationCode = ui->lineEdit_OrganizationCode->text();
	QString strDepartment = ui->lineEdit_Department->text();
	//QString strClass = ui->lineEdit_Class->text();
	QString strAddress = ui->lineEdit_Address->text();

	int nGender = pButtonGrp->checkedId() + 1;
	pSSCardInfo = make_shared<SSCardInfo>();
	switch (nPersonType)
	{
	case 1:	// 城镇职工
	default:
	{
		if (strOrganization.isEmpty() /*|| strOrganizationCode.isEmpty()*/)
		{
			strMessage = "城镇职工的单位名称不能为空!";
			return -1;
		}
		strcpy_s((char*)pSSCardInfo->strCompanyName, sizeof(pSSCardInfo->strCompanyName), strOrganization.toLocal8Bit().data());
		//strcpy_s((char*)pSSCardInfo->strLocalNum, sizeof(pSSCardInfo->strLocalNum), strOrganizationCode.toStdString().c_str());
	}
	break;
	case 2:	// 城镇居民
	case 3:	// 农村居民
	{
		if (strOrganization.isEmpty())
		{
			strMessage = "社区[村]不能为空!";
			return -1;
		}
		strcpy_s((char*)pSSCardInfo->strCommunity, sizeof(pSSCardInfo->strCommunity), strOrganization.toLocal8Bit().data());
		//strcpy_s((char*)pSSCardInfo->strLocalNum, sizeof(pSSCardInfo->strLocalNum), strOrganizationCode.toStdString().c_str());
	}
	break;
	case 4:	// 在读大学生
	{
		if (strOrganization.isEmpty() || strDepartment.isEmpty())
		{
			strMessage = "在读大学生院校名称、所在系不能为空!";
			return -1;
		}
		strcpy_s((char*)pSSCardInfo->strCompanyName, sizeof(pSSCardInfo->strCompanyName), strOrganization.toLocal8Bit().data());
		strcpy_s((char*)pSSCardInfo->strDepartmentName, sizeof(pSSCardInfo->strDepartmentName), strDepartment.toLocal8Bit().data());
		strcpy_s((char*)pSSCardInfo->strUniversity, sizeof(pSSCardInfo->strUniversity), "1");
		//strcpy_s((char*)pSSCardInfo->strClassName, sizeof(pSSCardInfo->strClassName), strClass.toLocal8Bit().data());
	}
	break;
	}

	itoa(nPersonType, pSSCardInfo->strPersonType, 10);
	strcpy_s((char*)pSSCardInfo->strName, sizeof(pSSCardInfo->strName), (const char*)pIDCard->szName);
	itoa(nGender, (char*)pSSCardInfo->strSex, 10);
	strcpy_s((char*)pSSCardInfo->strNation, sizeof(pSSCardInfo->strNation), (const char*)pIDCard->szNationaltyCode);
	strcpy_s((char*)pSSCardInfo->strBirthday, sizeof(pSSCardInfo->strBirthday), (const char*)pIDCard->szBirthday);
	strcpy_s((char*)pSSCardInfo->strCardID, sizeof(pSSCardInfo->strCardID), (const char*)pIDCard->szIdentity);
	strcpy_s((char*)pSSCardInfo->strAddress, sizeof(pSSCardInfo->strAddress), strAddress.toLocal8Bit().data());
	strcpy_s((char*)pSSCardInfo->strReleaseDate, sizeof(pIDCard->szExpirationDate1), (char*)pIDCard->szExpirationDate1);
	strcpy_s((char*)pSSCardInfo->strValidDate, sizeof(pIDCard->szExpirationDate2), (char*)pIDCard->szExpirationDate2);

	strcpy_s((char*)pSSCardInfo->strOrganID, sizeof(pSSCardInfo->strOrganID), Reginfo.strAgency.c_str());
	strcpy_s((char*)pSSCardInfo->strCity, sizeof(pSSCardInfo->strCity), Reginfo.strCityCode.c_str());
	strcpy_s((char*)pSSCardInfo->strSSQX, sizeof(pSSCardInfo->strSSQX), Reginfo.strCountry.c_str());
	strcpy_s((char*)pSSCardInfo->strCardVender, sizeof(pSSCardInfo->strCardVender), Reginfo.strCardVendor.c_str());
	strcpy_s((char*)pSSCardInfo->strBankCode, sizeof(pSSCardInfo->strBankCode), Reginfo.strBankCode.c_str());
	strcpy_s((char*)pSSCardInfo->strMobile, sizeof(pSSCardInfo->strMobile), strMobile.toStdString().c_str());
	strcpy_s((char*)pSSCardInfo->strPostalCode, sizeof(pSSCardInfo->strPostalCode), g_pDataCenter->GetSysConfigure()->Region.strPostCode.c_str());

	/*if (ui->checkBox_Agency->isChecked())
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
		pSSCardInfo->strGuardianName = strGuardian.toLocal8Bit().data();
		pSSCardInfo->strGuardianIDentity = strGuardianCardID.toStdString();
		pSSCardInfo->strGuardianShip = strGuandianShip.toLocal8Bit().data();
	}*/
	g_pDataCenter->strCardVersion = "3.00";

	g_pDataCenter->SetSSCardInfo(pSSCardInfo);
	return 0;
}

int Sys_SSCardAPITest::GetCardID(string& strCardID, QString strMessage)
{
	QString qstrCardID;
	do
	{
		qstrCardID = ui->lineEdit_CardID->text();
		if (qstrCardID.isEmpty())
		{
			strMessage = "请输入身份证号码!";
			break;
		}

		if (qstrCardID.size() < 18)
		{
			strMessage = "身份证号码位数不足18位，请检查后重新输入!";
			break;
		}
		char nX = VerifyCardID(qstrCardID.toStdString().c_str());
		if (nX != qstrCardID.toStdString().at(17))
		{
			strMessage = "身份证号码有误，请检查后重新输入!";
			break;
		}
		strCardID = qstrCardID.toStdString();
		return 0;
	} while (0);
	return -1;
}

int Sys_SSCardAPITest::GetCardInfo(/*IDCardInfoPtr& pIDCard,*/ QString& strMessage)
{
	gInfo() << __FUNCTION__;
	QString strName = ui->lineEdit_Name->text();
	QString strMobile = ui->lineEdit_Mobile->text();
	QString strCardID = ui->lineEdit_CardID->text();
	QString strNation = ui->comboBox_Nationality->currentText();
	QString strNationCode = ui->comboBox_Nationality->currentData().toString();
	QString strIssuedate = ui->dateEdit_Issued->dateTime().toString("yyyyMMdd");
	QString strExpireDate = ui->dateEdit_Expired->dateTime().toString("yyyyMMdd");
	//QString strAddress = ui->lineEdit_Address->text();
	//QString strProvince = ui->comboBox_Province->currentText();
	//QString strCity = ui->comboBox_City->currentText();
	//QString strCounty = ui->comboBox_County->currentText();
	/*QString strTown = ui->comboBox_Town->currentText();
	QString strStreet = ui->comboBox_Street->currentText();*/
	int nGender = pButtonGrp->checkedId();
	QString strGender = "";
	if (nGender == 0)
		strGender = "男";
	else if (nGender == 1)
		strGender = "女";

	if (strName.isEmpty() ||
		strCardID.isEmpty() ||
		strNation.isEmpty() ||
		strGender.isEmpty()
		/*||strAddress.isEmpty()*/)
	{
		strMessage = "姓名、民族、性别、身份证号码等信息不能为空!";
		return -1;
	}
	if (strName.contains(" "))
	{
		strMessage = "姓名中不能含有空格!";
		return -1;
	}
	if (strCardID.size() < 18)
	{
		strMessage = "身份证号码位数不足18位，请检查后重新输入!";
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
	strcpy_s((char*)pIDCard->szName, sizeof(pIDCard->szName), strName.toLocal8Bit().data());
	strcpy_s((char*)pIDCard->szIdentity, sizeof(pIDCard->szIdentity), strCardID.toLocal8Bit().data());
	strncpy_s((char*)pIDCard->szBirthday, sizeof(pIDCard->szBirthday), (char*)&pIDCard->szIdentity[6], 8);
	strcpy_s((char*)pIDCard->szGender, sizeof(pIDCard->szGender), strGender.toLocal8Bit().data());
	strcpy_s((char*)pIDCard->szNationalty, sizeof(pIDCard->szNationalty), strNation.toLocal8Bit().data());
	strcpy_s((char*)pIDCard->szNationaltyCode, sizeof(pIDCard->szNationaltyCode), strNationCode.toStdString().c_str());
	strcpy_s((char*)pIDCard->szExpirationDate1, sizeof(pIDCard->szExpirationDate1), strIssuedate.toLocal8Bit().data());
	strcpy_s((char*)pIDCard->szExpirationDate2, sizeof(pIDCard->szExpirationDate2), strExpireDate.toLocal8Bit().data());
	//QString strFullAddress = strProvince + strCity + strCounty /*+ strTown + strStreet */ + strAddress;
	//strcpy((char*)pIDCard->szAddress, strFullAddress.toLocal8Bit().data());
	g_pDataCenter->SetIDCardInfo(pIDCard);

	return 0;
}

void Sys_SSCardAPITest::on_AddNewIDCard(IDCardInfo* pIDCard)
{
	gInfo() << __FUNCTION__;
	shared_ptr<IDCardInfo> IDCardPtr(pIDCard);
	//ui->lineEdit_Guardian->setText(QString::fromLocal8Bit((char*)IDCardPtr->szName));
	//ui->lineEdit_GuardianCardID->setText(QString::fromLocal8Bit((char*)IDCardPtr->szIdentity));
	if (pDialogReadIDCard)
	{
		pDialogReadIDCard->accept();
	}
}

void Sys_SSCardAPITest::on_Name_textChanged(const QString& arg1)
{
	auto itfind = mapPersonFile.find(arg1);
	if (itfind != mapPersonFile.end())
	{
		disconnect(ui->lineEdit_Name, &QLineEdit::textChanged, this, &Sys_SSCardAPITest::on_Name_textChanged);
		ClearInfo();
		LoadPersonInfo(itfind->second);
		connect(ui->lineEdit_Name, &QLineEdit::textChanged, this, &Sys_SSCardAPITest::on_Name_textChanged);
	}
}


void Sys_SSCardAPITest::on_comboBox_PersonType_currentIndexChanged(int index)
{
	int nPersonType = index + 1;

	switch (nPersonType)
	{
	case 1:	// 城镇职工
	default:
	{
		ui->label_Organization->setText("单位名称");
		//ui->label_OrganizationCode->setText("单位编号");
		ui->label_Organization->show();
		ui->lineEdit_Organization->show();
		/*ui->lineEdit_OrganizationCode->show();
		ui->label_OrganizationCode->show();*/
		ui->label_Department->hide();
		ui->lineEdit_Department->hide();
		//ui->label_Class->hide();
		//ui->lineEdit_Class->hide();
	}
	break;
	case 2:	// 城镇居民
	case 3:	// 农村居民
	{
		ui->label_Organization->setText("社区[村]");
		ui->label_Organization->show();
		ui->lineEdit_Organization->show();
		//ui->label_OrganizationCode->setText("社区(村)编号");
		//ui->lineEdit_OrganizationCode->show();
		//ui->label_OrganizationCode->show();
		ui->label_Department->hide();
		ui->lineEdit_Department->hide();
		//ui->label_Class->hide();
		//ui->lineEdit_Class->hide();
	}
	break;

	//{
	//	ui->label_Organization->hide();
	//	ui->lineEdit_Organization->hide();
	//	ui->label_OrganizationCode->hide();
	//	ui->lineEdit_OrganizationCode->hide();
	//	ui->label_Department->hide();
	//	ui->lineEdit_Department->hide();
	//	ui->label_Class->hide();
	//	ui->lineEdit_Class->hide();

	//}
	//break;
	case 4:	// 在读大学生
	{
		ui->label_Organization->setText("学校名称");
		ui->label_Organization->show();
		ui->lineEdit_Organization->show();
		//ui->lineEdit_OrganizationCode->hide();
		//ui->label_OrganizationCode->hide();
		ui->label_Department->show();
		ui->lineEdit_Department->show();
		//ui->label_Class->show();
		//ui->lineEdit_Class->show();
	}
	break;
	}
}


void Sys_SSCardAPITest::on_pushButton_TakePhoto_clicked()
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
		SavePersonInfo();
		nResult = 0;
	} while (0);
	if (QFailed(nResult))
	{
		QMessageBox_CN(QMessageBox::Critical, "提示", strMessage, QMessageBox::Ok, this);
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


void Sys_SSCardAPITest::on_pushButton_SelectPhoto_clicked()
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

			QMessageBox_CN(QMessageBox::Critical, "提示", strMessage, QMessageBox::Ok, this);
			return;
		}
	}
	catch (std::exception& e)
	{
		QMessageBox_CN(QMessageBox::Critical, "提示", QString::fromLocal8Bit(e.what()), QMessageBox::Ok, this);

	}
}


void Sys_SSCardAPITest::on_pushButton_OK_clicked()
{
	gInfo() << __FUNCTION__;
	WaitCursor();
	QString strMessage;
	int nResult = -1;
	int nOperation = -1;
	int nPage = 0;
	MaskStatus nStatus = Success;
	int nCardStatus = -1;
	QString strTips = "操作成功";
	do
	{
		if (QFailed(GetCardInfo(strMessage)))
			break;

		if (QFailed(GetSSCardInfo(strMessage)))
			break;

		if (g_pDataCenter->strSSCardPhotoFile.empty())
		{
			strMessage = "尚未选取或采集个人照片!";
			break;
		}
		SavePersonInfo();

		SSCardInfoPtr pTempSSCardInfo = make_shared<SSCardInfo>();
		strcpy((char*)pTempSSCardInfo->strName, (const char*)pIDCard->szName);
		strcpy((char*)pTempSSCardInfo->strCardID, (const char*)pIDCard->szIdentity);
		QFile filePhoto(g_pDataCenter->strSSCardPhotoFile.c_str());
		if (!filePhoto.open(QIODevice::ReadOnly))
		{
			strTips = "操作失败";
			strMessage = QString("无法读取照片文件%1").arg(g_pDataCenter->strSSCardPhotoFile.c_str());
			break;
		}

		if (QFailed(QueryCardProgress(strMessage, nCardStatus, pTempSSCardInfo)))
		{
			break;
		}
		{
			QByteArray baPhoto;
			// 16岁以上，需要照片
			QFileInfo fi(g_pDataCenter->strSSCardPhotoFile.c_str());
			if (fi.isFile())
			{
				QFile qfile(g_pDataCenter->strSSCardPhotoFile.c_str());
				if (qfile.open(QIODevice::ReadOnly))
				{
					baPhoto = qfile.readAll().toBase64();
					pSSCardInfo->strPhoto = baPhoto.data();
				}
			}
			int nStatus = 0;
			//strcpy(pSSCardInfo->strDepartmentName, "");
			//strcpy(pSSCardInfo->strClassName, "");
			//strcpy(pSSCardInfo->strCommunity, "");
			//strcpy(pSSCardInfo->strCompanyName, "");
			//strcpy(pSSCardInfo->strLocalNum, "");
			nResult = ApplyNewCard(strMessage, nStatus, pSSCardInfo);			 // 申请 新制卡
		}

		g_pDataCenter->strCardMakeProgress = QString::fromLocal8Bit(pTempSSCardInfo->strCardStatus).toStdString();
		if (g_pDataCenter->strCardMakeProgress == "制卡中")
		{
			// 可以获取新的社保卡一些数据
			if (QFailed(nResult = g_pDataCenter->ReadSSCardInfo(pSSCardInfo, nCardStatus, strMessage)))
				break;
			if (nStatus != 0 && nStatus != 1)
				break;
			strTips = "操作成功";
			strMessage = "制卡信息已确认,现将转入制卡页面!";
			nOperation = Goto_Page;
			nPage = Page_MakeCard;
			nResult = 0;
			break;
		}
		else if (g_pDataCenter->strCardMakeProgress == "卡已领取")
		{
			nStatus = Failed;
			strTips = "操作失败";
			strMessage = "当前申请人卡片已领取,请走补换卡流程!";
			nOperation = Return_MainPage;
			//emit ShowMaskWidget(strTips, strMessage, nStatus, nOperation, 0);
			QMessageBox_CN(QMessageBox::Critical, "提示", strMessage, QMessageBox::Ok, this);
			return;
		}

		//char szOutInfo[1024] = { 0 };
		//applyNewCard(*pSSCardInfo, szOutInfo);

		nOperation = Goto_Page;
		nPage = Page_MakeCard;
		strMessage = "人员身份信息已经确认,现将转入制卡页面!";
		nResult = 0;
	} while (0);

	if (QFailed(nResult))
	{
		nStatus = Failed;
		nOperation = Stay_CurrentPage;
		strTips = "操作失败";
	}
	QMessageBox_CN(QMessageBox::Critical, "提示", strMessage, QMessageBox::Ok, this);
	//emit ShowMaskWidget(strTips, strMessage, nStatus, nOperation, nPage);
}

void Sys_SSCardAPITest::on_pushButton_ResetPage_clicked()
{
	ResetPage();
	if (g_pDataCenter->GetIDCardInfo())
	{
		IDCardInfoPtr& pIDCard = g_pDataCenter->GetIDCardInfo();
		ui->lineEdit_Name->setText(QString((char*)pIDCard->szName));
		ui->lineEdit_CardID->setText((char*)pIDCard->szIdentity);
		QString strGender = QString((char*)pIDCard->szGender);
		if (strGender == "男")
			ui->radioButton_Male->setChecked(true);
		else
			ui->radioButton_Female->setChecked(true);
		ui->lineEdit_Address->setText(QString((char*)pIDCard->szAddress));

		ui->dateEdit_Issued->setDateTime(QDateTime::fromString((char*)pIDCard->szExpirationDate1, "yyyyMMdd"));
		ui->dateEdit_Expired->setDateTime(QDateTime::fromString((char*)pIDCard->szExpirationDate2, "yyyyMMdd"));

		QString strNationalty = QString((char*)pIDCard->szNationalty) + "族";
		int nIndex = ui->comboBox_Nationality->findText(strNationalty);
		if (nIndex != -1)
			ui->comboBox_Nationality->setCurrentIndex(nIndex);
	}
	QString strAppPath = qApp->applicationDirPath();
	QString path = strAppPath + "/data";
	QDirIterator iter(path, QStringList() << "*.json", QDir::Files | QDir::NoSymLinks);
	QStringList strNameList;
	while (iter.hasNext())
	{
		iter.next();
		qDebug() << "fileName:" << iter.fileName();  // 只有文件名
		qDebug() << "filePath:" << iter.filePath();  //包含文件名的文件的全路径

		QString strAbsoluteFilePath = iter.filePath();
		QString strName;
		if (GetPersonName(strAbsoluteFilePath, strName))
		{
			strNameList << strName;
			auto [it, Inserted] = mapPersonFile.try_emplace(strName, strAbsoluteFilePath);
		}
	}
	pCompleter->setModel(new QStringListModel(strNameList, pCompleter));
}

void Sys_SSCardAPITest::on_pushButton_Excecute_clicked()
{
	int nSelect = ui->comboBox_Function->currentIndex();
	RegionInfo& Reginfo = g_pDataCenter->GetSysConfigure()->Region;
	Reginfo.strCityCode = ui->lineEdit_City->text().toStdString();
	Reginfo.strCountry = ui->lineEdit_Country->text().toStdString();
	Reginfo.strAgency = ui->lineEdit_Agency->text().toStdString();
	QString strMessage;
	
	if (!g_pDataCenter->GetSSCardInfo())
	{
		pSSCardInfo = make_shared<SSCardInfo>();
		g_pDataCenter->SetSSCardInfo(pSSCardInfo);
	}
	else
		pSSCardInfo = g_pDataCenter->GetSSCardInfo();
	if (!g_pDataCenter->GetIDCardInfo())
	{
		pIDCard = make_shared<IDCardInfo>();
		g_pDataCenter->SetIDCardInfo(pIDCard);
	}
	else
		pIDCard = g_pDataCenter->GetIDCardInfo();
	
	auto tpProgressInfo = FindCardProgress((const char*)pIDCard->szIdentity, pSSCardInfo);

	auto ProgressJson = g_pDataCenter->LoadSSCardData(std::get<1>(tpProgressInfo), pSSCardInfo);
	if (ProgressJson)
	{
		g_pDataCenter->SetSSCardInfo(pSSCardInfo);
		g_pDataCenter->SetProgress(tpProgressInfo);
	}
	else
	{
		gInfo() << "Failed in load SSCardData from " << ProgressJson.value();
		string strCardID = ui->lineEdit_CardID->text().toStdString();
		string strName = ui->lineEdit_Name->text().toLocal8Bit().data();
		string strNationality = ui->comboBox_Nationality->currentText().toLocal8Bit().data();
		string strMobile = ui->lineEdit_Mobile->text().toStdString();
		string strAddress = ui->lineEdit_Address->text().toStdString();

		int nGender = pButtonGrp->checkedId() + 1;

		string strIssuedDatetime = ui->dateEdit_Issued->dateTime().toString("yyyyMMdd").toStdString();
		string strExpiredDatetime = ui->dateEdit_Expired->dateTime().toString("yyyyMMdd").toStdString();
		
		if (QFailed(GetCardID(strCardID, strMessage)))
		{
			QMessageBox_CN(QMessageBox::Information, "提示", strMessage, QMessageBox::Ok);
			return;
		}
		strcpy((char*)pIDCard->szName,strName.c_str());
		strcpy((char*)pIDCard->szIdentity, strCardID.c_str());
		strcpy((char*)pIDCard->szAddress, strAddress.c_str());
		strcpy((char*)pIDCard->szExpirationDate1, strIssuedDatetime.c_str());
		strcpy((char*)pIDCard->szExpirationDate2, strExpiredDatetime.c_str());
		strcpy((char*)pIDCard->szNationalty, strNationality.c_str());
		
		if (nGender == 1)
			strcpy((char*)pIDCard->szGender, "男");
		else
			strcpy((char*)pIDCard->szGender, "女");
		
		strcpy((char*)pSSCardInfo->strName, strName.c_str());
		strcpy((char*)pSSCardInfo->strCardID, strCardID.c_str());
		strcpy((char*)pSSCardInfo->strMobile, strMobile.c_str());
		strcpy((char*)pSSCardInfo->strOrganID, Reginfo.strAgency.c_str());
		strcpy((char*)pSSCardInfo->strBankCode, Reginfo.strBankCode.c_str());
		strcpy((char*)pSSCardInfo->strTransType, "5");
		strcpy((char*)pSSCardInfo->strCity, Reginfo.strCityCode.c_str());
		strcpy((char*)pSSCardInfo->strSSQX, Reginfo.strCountry.c_str());
		strcpy((char*)pSSCardInfo->strCardVender, Reginfo.strCardVendor.c_str());
		strcpy((char*)pSSCardInfo->strBankCode, Reginfo.strBankCode.c_str());
		auto optProgressFile = g_pDataCenter->GetProgressFile(pIDCard);
		auto optProgressJson = g_pDataCenter->OpenProgress(optProgressFile.value());
		if (optProgressJson)
		{
			std::get<1>(tpProgressInfo) = optProgressFile.value();
			std::get<2>(tpProgressInfo) = optProgressJson.value();
			g_pDataCenter->SetProgress(tpProgressInfo);
		}
	}
		
	int nStatus = 0;
	int nResult = -1;
	QString strCardProgress;
	switch (nSelect)
	{
	case 0:		// 移除制卡数据
	default:
	{
		RemoveCardData(pSSCardInfo);
		break;
	}

	case 1:	// 照片采集
		on_pushButton_TakePhoto_clicked();
		break;
	case 2:	// 自选照片
		on_pushButton_SelectPhoto_clicked();
		break;
	case 4:	// 加载制止数据
	{
		//SSCardInfoPtr pSSCardInfo = make_shared<SSCardInfo>();
		QString strAppPath = QCoreApplication::applicationDirPath();
		strAppPath += "/Debug";
		QFileInfo fdir(strAppPath);
		if (fdir.exists())
		{
			if (!fdir.isDir())
			{
				QFile f(strAppPath);
				f.remove();
			}
		}
		else
		{
			QDir dir;
			dir.mkdir(strAppPath);
		}

		strAppPath += QString("/Progress_%1.json").arg((char *)pIDCard->szIdentity);
		QFileInfo ffile(strAppPath);
		if (ffile.isFile())
		{
			g_pDataCenter->LoadSSCardData(strAppPath.toStdString(),pSSCardInfo);
			ui->lineEdit_Name->setText(QString::fromLocal8Bit(pSSCardInfo->strName));
			g_pDataCenter->SetSSCardInfo(pSSCardInfo);
		}
		break;
	}
	case 3:	// 查询制卡状态
	{
		SSCardInfoPtr pTempSSCardInfo = make_shared<SSCardInfo>();
		strcpy((char*)pTempSSCardInfo->strName,(char *)pIDCard->szName);
		strcpy((char*)pTempSSCardInfo->strCardID, (char *)pIDCard->szIdentity);
		strcpy((char*)pSSCardInfo->strName, (char*)pIDCard->szName);
		strcpy((char*)pSSCardInfo->strCardID, (char*)pIDCard->szIdentity);
		if (QFailed(QueryCardProgress(strMessage, nStatus, pTempSSCardInfo)))
		{
			break;
		}

		g_pDataCenter->strCardMakeProgress = QString::fromLocal8Bit(pTempSSCardInfo->strCardStatus).toStdString();
		if (g_pDataCenter->nCardServiceType == ServiceType::Service_ReplaceCard &&
			g_pDataCenter->strCardMakeProgress == "制卡中")
		{
			// 可以获取新的社保卡一些数据
			if (QFailed(nResult = g_pDataCenter->ReadSSCardInfo(pSSCardInfo, nStatus, strMessage)))
				break;
			if (nStatus != 0 && nStatus != 1)
				break;
			nResult = 0;
			break;
		}
		ui->lineEdit_Status->setText(g_pDataCenter->strCardMakeProgress.c_str());
		// 可取旧社保卡号
		if (QFailed(nResult = g_pDataCenter->ReadSSCardInfo(pSSCardInfo, nStatus, strMessage)))
			break;

		if (nStatus != 0 && nStatus != 1)
			break;

		if (QFailed(nResult = g_pDataCenter->QuerySSCardStatus(pSSCardInfo, strMessage)))
			break;

		if (nStatus != 0)
		{
			break;
		}
		ui->lineEdit_Status->setText(pSSCardInfo->strCardStatus);
		break;
	}
	case 5:	// 挂失
	{
		if (g_pDataCenter->strCardMakeProgress == "制卡中")
		{
			QMessageBox_CN(QMessageBox::Information, "提示", "制卡中,无须挂失!",QMessageBox::Ok);
		}
		else
		{
			char szStatus[1024] = { 0 };
			int nResult = 0;
			if (QFailed(nResult = reportLostCard(*pSSCardInfo, szStatus)))
			{
				FailureMessage("挂失失败", pSSCardInfo, szStatus, strMessage);
				nStatus = strtol(szStatus, nullptr, 0);
				/*if (strcmp(szStatus, "08") == 0)
					strMessage = QString("挂失失败:人社服务器没有响应,可能网络异常或人社服务器故障\n姓名:%1\1卡号:%2\t").arg(pSSCardInfo->strName).arg(pSSCardInfo->strCardNum);
				else
					strMessage = QString("挂失失败:失败:%1\n姓名:%2\t卡号:%3\t").arg(nResult).arg((char*)pSSCardInfo->strName).arg((char*)pSSCardInfo->strCardNum);*/
				gError() << gQStr(strMessage);
			}

			gInfo() << "reportLostCard:" << szStatus;

			char szDigit[16] = { 0 }, szText[1024] = { 0 };
			SplitString(szStatus, szDigit, szText);
			if (strlen(szText))
			{
				strMessage = QString::fromLocal8Bit(szText);
				gInfo() << "挂失消息" << szText;
			}
			
			if (strlen(szDigit))
			{
				gInfo() << "挂失返回：" << szDigit;
			}
			else
			{
				gInfo() << "挂失无返回";
			}
		}
		break;
	}
    case 13:
    {
		char szStatus[1024] = { 0 };
		int nResult = 0;
		if (QFailed(nResult = cancelLostCard(*pSSCardInfo, szStatus)))
		{
			FailureMessage("解除挂失", pSSCardInfo, szStatus, strMessage);
			gError() << gQStr(strMessage);
			break;
		}

		gInfo() << "cancelLostCard:" << szStatus;
		char szDigit[16] = { 0 }, szText[1024] = { 0 };
		SplitString(szStatus, szDigit, szText);
		if (strlen(szText))
			strMessage = QString::fromLocal8Bit(szText);
		if (strlen(szDigit))
		{
			nStatus = strtolong(szDigit, 10);
			break;
		}
		QMessageBox_CN(QMessageBox::Information, "提示", strMessage, QMessageBox::Ok);
		
        break;
    }
	case 6:// 注册缴费
	{
		nResult = 0;// ResgisterPayment(strMessage, nStatus, g_pDataCenter->GetSSCardInfo());          // 缴费登记
		if (QFailed(nResult))
			strMessage = QString("缴费登记失败,Result:%1!").arg(nResult);
		else if (nStatus != 0 && nStatus != 1)
			strMessage = QString("缴费登记失败,Status:%1!").arg(nStatus);
		QMessageBox_CN(QMessageBox::Information, "提示", strMessage, QMessageBox::Ok);
		break;
	}
			
	case 7:// 即制卡标注
	{
		int nStatus = 0;
		int nResult = 0;
		SSCardInfoPtr& pSSCardInfo = g_pDataCenter->GetSSCardInfo();
		QString strCardProgress = QString::fromLocal8Bit(pSSCardInfo->strCardStatus);

		if (QFailed(nResult = MarkCard(strMessage, nStatus, pSSCardInfo)))
			QMessageBox_CN(QMessageBox::Information, "提示", strMessage, QMessageBox::Ok);
		break;
	}
	
	case 8:
	{
		nResult = ApplyCardReplacement(strMessage, nStatus, pSSCardInfo);     //  申请补换卡
		if (QFailed(nResult))
		{
			QMessageBox_CN(QMessageBox::Information, "提示", strMessage, QMessageBox::Ok);
		}
		break;
	}

	case 9:// 获取制卡数据
	{
		if (QFailed(nResult = GetCardData(strMessage, nStatus, pSSCardInfo)))
		{
			if (strMessage.contains("已经有批次号"))
			{
				QString strText = strMessage;
				strText += "\n请转到后台以写卡失败选项手动制卡!";
				strMessage = strText;
				break;
			}
			if (strMessage.contains("批次"))
			{
				strMessage += ",需社保局后台更新数据,请在2小时后再尝试制卡!";
			}
			
		}

		if (nStatus != 0 && nStatus != 1)
		{
			strMessage = QString("GetCardData Status = %d,should be 1 or 0.").arg(nStatus);
			break;
		}
		QMessageBox_CN(QMessageBox::Information, "提示", strMessage, QMessageBox::Ok);
		break;
	}
	case 10:	// 写卡
	{
		if (!g_pDataCenter->GetSSCardReader())
		{
			if (QFailed(g_pDataCenter->OpenSSCardReader(strMessage)))
			{
				gError() << gQStr(strMessage);
				break;
			}
		}
		if (QFailed(g_pDataCenter->SafeWriteCard(strMessage)))
		{
			strMessage = QString("%1,稍后,您可以点击重试按钮或更换卡片后继续制卡!");
			gError() << gQStr(strMessage);
			break;
		}
		QMessageBox_CN(QMessageBox::Critical, "提示", strMessage, QMessageBox::Ok);
		break;
	}
	case 11: // 打印
	{

	}
	case 12:// 回盘启用
	{
		QString strMessage;
		int nResult = -1;
		int nStatus = -1;
		SSCardInfoPtr& pSSCardInfo = g_pDataCenter->GetSSCardInfo();
		QString strName = ui->lineEdit_Name->text();
		strcpy(pSSCardInfo->strName, strName.toLocal8Bit().data());
		if (QFailed(nResult = ReturnCardData(strMessage, nStatus, pSSCardInfo, false)))
		{
			QMessageBox_CN(QMessageBox::Critical, "提示", strMessage, QMessageBox::Ok);
			break;
		}

		if (nStatus != 0 && nStatus != 1)
		{
			QMessageBox_CN(QMessageBox::Critical, "提示", "回盘失败", QMessageBox::Ok);
			break;
		}

		// 启用
		if (QFailed(nResult = EnalbeCard(strMessage, nStatus, pSSCardInfo)))
		{
			gError() << strMessage.toLocal8Bit().data();
			QMessageBox_CN(QMessageBox::Critical, "提示", strMessage, QMessageBox::Ok);
			break;
		}
		QMessageBox_CN(QMessageBox::Critical, "提示", "操作成功", QMessageBox::Ok);
		break;
	}
	}
}
