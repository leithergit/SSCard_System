﻿#include "uc_inputidcardinfo.h"
#include "ui_uc_inputidcardinfo.h"
#include "Gloabal.h"
//#include "qnationwidget.h"
#include "uc_readidcard.h"
#include "Payment.h"
#include "sys_dialogreadidcard.h"
#include "mainwindow.h"
#include "Sys_dialogcameratest.h"

#include <ctime>
#include <QStringListModel>


uc_InputIDCardInfo::uc_InputIDCardInfo(QLabel* pTitle, QString strStepImage, Page_Index nIndex, QWidget* parent) :
	QStackPage(pTitle, strStepImage, nIndex, parent),
	ui(new Ui::uc_InputIDCardInfo)
{
	ui->setupUi(this);

	//ShowGuardianWidget(false);
	ui->lineEdit_CardID->setValidator(new QRegExpValidator(QRegExp("^[1-9]\\d{5}(18|19|([23]\\d))\\d{2}((0[1-9])|(10|11|12))(([0-2][1-9])|10|20|30|31)\\d{3}[0-9Xx]$")));   //只能输入数字
	//ui->lineEdit_PostCode->setValidator(new QRegExpValidator(QRegExp("\\d{6}")));
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
		SSCardInfoPtr pSSCardInfo = make_shared<SSCardInfo>();
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
	InitializeDB(strMessage);
	connect(this, &uc_InputIDCardInfo::AddNewIDCard, this, &uc_InputIDCardInfo::on_AddNewIDCard, Qt::QueuedConnection);
	qDebug() << geometry();
	ui->label_Department->hide();
	ui->lineEdit_Department->hide();
	//ui->label_Class->hide();
	//ui->lineEdit_Class->hide();
	ui->lineEdit_Name->setCompleter(pCompleter);
	connect(ui->lineEdit_Name, &QLineEdit::textChanged, this, &uc_InputIDCardInfo::on_Name_textChanged);
}

bool  uc_InputIDCardInfo::InitializeDB(QString& strMessage)
{
	return true;
}

uc_InputIDCardInfo::~uc_InputIDCardInfo()
{
	delete ui;
}

bool uc_InputIDCardInfo::LoadPersonInfo(QString strJson)
{
	try
	{
		if (!fs::exists(strJson.toLocal8Bit().data()))
		{
			gError() << "Failed in locating file " << strJson.toLocal8Bit().data();
			return false;
		}

		ifstream ifs(strJson.toLocal8Bit().data(), ios::in | ios::binary);
		stringstream ss;
		ss << ifs.rdbuf();
		CJsonObjectPtr pJson = make_shared<CJsonObject>();
		if (!pJson->Parse(ss.str()))
		{
			gError() << "Failed in parse file " << strJson.toLocal8Bit().data();
			return false;
		}
			

		SSCardInfoPtr pSSCardInfo = make_shared<SSCardInfo>();
		char szNation[64] = { 0 };

		GetJsonPtrValue(pJson, "Name", pSSCardInfo->strName);
		GetJsonPtrValue(pJson, "Identity", pSSCardInfo->strCardID);
		GetJsonPtrValue(pJson, "Gender", pSSCardInfo->strSex);
		GetJsonPtrValue(pJson, "Birthday", pSSCardInfo->strBirthday);
		GetJsonPtrValue(pJson, "Nation", szNation);
		GetJsonPtrValue(pJson, "PaperIssuedate", pSSCardInfo->strReleaseDate);
		GetJsonPtrValue(pJson, "PaperExpiredate", pSSCardInfo->strValidDate);
		GetJsonPtrValue(pJson, "Mobile", pSSCardInfo->strMobile);
		GetJsonPtrValue(pJson, "Address", pSSCardInfo->strAddress);
		GetJsonPtrValue(pJson, "PostCode", pSSCardInfo->strPostalCode);
		GetJsonPtrValue(pJson, "BankCode", pSSCardInfo->strBankCode);
		GetJsonPtrValue(pJson, "City", pSSCardInfo->strCity);
				   
		GetJsonPtrValue(pJson, "PersonType", pSSCardInfo->strPersonType);
		GetJsonPtrValue(pJson, "Company", pSSCardInfo->strCompanyName);
		GetJsonPtrValue(pJson, "Community", pSSCardInfo->strCommunity);
		GetJsonPtrValue(pJson, "LocalNum", pSSCardInfo->strLocalNum);
		GetJsonPtrValue(pJson, "Department", pSSCardInfo->strDepartmentName);
		GetJsonPtrValue(pJson, "Class", pSSCardInfo->strClassName);


		ui->lineEdit_Name->setText(QString::fromLocal8Bit(pSSCardInfo->strName));
		ui->lineEdit_CardID->setText(pSSCardInfo->strCardID);
		ui->lineEdit_Mobile->setText(pSSCardInfo->strMobile);

		int nGender = atoi(pSSCardInfo->strSex);
		if (nGender == 1)
			ui->radioButton_Male->setChecked(true);
		else if (nGender == 2)
			ui->radioButton_Female->setChecked(true);

		ui->dateEdit_Issued->setDateTime(QDateTime::fromString(pSSCardInfo->strReleaseDate, "yyyyMMdd"));
		ui->dateEdit_Expired->setDateTime(QDateTime::fromString(pSSCardInfo->strValidDate, "yyyyMMdd"));
		ui->lineEdit_Address->setText(QString::fromLocal8Bit(pSSCardInfo->strAddress));

		int nIndex = ui->comboBox_Nationality->findText(QString::fromLocal8Bit(szNation));
		if (nIndex != -1)
			ui->comboBox_Nationality->setCurrentIndex(nIndex);

		int nPersonType = atoi(pSSCardInfo->strPersonType);
		ui->comboBox_PersonType->setCurrentIndex(nPersonType - 1);

		switch (nPersonType)
		{
		case 1:	// 城镇职业
		default:
			ui->lineEdit_Organization->setText(QString::fromLocal8Bit(pSSCardInfo->strCompanyName));
			ui->label_Organization->setText("单位名称");
			ui->label_Organization->show();
			ui->lineEdit_Organization->show();
			break;
		case 2:	// 城镇居民
		case 3:	// 农村居民
		{
			ui->lineEdit_Organization->setText(QString::fromLocal8Bit(pSSCardInfo->strCommunity));
			ui->label_Organization->setText("社区[村]");
			ui->label_Organization->show();
			ui->lineEdit_Organization->show();
			ui->label_Department->hide();
			ui->lineEdit_Department->hide();
		}
		break;

		case 4:	// 在读大学生
		{
			ui->lineEdit_Organization->setText(QString::fromLocal8Bit(pSSCardInfo->strCompanyName));
			ui->lineEdit_Department->setText(QString::fromLocal8Bit(pSSCardInfo->strDepartmentName));
			ui->label_Organization->setText("学校名称");
			ui->label_Organization->show();
			ui->lineEdit_Organization->show();
			ui->label_Department->show();
			ui->lineEdit_Department->show();
		}
		break;
		}

		string strPhoto;
		if (pJson->KeyExist("Photo"))
		{
			pJson->Get("Photo", strPhoto);
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

void uc_InputIDCardInfo::SavePersonInfo()
{
	SSCardInfoPtr& pSSCardInfo = g_pDataCenter->GetSSCardInfo();
	if (!pSSCardInfo)
		return;
	IDCardInfoPtr& pIDCard = g_pDataCenter->GetIDCardInfo();
	string strMessage = "";
	CJsonObject json;
	
	try
	{
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

		QFileInfo fi(g_pDataCenter->strSSCardPhotoFile.c_str());
		if (fi.isFile())
		{
			json.Add("Photo", g_pDataCenter->strSSCardPhotoFile);
		}
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

int uc_InputIDCardInfo::ProcessBussiness()
{
	MainWindow* pMainWind = (MainWindow*)qApp->activeWindow();
	if (pMainWind && pMainWind->pLastStackPage)
	{
		pMainWind->pLastStackPage->ResetTimer(true, this);
	}
	ResetPage();
	
	QString strAppPath = qApp->applicationDirPath();
	QString path = strAppPath + "/data";
	QDirIterator iter(path, QStringList() << "Person*.json", QDir::Files | QDir::NoSymLinks);
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
	if (g_pDataCenter->GetIDCardInfo())
	{
		IDCardInfoPtr& pIDCard = g_pDataCenter->GetIDCardInfo();
		ui->lineEdit_Name->setText(QString::fromLocal8Bit((char*)pIDCard->szName));
		ui->lineEdit_CardID->setText((char*)pIDCard->szIdentity);
		QString strGender = QString::fromLocal8Bit((char*)pIDCard->szGender);
		if (strGender == "男")
			ui->radioButton_Male->setChecked(true);
		else
			ui->radioButton_Female->setChecked(true);
		ui->lineEdit_Address->setText(QString::fromLocal8Bit((char*)pIDCard->szAddress));

		ui->dateEdit_Issued->setDateTime(QDateTime::fromString((char*)pIDCard->szExpirationDate1, "yyyyMMdd"));
		ui->dateEdit_Expired->setDateTime(QDateTime::fromString((char*)pIDCard->szExpirationDate2, "yyyyMMdd"));

		QString strNationalty = QString::fromLocal8Bit((char*)pIDCard->szNationalty) + "族";
		int nIndex = ui->comboBox_Nationality->findText(strNationalty);
		if (nIndex != -1)
			ui->comboBox_Nationality->setCurrentIndex(nIndex);
	}
	
	//LoadPersonInfo();
	return 0;
}

bool uc_InputIDCardInfo::GetPersonName(QString strPersonFile, QString& strName)
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

bool uc_InputIDCardInfo::eventFilter(QObject* watched, QEvent* event)
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

void uc_InputIDCardInfo::ClearInfo()
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

void uc_InputIDCardInfo::ResetPage()
{
	gInfo() << __FUNCTION__;
	mapPersonFile.clear();
	ClearInfo();
}

#define sstrcpy(x,y)	strcpy_s(x,sizeof(x),y);
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
		if (strOrganization.toLocal8Bit().size() > 100)
		{
			strMessage = "“单位名称”字数不能超过50个汉字!";
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
			strMessage = "“社区[村]”不能为空!";
			return -1;
		}
		if (strOrganization.toLocal8Bit().size() > 20)
		{
			strMessage = "“社区[村]”字数不能超过10个汉字!";
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
			strMessage = "在读大学生院校名称、所在院系不能为空!";
			return -1;
		}
		if (strOrganization.toLocal8Bit().size() > 100)
		{
			strMessage = "“学校名称”字数不能超过50个汉字!";
			return -1;
		}
		if (strDepartment.toLocal8Bit().size() > 100)
		{
			strMessage = "“所在院系”字数不能超过25个汉字!";
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
	strcpy_s((char*)pSSCardInfo->strReleaseDate, sizeof(pSSCardInfo->strReleaseDate), (char*)pIDCard->szExpirationDate1);
#ifdef HN2022
	strcpy_s((char*)pSSCardInfo->strIDCardIssuedDate, sizeof(pIDCard->szExpirationDate1), (char*)pIDCard->szExpirationDate1);
#endif
	strcpy_s((char*)pSSCardInfo->strValidDate, sizeof(pSSCardInfo->strValidDate), (char*)pIDCard->szExpirationDate2);

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

int uc_InputIDCardInfo::GetCardInfo(/*IDCardInfoPtr& pIDCard,*/ QString& strMessage)
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
	//else
	//{
	//	strIssuedate = "";
	//	strExpireDate = "";
	//}

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

void uc_InputIDCardInfo::on_AddNewIDCard(IDCardInfo* pIDCard)
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
			emit ShowMaskWidget("严重错误", strError, Fatal, Return_MainPage);
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
		SavePersonInfo();
		nResult = 0;
	} while (0);
	if (QFailed(nResult))
	{
		//QMessageBox_CN(QMessageBox::Critical, "提示", strMessage, QMessageBox::Ok, this);
		emit ShowMaskWidget("提示", strMessage, Fatal, Stay_CurrentPage);
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
			emit ShowMaskWidget("提示", strMessage, Fatal, Stay_CurrentPage);
			//QMessageBox_CN(QMessageBox::Critical, "提示", strMessage, QMessageBox::Ok, this);
			return;
		}
	}
	catch (std::exception& e)
	{
		//QMessageBox_CN(QMessageBox::Critical, "提示", QString::fromLocal8Bit(e.what()), QMessageBox::Ok, this);
		emit ShowMaskWidget("提示", QString::fromLocal8Bit(e.what()), Fatal, Stay_CurrentPage);
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
	int nCardStatus = -1;
	QString strTips = "操作成功";
	MaskStatus nStatus = Success;
	
	do
	{
		if (QFailed(GetCardInfo(strMessage)))
			break;

		if (QFailed(GetSSCardInfo(strMessage)))
			break;
		if (g_pDataCenter->strSSCardPhotoFile.empty())
		{
			strMessage = "尚未选取或采集制卡人照片!";
			break;
		}
		SavePersonInfo();
			
		SSCardInfoPtr pSSCardInfoLocal = nullptr;
		auto tpProgressInfo = FindCardProgress((const char*)pIDCard->szIdentity, pSSCardInfoLocal);
		switch (std::get<0>(tpProgressInfo))
		{
		default:
		case ProgrerssType::Progress_UnStart:
		{
			QFile filePhoto(g_pDataCenter->strSSCardPhotoFile.c_str());
			if (!filePhoto.open(QIODevice::ReadOnly))
			{
				strTips = "操作失败";
				strMessage = QString("无法读取照片文件%1").arg(g_pDataCenter->strSSCardPhotoFile.c_str());
				break;
			}
			// 创建新的流程
			auto optProgressFile = g_pDataCenter->GetProgressFile(pIDCard);
			auto optProgressJson = g_pDataCenter->OpenProgress(optProgressFile.value());
			if (optProgressJson)
			{
				std::get<1>(tpProgressInfo) = optProgressFile.value();
				std::get<2>(tpProgressInfo) = optProgressJson.value();
				g_pDataCenter->SetProgress(tpProgressInfo);
			}

			if (QFailed(QueryCardProgress(strMessage, nCardStatus, pSSCardInfo)))
			{
				nStatus = Failed;
				break;
			}

			if (!pSSCardInfo->strPhoto)
			{
				if (g_pDataCenter->strPhotoBase64.size())
					pSSCardInfo->strPhoto = (char*)g_pDataCenter->strPhotoBase64.c_str();
				else
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
							g_pDataCenter->strPhotoBase64 = baPhoto.data();
							pSSCardInfo->strPhoto = (char*)g_pDataCenter->strPhotoBase64.c_str();
						}
					}
				}
			}
			int nStatus = 0;
			nResult = ApplyNewCard(strMessage, nStatus, pSSCardInfo);			 // 申请 新制卡

			g_pDataCenter->strCardMakeProgress = QString::fromLocal8Bit(pSSCardInfo->strCardStatus).toStdString();
			if (g_pDataCenter->strCardMakeProgress == "制卡中")
			{
				// 可以获取新的社保卡一些数据
				if (QFailed(nResult = g_pDataCenter->ReadSSCardInfo(pSSCardInfo, nCardStatus, strMessage)))
				{
					nStatus = Failed;
					break;
				}
					
				if (nStatus != 0 && nStatus != 1)
					break;
				strTips = "操作成功";
				strMessage = "制卡信息已确认,现将转入制卡页面!";
				nOperation = Goto_Page;
				nPage = Page_MakeCard;
				g_pDataCenter->SetProgressStatus("EnsureInformation", 1);
				nResult = 0;
				break;
			}
			else if (g_pDataCenter->strCardMakeProgress == "卡已领取")
			{
				nStatus = Failed;
				strTips = "操作失败";
				strMessage = "当前申请人卡片已领取,请走补换卡流程!";
				nOperation = Return_MainPage;
				emit ShowMaskWidget(strTips, strMessage, nStatus, nOperation, 0);
				return;
			}
		}
			break;
		case ProgrerssType::Progress_Making:
		{
			strMessage = "发现尚未完成的制卡数据,现将转入制卡页面继续制卡!";
			g_pDataCenter->SetSSCardInfo(pSSCardInfoLocal);
			g_pDataCenter->SetProgress(tpProgressInfo);
			nOperation = Goto_Page;
			nPage = Page_MakeCard;		
			nResult = 0;
			break;
		}
		case ProgrerssType::Progress_Finished:
		{
			if (QMessageBox_CN(QMessageBox::Question, "询问", "系统中存在一个已经完成的制卡进度文件，是否需使用该进度?如果是仅打印卡面选择是!", QMessageBox::Yes | QMessageBox::No) == QMessageBox::Yes)
			{
				strMessage = "发现已经完成的制卡数据,现将转入制卡页面,你可以进行相关的操作!";
				g_pDataCenter->SetSSCardInfo(pSSCardInfoLocal);
				g_pDataCenter->SetProgress(tpProgressInfo);
				nOperation = Goto_Page;
				nPage = Page_MakeCard;
				nResult = 0;
			}
			else
			{
				auto optProgressFile = g_pDataCenter->GetProgressFile(pIDCard);
				auto optProgressJson = g_pDataCenter->OpenProgress(optProgressFile.value());
				if (optProgressJson)
				{
					std::get<1>(tpProgressInfo) = optProgressFile.value();
					std::get<2>(tpProgressInfo) = optProgressJson.value();
					g_pDataCenter->SetProgress(tpProgressInfo);
				}
			}
			
			break;
		}
		}

		
	} while (0);

	if (QFailed(nResult))
	{
		nStatus = Failed;
		nOperation = Stay_CurrentPage;
		strTips = "操作失败";
	}

	emit ShowMaskWidget(strTips, strMessage, nStatus, nOperation, nPage);
}

void uc_InputIDCardInfo::on_comboBox_PersonType_currentIndexChanged(int index)
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

void uc_InputIDCardInfo::on_Name_textChanged(const QString& arg1)
{
	auto itfind = mapPersonFile.find(arg1);
	if (itfind != mapPersonFile.end())
	{
		disconnect(ui->lineEdit_Name, &QLineEdit::textChanged, this, &uc_InputIDCardInfo::on_Name_textChanged);
		ClearInfo();
		LoadPersonInfo(itfind->second);
		connect(ui->lineEdit_Name, &QLineEdit::textChanged, this, &uc_InputIDCardInfo::on_Name_textChanged);
	}
}

//void uc_InputIDCardInfo::on_lineEdit_Name_textEdited(const QString& arg1)
//{
//	
//}

void uc_InputIDCardInfo::on_lineEdit_CardID_textChanged(const QString & strCardID)
{
	
}
