#include "uc_inputidcardinfo.h"
#include "ui_uc_inputidcardinfo.h"
#include "Gloabal.h"
//#include "qnationwidget.h"
#include "uc_readidcard.h"
#include "Payment.h"
#include "sys_dialogreadidcard.h"
#include "mainwindow.h"
#include "Sys_dialogcameratest.h"
#include "waitingprogress.h"

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

uc_InputIDCardInfo::uc_InputIDCardInfo(QLabel* pTitle, QString strStepImage, Page_Index nIndex, QWidget* parent) :
	QStackPage(pTitle, strStepImage, nIndex, parent),
	ui(new Ui::uc_InputIDCardInfo)
{
	ui->setupUi(this);

	ShowGuardianWidget(false);
	ui->pushButton_GetCardID->setVisible(false);
	ui->lineEdit_CardID->setValidator(new QRegExpValidator(QRegExp("^[1-9]\\d{5}(18|19|([23]\\d))\\d{2}((0[1-9])|(10|11|12))(([0-2][1-9])|10|20|30|31)\\d{3}[0-9Xx]$")));   //只能输入数字
	ui->lineEdit_Name->setValidator(new QRegExpValidator(QRegExp("[\u4e00-\u9fa5]{2,10}")));

	ui->lineEdit_GuardianMobile->setValidator(new QRegExpValidator(QRegExp("0?(13|15|16|17|18|19)[0-9]{9}")));   //只能输入数字
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

	ui->lineEdit_Name->installEventFilter(this);
	ui->lineEdit_Name->setFocusPolicy(Qt::ClickFocus);
	pCompleter = new QCompleter(this);
	if (pCompleter)
	{
		QAbstractItemView* popup = pCompleter->popup();
		if (popup)
			popup->setStyleSheet("font-weight:normal;line-height:49px;letter-spacing:1px;color:#707070;font:42px \"思源黑体 CN Medium\";");
	}
	ui->lineEdit_Name->setCompleter(pCompleter);
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
	/*ui->comboBox_Town->setStyleSheet(strQSS);
	ui->comboBox_Street->setStyleSheet(strQSS);*/
	ui->comboBox_Guardianship->setStyleSheet(strQSS);
	ui->comboBox_Nationality->setStyleSheet(strQSS);
	ui->comboBox_Career->setStyleSheet(strQSS);
	//InitializeDB(strMessage);
	connect(this, &uc_InputIDCardInfo::AddNewIDCard, this, &uc_InputIDCardInfo::on_AddNewIDCard, Qt::QueuedConnection);
	connect(this, &uc_InputIDCardInfo::CheckPersonInfo, this, &uc_InputIDCardInfo::on_CheckPersonInfo);
	//connect(ui->lineEdit_Name, &QLineEdit::editingFinished, this, &uc_InputIDCardInfo::on_Name_EditingFinished);
	connect(ui->lineEdit_Name, &QLineEdit::textChanged, this, &uc_InputIDCardInfo::on_Name_textChanged);
}

/*
bool  uc_InputIDCardInfo::InitializeDB(QString& strMessage)
{
	QSqlQuery query = g_pDataCenter->SQLiteDB.exec("SELECT ID,Name FROM Area_2022 as t where t.LevelType=1 ORDER BY ID");
	while (query.next())
	{
		QString strID = query.value("ID").toString();
		QString strProvince = query.value("Name").toString();
		ui->comboBox_Province->addItem(strProvince, strID);
	}
	bInitialized = true;

	QString strCity = g_pDataCenter->GetSysConfigure()->Region.strCityCode.c_str();
	QString strQuery = QString("SELECT t.ID,t.Province,t.Name from Area_2022 as t where t.id=%1").arg(strCity);

	query = g_pDataCenter->SQLiteDB.exec(strQuery);
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
*/

uc_InputIDCardInfo::~uc_InputIDCardInfo()
{
	delete ui;
}

bool uc_InputIDCardInfo::LoadPersonInfo(QString strJson)
{
	try
	{
		if (!fs::exists(strJson.toLocal8Bit().data()))
			return false;

		ifstream ifs(strJson.toLocal8Bit().data(), ios::in | ios::binary);
		stringstream ss;
		ss << ifs.rdbuf();
		CJsonObject json;
		if (!json.Parse(ss.str()))
			return false;

		SSCardBaseInfoPtr pSSCardInfo = make_shared<SSCardBaseInfo>();
		json.Get("Name", pSSCardInfo->strName);
		json.Get("Identity", pSSCardInfo->strIdentity);
		json.Get("Gender", pSSCardInfo->strGender);
		json.Get("Birthday", pSSCardInfo->strBirthday);
		json.Get("Nantion", pSSCardInfo->strNation);
		json.Get("PaperIssuedate", pSSCardInfo->strReleaseDate);
		json.Get("PaperExpiredate", pSSCardInfo->strValidDate);
		json.Get("Mobile", pSSCardInfo->strMobile);
		json.Get("Address", pSSCardInfo->strAddress);
		json.Get("BankCode", pSSCardInfo->strBankCode);
		json.Get("City", pSSCardInfo->strCity);

		string strAddress;
		//string strProvince, strCity, strCounty;
		//json.Get("Province", strProvince);
		//json.Get("CityName", strCity);
		//json.Get("County", strCounty);
		//json.Get("Address", strAddress);
		//int nProvince = ui->comboBox_Province->findText(QString::fromLocal8Bit(strProvince.c_str()));
		//if (nProvince >= 0)
		//{
		//	ui->comboBox_Province->setCurrentIndex(nProvince);
		//	on_comboBox_Province_currentIndexChanged(nProvince);
		//	int nCity = ui->comboBox_City->findText(QString::fromLocal8Bit(strCity.c_str()));
		//	if (nCity >= 0)
		//	{
		//		ui->comboBox_City->setCurrentIndex(nCity);
		//		on_comboBox_City_currentIndexChanged(nCity);
		//		int nCounty = ui->comboBox_County->findText(QString::fromLocal8Bit(strCounty.c_str()));
		//		if (nCounty >= 0)
		//			ui->comboBox_County->setCurrentIndex(nCounty);
		//	}
		//}


		ui->lineEdit_Name->setText(QString::fromLocal8Bit(pSSCardInfo->strName.c_str()));
		ui->lineEdit_CardID->setText(pSSCardInfo->strIdentity.c_str());

		QString strGender = QString::fromLocal8Bit(pSSCardInfo->strGender.c_str());
		if (strGender == "男")
			ui->radioButton_Male->setChecked(true);
		else if (strGender == "女")
			ui->radioButton_Female->setChecked(true);

		ui->dateEdit_Issued->setDateTime(QDateTime::fromString(pSSCardInfo->strReleaseDate.c_str(), "yyyyMMdd"));
		ui->dateEdit_Expired->setDateTime(QDateTime::fromString(pSSCardInfo->strValidDate.c_str(), "yyyyMMdd"));
		ui->lineEdit_GuardianMobile->setText(pSSCardInfo->strMobile.c_str());
		ui->plainTextEdit_Address->setPlainText(QString::fromLocal8Bit(pSSCardInfo->strAddress.c_str()));
		if (strAddress.size())
			ui->plainTextEdit_Address->setPlainText(strAddress.c_str());
		QString strNation = QString::fromLocal8Bit(pSSCardInfo->strNation.c_str());
		int nIndex = ui->comboBox_Nationality->findText(strNation);
		if (nIndex != -1)
			ui->comboBox_Nationality->setCurrentIndex(nIndex);

		int nAge = GetAge(pSSCardInfo->strBirthday);
		if (nAge >= 0 && nAge < 16)
		{
			ui->checkBox_Agency->setChecked(true);
			g_pDataCenter->bGuardian = true;
			ShowGuardianWidget(true);
			if (json.KeyExist("Guardian"))
			{
				json.Get("Guardian", pSSCardInfo->strGuardianName);
				ui->lineEdit_Guardian->setText(QString::fromLocal8Bit(pSSCardInfo->strGuardianName.c_str()));
			}

			if (json.KeyExist("GuardianShip"))
			{
				json.Get("GuardianShip", pSSCardInfo->strGuardianShip);
				QString strGuardianShip = pSSCardInfo->strGuardianShip.c_str();
				int nIndex = strGuardianShip.toInt();
				if (nIndex >= 1 && nIndex <= ui->comboBox_Guardianship->count())
					ui->comboBox_Guardianship->setCurrentIndex(nIndex - 1);
				/*int nIndex = ui->comboBox_Guardianship->findText(QString::fromLocal8Bit(pSSCardInfo->strGuardianShip.c_str()));
				if (nIndex != -1)*/
			}

			if (json.KeyExist("GuardianCardID"))
			{
				json.Get("GuardianCardID", pSSCardInfo->strGuardianIDentity);
				ui->lineEdit_GuardianCardID->setText(pSSCardInfo->strGuardianIDentity.c_str());
			}

			string strPhotoGuardian;
			if (json.KeyExist("PhotoGuardian"))
			{
				json.Get("PhotoGuardian", strPhotoGuardian);
				if (fs::exists((strPhotoGuardian)))
					g_pDataCenter->strGuardianPhotoFile = strPhotoGuardian;
			}
		}
		else
		{
			ui->checkBox_Agency->setChecked(false);
			g_pDataCenter->bGuardian = false;
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

void uc_InputIDCardInfo::SavePersonInfo()
{
	SSCardBaseInfoPtr& pSSCardInfo = g_pDataCenter->GetSSCardInfo();
	if (!pSSCardInfo)
		return;
	IDCardInfoPtr& pIDCard = g_pDataCenter->GetIDCardInfo();
	CJsonObject json;

	//QString strProvince = ui->comboBox_Province->currentText();
	//QString strCity = ui->comboBox_City->currentText();
	//QString strCounty = ui->comboBox_County->currentText();
	QString strAddress = ui->plainTextEdit_Address->toPlainText();
	json.Add("Name", pSSCardInfo->strName);
	json.Add("Identity", pSSCardInfo->strIdentity);
	json.Add("Gender", pSSCardInfo->strGender);
	json.Add("Birthday", pSSCardInfo->strBirthday);
	json.Add("Nantion", pSSCardInfo->strNation);
	json.Add("PaperIssuedate", (const char*)pIDCard->szExpirationDate1);
	json.Add("PaperExpiredate", (const char*)pIDCard->szExpirationDate2);
	json.Add("Mobile", pSSCardInfo->strMobile);
	json.Add("BankCode", pSSCardInfo->strBankCode);
	json.Add("City", pSSCardInfo->strCity);

	//json.Add("Province", strProvince.toLocal8Bit().data());
	//json.Add("CityName", strCity.toLocal8Bit().data());
	//json.Add("County", strCounty.toLocal8Bit().data());
	json.Add("Address", strAddress.toLocal8Bit().data());

	try
	{
		if (fs::exists(g_pDataCenter->strSSCardPhotoFile))
			json.Add("Photo", g_pDataCenter->strSSCardPhotoFile);

		if (ui->checkBox_Agency->isChecked())
		{
			json.Add("Guardian", pSSCardInfo->strGuardianName);
			json.Add("GuardianShip", pSSCardInfo->strGuardianShip);
			json.Add("GuardianCardID", pSSCardInfo->strGuardianIDentity);
			if (fs::exists(g_pDataCenter->strSSCardPhotoFile))
				json.Add("GuardianPhoto", g_pDataCenter->strSSCardPhotoFile);
		}

		QString strAppPath = QApplication::applicationDirPath();
		QString strJsonPath = QString("%1/data/Person_%2.json").arg(strAppPath).arg(pSSCardInfo->strIdentity.c_str());
		ofstream ofs(strJsonPath.toLocal8Bit().data(), ios::out | ios::binary);
		ofs << json.ToFormattedString();
		ofs.flush();
		ofs.close();
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
		ui->plainTextEdit_Address->setPlainText(QString::fromLocal8Bit((char*)pIDCard->szAddress));

		ui->dateEdit_Issued->setDateTime(QDateTime::fromString((char*)pIDCard->szExpirationDate1, "yyyyMMdd"));
		ui->dateEdit_Expired->setDateTime(QDateTime::fromString((char*)pIDCard->szExpirationDate2, "yyyyMMdd"));

		QString strNationalty = QString::fromLocal8Bit((char*)pIDCard->szNationalty) + "族";
		int nIndex = ui->comboBox_Nationality->findText(strNationalty);
		if (nIndex != -1)
			ui->comboBox_Nationality->setCurrentIndex(nIndex);
	}
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

void uc_InputIDCardInfo::HideItem()
{
	ui->checkBox_Agency->setVisible(false);
	ui->label_Gender->setVisible(false);
	ui->radioButton_Male->setVisible(false);
	ui->radioButton_Female->setVisible(false);
	ui->label_Nation->setVisible(false);
	ui->comboBox_Nationality->setVisible(false);
	ui->label_ExpireDate->setVisible(false);
	ui->dateEdit_Issued->setVisible(false);
	ui->line->setVisible(false);
	ui->dateEdit_Expired->setVisible(false);
	ui->label_Career->setVisible(false);
	//ui->label_Career->setText("查询范围");
	//struct QueryRange
	//{
	//	QString strValue;
	//	QString strName;
	//};
	//QueryRange strRange[] = {
	//	{"0","放号"},
	//	{"1","正常"},
	//	{"2","挂失"},
	//	{"B","临时挂失"},
	//	{"3","挂失后注销"},
	//	{"4","注销"},
	//	{"A","未启用"}
	//};
	//ui->comboBox_Career->clear();
	//for (auto var : strRange)
	//{
	//	ui->comboBox_Career->addItem(var.strName, var.strValue);
	//}
	//ui->comboBox_Career->setCurrentIndex(1);
	ui->comboBox_Career->setVisible(false);
	ui->comboBox_Career->setVisible(false);
	ui->label_Address->setVisible(false);
	ui->plainTextEdit_Address->setVisible(false);
	ui->label_Guardian->setVisible(false);
	ui->lineEdit_Guardian->setVisible(false);
	ui->label_GuardianMobile->setVisible(false);
	ui->lineEdit_GuardianMobile->setVisible(false);
	ui->label_GuardianIDCard->setVisible(false);
	ui->lineEdit_GuardianCardID->setVisible(false);
	ui->label_Guardianship->setVisible(false);
	ui->comboBox_Guardianship->setVisible(false);
	ui->label_Photo->setVisible(false);
	ui->pushButton_TakePhoto->setVisible(false);
	ui->pushButton_SelectPhoto->setVisible(false);
	ui->pushButton_OK->setVisible(false);
	ui->pushButton_GetCardID->setVisible(true);

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

/*
void uc_InputIDCardInfo::on_comboBox_Province_currentIndexChanged(int index)
{
	if (!bInitialized)
		return;
	ui->comboBox_City->clear();
	int nProvinces = ui->comboBox_Province->count();
	if (index >= nProvinces)
		return;
	QString strID = ui->comboBox_Province->itemData(index).toString();
	QSqlQuery query = g_pDataCenter->SQLiteDB.exec(QString("SELECT id,Name FROM Area_2022 as t where   t.LevelType = 2 and t.ParentId = %1 ORDER BY ID").arg(strID));
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
	QSqlQuery query = g_pDataCenter->SQLiteDB.exec(QString("SELECT id,Name FROM Area_2022 as t where   t.LevelType = 3 and t.ParentId = %1 ORDER BY ID").arg(strID));
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
	QSqlQuery query = g_pDataCenter->SQLiteDB.exec(QString("SELECT id,Name FROM Area_2022 as t where   t.LevelType = 4 and t.ParentId = %1 ORDER BY ID").arg(strID));
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
	QSqlQuery query = g_pDataCenter->SQLiteDB.exec(QString("SELECT id,Name FROM Area_2022 as t where   t.LevelType = 5 and t.ParentId = %1 ORDER BY ID").arg(strID));
	while (query.next())
	{
		QString strID = query.value("ID").toString();
		QString strName = query.value("Name").toString();
		ui->comboBox_Street->addItem(strName, strID);
	}
}
*/

void uc_InputIDCardInfo::ClearInfo()
{
	ui->lineEdit_Name->setText("");
	ui->lineEdit_CardID->setText("");
	ui->plainTextEdit_Address->setPlainText("");
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
}

void uc_InputIDCardInfo::ResetPage()
{
	gInfo() << __FUNCTION__;
	ClearInfo();

	mapPersonFile.clear();
	ShowGuardianWidget(false);
	ClearGuardianInfo();
	ui->plainTextEdit_Address->setPlainText("山东省滨州市");
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
	if (strMobile.size() < 11)
	{
		strMessage = "手机号码输入有误,必须为11位数字!";
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

		int nGuandianShip = ui->comboBox_Guardianship->currentIndex() + 1;
		pSSCardInfo->strGuardianName = strGuardian.toLocal8Bit().data();
		pSSCardInfo->strGuardianIDentity = strGuardianCardID.toStdString();
		pSSCardInfo->strGuardianShip = QString("%1").arg(nGuandianShip).toStdString();
	}
	g_pDataCenter->strCardVersion = "3.00";

	g_pDataCenter->SetSSCardInfo(pSSCardInfo);
	return 0;
}

// 只取姓名和身份证号
int uc_InputIDCardInfo::GetCardInfo(IDCardInfoPtr& pIDCard, QString& strMessage)
{
	gInfo() << __FUNCTION__;
	QString strName = ui->lineEdit_Name->text();
	//QString strMobile = ui->lineEdit_Mobile->text();
	QString strCardID = ui->lineEdit_CardID->text();

	if (strName.isEmpty() ||
		strCardID.isEmpty())

	{
		strMessage = "姓名、身份证号码不能为空!";
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

	strcpy((char*)pIDCard->szName, strName.toLocal8Bit().data());
	strcpy((char*)pIDCard->szIdentity, strCardID.toLocal8Bit().data());
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
	QString strAddress = ui->plainTextEdit_Address->toPlainText();
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
		strGender.isEmpty() ||
		strAddress.isEmpty())
	{
		strMessage = "姓名、民族、性别、身份证号码,住址等信息不能为空!";
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
	strcpy((char*)pIDCard->szName, strName.toLocal8Bit().data());
	strcpy((char*)pIDCard->szIdentity, strCardID.toLocal8Bit().data());
	strncpy((char*)pIDCard->szBirthday, (char*)&pIDCard->szIdentity[6], 8);
	strcpy((char*)pIDCard->szGender, strGender.toLocal8Bit().data());
	strcpy((char*)pIDCard->szNationalty, strNation.toLocal8Bit().data());
	strcpy((char*)pIDCard->szNationaltyCode, strNationCode.toStdString().c_str());
	strcpy((char*)pIDCard->szExpirationDate1, strIssuedate.toLocal8Bit().data());
	strcpy((char*)pIDCard->szExpirationDate2, strExpireDate.toLocal8Bit().data());
	//QString strFullAddress = /*+strProvince + strCity + strCounty  strTown + strStreet  +*/ strAddress;
	strcpy((char*)pIDCard->szAddress, strAddress.toLocal8Bit().data());
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

int uc_InputIDCardInfo::QueryPersonInfo(void* p)
{
	QWaitCursor W;
	QString strMessage;
	QString strCardProgress;
	if (!p)
		return -1;
	SSCardBaseInfo* pSSCardInfo = (SSCardBaseInfo*)p;
	RegionInfo& Reginfo = g_pDataCenter->GetSysConfigure()->Region;
	int nResult = -1;
	do
	{
		IDCardInfoPtr pIDCard = make_shared<IDCardInfo>();
		strcpy((char*)pIDCard->szName, pSSCardInfo->strName.c_str());
		strcpy((char*)pIDCard->szIdentity, pSSCardInfo->strIdentity.c_str());

		SSCardService* pService = nullptr;
		if (QFailed(g_pDataCenter->OpenSSCardService(&pService, strMessage, pIDCard)))
		{
			break;
		}

		if (!pService)
		{
			strMessage = "社保卡卡管服务模块不可用!";
			break;
		}

		CJsonObject jsonIn;

		/*jsonIn.Add("CardID", strCardID);
		jsonIn.Add("Name", strName);
		jsonIn.Add("City", g_pDataCenter->GetSysConfigure()->Region.strCityCode);
		jsonIn.Add("BankCode", g_pDataCenter->GetSysConfigure()->Region.strBankCode);
		string strJsonIn = jsonIn.ToString();
		string strJsonOut;
		if (QFailed(pService->QueryCardInfo(strJsonIn, strJsonOut)))
		{
			CJsonObject jsonOut(strJsonOut);
			string strText;
			jsonOut.Get("Result", nErrCode);
			jsonOut.Get("Message", strText);
			strMessage = QString("查询制卡信息失败:%1").arg(QString::fromLocal8Bit(strText.c_str()));
			break;
		}

		CJsonObject jsonOut(strJsonOut);
		jsonOut.Get("Result", nErrCode);
		if (QFailed(nErrCode))
		{
			string strText;
			jsonOut.Get("Result", nErrCode);
			jsonOut.Get("Message", strText);
			strMessage = QString("查询制卡信息失败:%1").arg(QString::fromLocal8Bit(strText.c_str()));
			break;
		}
		string strSSCardNum, strBankName;
		int nCardStatus;
		jsonOut.Get("CardNum", strSSCardNum);
		jsonOut.Get("CardStatus", nCardStatus);
		jsonOut.Get("BankName", strBankName);
		jsonIn.Clear();*/

		jsonIn.Add("CardID", pSSCardInfo->strIdentity);
		jsonIn.Add("Name", pSSCardInfo->strName);
		jsonIn.Add("City", Reginfo.strCityCode);

		string strJsonIn = jsonIn.ToString();
		string strJsonOut;
		string strCommand = "QueryPersonPhoto";

		if (QFailed(pService->SetExtraInterface(strCommand, strJsonIn, strJsonOut)))
		{
			CJsonObject jsonOut(strJsonOut);
			string strText;
			int nErrCode = -1;
			jsonOut.Get("Result", nErrCode);
			jsonOut.Get("Message", strText);
			strMessage = QString("获取个人照片失败:%1").arg(QString::fromLocal8Bit(strText.c_str()));
			break;
		}
		CJsonObject jsonOut(strJsonOut);
		string strPhoto;
		if (jsonOut.Get("Photo", strPhoto))
		{
			SaveSSCardPhoto(strMessage, strPhoto.c_str(), (char*)pIDCard->szIdentity);
			if (QFailed(SaveSSCardPhotoBase64(strMessage, strPhoto.c_str())))
			{
				strMessage = QString("保存照片数据失败!");
				break;
			}
		}
		else
		{
			strMessage = QString("社保后台未返回个人照片!");
			break;
		}

		strCommand = "QueryPersonInfo";
		if (QFailed(pService->SetExtraInterface(strCommand, strJsonIn, strJsonOut)))
		{
			CJsonObject jsonOut(strJsonOut);
			string strText;
			int nErrCode = -1;
			jsonOut.Get("Result", nErrCode);
			jsonOut.Get("Message", strText);
			strMessage = QString("获取个人信息失败:%1").arg(QString::fromLocal8Bit(strText.c_str()));
			break;
		}
		jsonOut.Parse(strJsonOut);
		string strAddress, strMobile, strOccupation, strNation, strPaperIssuedDate, strPaperExpireDate;
		jsonOut.Get("Address", pSSCardInfo->strAddress);
		jsonOut.Get("Mobile", pSSCardInfo->strMobile);
		jsonOut.Get("Occupation", pSSCardInfo->strOccupType);
		jsonOut.Get("Nation", pSSCardInfo->strNation);
		jsonOut.Get("PaperIssuedDate", pSSCardInfo->strReleaseDate);
		jsonOut.Get("PaperExpireDate", pSSCardInfo->strValidDate);

		nResult = 0;
	} while (0);
	gInfo() << GBKString(strMessage);
	return nResult;
}

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
		ui->label_GuardianMobile->setMinimumSize(QSize(200, 60));
		ui->label_GuardianMobile->setMaximumSize(QSize(200, 60));
		ui->lineEdit_GuardianMobile->setMinimumSize(QSize(300, 60));
		ui->lineEdit_GuardianMobile->setMaximumSize(QSize(300, 60));


		if (ui->horizontalLayout_Guardian->count() == 5)
		{
			horizontalSpacer_Mobile = new QSpacerItem(40, 20, QSizePolicy::Fixed, QSizePolicy::Minimum);
			ui->horizontalLayout_Guardian->insertSpacerItem(2, horizontalSpacer_Mobile);
		}

		qDebug() << "Item Count =" << ui->horizontalLayout_Guardian->count();
	}
	else
	{
		ui->label_GuardianMobile->setMinimumSize(QSize(220, 60));
		ui->label_GuardianMobile->setMaximumSize(QSize(220, 60));
		ui->lineEdit_GuardianMobile->setMinimumSize(QSize(700, 60));
		ui->lineEdit_GuardianMobile->setMaximumSize(QSize(700, 60));
		if (ui->horizontalLayout_Guardian->count() == 6)
		{
			horizontalSpacer_Mobile = (QSpacerItem*)ui->horizontalLayout_Guardian->itemAt(2);
			ui->horizontalLayout_Guardian->removeItem(horizontalSpacer_Mobile);
			qDebug() << "Item Count =" << ui->horizontalLayout_Guardian->count();
			//horizontalSpacer_Mobile->objectName();
		}
	}
	//int nCount = ui->horizontalLayout_Guardian->count();
	//for (int i = 0; i < nCount; i++)
	//{
	//	ui->horizontalLayout_Guardian->itemAt(i);
	//}
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
			break;

		if (QFailed(nResult = GetSSCardInfo(strMessage)))
			break;

		SavePersonInfo();
		nResult = 0;
	} while (0);
	if (QFailed(nResult))
	{
		//QMessageBox_CN(QMessageBox::Critical, "提示", strMessage, QMessageBox::Ok, this);
		emit ShowMaskWidget("提示", strMessage, Fetal, Stay_CurrentPage);
		return;
	}
	bool bGuardian = false;
	if (GetAge(string((char*)g_pDataCenter->GetIDCardInfo()->szBirthday)) < 16)
		bGuardian = true;
	DialogCameraTest dialog(bGuardian);
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
				if (QSucceed(GetImageStorePath(strSSCardPhoto, PhotoType::Photo_SSCard)))
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

		// 图像文件内容非空，但不是当前人员的照片
		if (!g_pDataCenter->strSSCardPhotoFile.empty())
		{
			QImage Photo(g_pDataCenter->strSSCardPhotoFile.c_str());
			if (Photo.isNull())
			{
				strMessage = "照片读取失败,请重新采集或上传照片!";
				break;
			}
			if (Photo.width() != 358 || Photo.height() != 441)
			{
				strMessage = "照片尺寸不符要求,请重新采集或上传照片!";
				break;
			}
		}

		/*if (GetAge(string((char*)g_pDataCenter->GetIDCardInfo()->szBirthday)) < 16 &&
			!fs::exists(g_pDataCenter->strGuardianPhotoFile))
		{
			strMessage = "未成年人办卡时,需要同时上传本人和监护人的照片!";
			break;
		}*/

		SavePersonInfo();

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
				QString qstrText = QString::fromLocal8Bit(strText.c_str());
				if ((nErrCode == 3) || (nErrCode == 4 && qstrText.contains("放号")))	// 已经申请过,则继续制卡
				{
					strMessage = qstrText;
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

void uc_InputIDCardInfo::on_pushButton_GetCardID_clicked()
{
	gInfo() << __FUNCTION__;
	WaitCursor();
	QString strMessage;
	int nResult = -1;
	int nOperation = -1;
	int nPage = 0;
	MaskStatus nStatus = Success;
	QString strTips = "操作成功";
	do
	{
		IDCardInfoPtr pIDCard = make_shared<IDCardInfo>();
		if (QFailed(GetCardInfo(pIDCard, strMessage)))
		{
			break;
		}
		g_pDataCenter->SetIDCardInfo(pIDCard);
		nOperation = Goto_Page;
		nPage = Page_QueryInformation;
		strMessage = "请稍候,正在查询!";
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
	//桌面版注释

	//if (bChecked)
	//{

	//	StartDetect();
	//	pDialogReadIDCard = new Sys_DialogReadIDCard("请刷监护人身份证");
	//	if (pDialogReadIDCard->exec() != QDialog::Accepted)
	//	{
	//		ui->checkBox_Agency->setChecked(false);
	//		ShowGuardianWidget(false);
	//		g_pDataCenter->bGuardian = false;
	//	}
	//	delete pDialogReadIDCard;
	//	pDialogReadIDCard = nullptr;
	//	StopDetect();
	//}
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

void uc_InputIDCardInfo::on_Name_EditingFinished()
{
	//QString arg1 = ui->lineEdit_Name->text();
	//auto itfind = mapPersonFile.find(arg1);
	//if (itfind != mapPersonFile.end())
	//{
	//	ClearInfo();
	//	LoadPersonInfo(itfind->second);
	//}
}

void uc_InputIDCardInfo::on_CheckPersonInfo()
{
	if (g_pDataCenter->nCardServiceType == ServiceType::Service_QueryInformation)
		return;

	QString strMessage;
	IDCardInfoPtr pIDCard = make_shared<IDCardInfo>();
	if (QFailed(GetCardInfo(pIDCard, strMessage)))
	{
		return;
	}

	SSCardBaseInfoPtr  pSSCardInfo = make_shared<SSCardBaseInfo>();
	pSSCardInfo->strIdentity = (const char*)pIDCard->szIdentity;
	pSSCardInfo->strName = (const char*)pIDCard->szName;

	int nResult = -1;
	WaitingProgress WaitingUI(uc_InputIDCardInfo::QueryPersonInfo,
		pSSCardInfo.get(),
		30,
		QString("正在查询人员信息:%1%"),
		false);
	if (WaitingUI.exec() == QDialog::Accepted)
	{
		ui->plainTextEdit_Address->setPlainText(QString::fromLocal8Bit(pSSCardInfo->strAddress.c_str()));
		int nIndex = ui->comboBox_Nationality->findData(pSSCardInfo->strNation.c_str());
		if (nIndex != -1)
			ui->comboBox_Nationality->setCurrentIndex(nIndex);
		nIndex = ui->comboBox_Career->findData(pSSCardInfo->strOccupType.c_str());
		if (nIndex != -1)
			ui->comboBox_Career->setCurrentIndex(nIndex);
		if (pSSCardInfo->strMobile.size())
			ui->lineEdit_GuardianMobile->setText(pSSCardInfo->strMobile.c_str());
		ui->dateEdit_Issued->setDate(QDate::fromString(pSSCardInfo->strReleaseDate.c_str(), "yyyyMMdd"));
		ui->dateEdit_Expired->setDate(QDate::fromString(pSSCardInfo->strValidDate.c_str(), "yyyyMMdd"));

		QString strStyle = QString("border-image: url(%1);").arg(g_pDataCenter->strSSCardPhotoFile.c_str());
		ui->label_Photo->setStyleSheet(strStyle);
		ui->label_Photo->setText("");
	}
}

void uc_InputIDCardInfo::on_lineEdit_Name_textEdited(const QString& arg1)
{
	if (arg1.size() < 2)
		return;

	QString strCardID = ui->lineEdit_CardID->text();
	if (strCardID.size() < 18)
		return;

	char nX = VerifyCardID(strCardID.toStdString().c_str());
	if (nX != strCardID.toStdString().at(17))
		return;
	emit CheckPersonInfo();
}

void uc_InputIDCardInfo::on_lineEdit_CardID_textEdited(const QString& arg1)
{
	QString strCardID = arg1;
	if (strCardID.size() < 18)
		return;

	char nX = VerifyCardID(strCardID.toStdString().c_str());
	if (nX != strCardID.toStdString().at(17))
		return;

	QString strName = ui->lineEdit_Name->text();
	if (strName.size() < 2)
		return;
	emit CheckPersonInfo();
}
