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
	ui->lineEdit_CardID->setValidator(new QRegExpValidator(QRegExp("[0-9,X]+$")));   //只能输入数字
	ui->lineEdit_Mobile->setValidator(new QRegExpValidator(QRegExp("[0-9]+$")));   //只能输入数字
	ui->lineEdit_ExpiredDate->setValidator(new QRegExpValidator(QRegExp("[0-9]+$")));   //只能输入数字
	ui->lineEdit_IssuedDate->setValidator(new QRegExpValidator(QRegExp("[0-9]+$")));   //只能输入数字
	for (auto var : g_vecNationCode)
	{
		ui->comboBox_Nationality->addItem(var.strNationalty.c_str(), var.strCode.c_str());
	}

	if (g_pDataCenter->bDebug)
	{
		SSCardBaseInfoPtr pSSCardInfo = make_shared<SSCardBaseInfo>();
		IDCardInfoPtr pIDCard = make_shared<IDCardInfo>();
		if (QSucceed(LoadTestIDData(pIDCard, pSSCardInfo)))
		{
			ui->lineEdit_Name->setText(QString::fromLocal8Bit((char*)pIDCard->szName));
			ui->lineEdit_CardID->setText(QString::fromLocal8Bit((char*)pIDCard->szIdentity));
			ui->comboBox_Nationality->setCurrentText(QString::fromLocal8Bit((char*)pIDCard->szNationalty));
			ui->lineEdit_IssuedDate->setText(QString::fromLocal8Bit((char*)pIDCard->szExpirationDate1));
			ui->lineEdit_ExpiredDate->setText(QString::fromLocal8Bit((char*)pIDCard->szExpirationDate2));
			ui->lineEdit_Mobile->setText(QString::fromLocal8Bit(pSSCardInfo->strMobile.c_str()));
			if (QString::fromLocal8Bit((char*)pIDCard->szGender) == "男")
				ui->radioButton_Male->setChecked(true);
			else
				ui->radioButton_Female->setChecked(true);
		}
	}
	//ui->comboBox_Nationality->setCurrentIndex(0);
	//connect(&ui->lineEdit_Nation,&QClickableLineEdit::clicked,this,)
}

uc_InputIDCardInfo::~uc_InputIDCardInfo()
{
	delete ui;
}

int uc_InputIDCardInfo::TestInput(QString& strMessage)
{
	QString strName = ui->lineEdit_Name->text();
	QString strMobile = ui->lineEdit_Mobile->text();
	QString strCardID = ui->lineEdit_CardID->text();
	QString strNation = ui->comboBox_Nationality->currentText();
	QString strIssuedate = ui->lineEdit_IssuedDate->text();
	QString strExpireDate = ui->lineEdit_ExpiredDate->text();

	if (strName.isEmpty() ||
		strMobile.isEmpty() ||
		strCardID.isEmpty() ||
		strNation.isEmpty() ||
		strIssuedate.isEmpty() ||
		strExpireDate.isEmpty())
	{
		strMessage = "姓名、手机、民族、身份证号码、证件有效期等信息不能为空!";
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
	if (strMobile.size() < 11)
	{
		strMessage = "手机号码位数不足，必须为11位的数字!";
		return -1;
	}
	return 0;
}


void uc_InputIDCardInfo::on_pushButton_Query_clicked()
{
	QString strMessage;
	int nResult = -1;
	QString strCardProgress;
	RegionInfo& Reginfo = g_pDataCenter->GetSysConfigure()->Region;
	int nErrCode = -1;
	do
	{
		if (QFailed(TestInput(strMessage)))
		{
			break;
		}

		SSCardService* pService = nullptr;
		if (QFailed(g_pDataCenter->OpenSSCardService(&pService, strMessage)))
		{
			break;
		}

		if (!pService)
		{
			strMessage = "社保卡卡管服务不可用!";
			break;
		}

		CJsonObject jsonIn;
		string strCardID = ui->lineEdit_CardID->text().toStdString();
		string strName = ui->lineEdit_Name->text().toLocal8Bit().data();

		jsonIn.Add("CardID", strCardID);
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

		ui->lineEditl_SSCard->setText(QString::fromLocal8Bit(strSSCardNum.c_str()));
		ui->lineEdit_Status->setText(CardStatusString((CardStatus)nCardStatus));
		ui->lineEdit_Bank->setText(QString::fromLocal8Bit(strBankName.c_str()));

		jsonIn.Clear();
		jsonIn.Add("CardID", strCardID);
		jsonIn.Add("Name", strName);
		jsonIn.Add("City", Reginfo.strCityCode);

		strJsonIn = jsonIn.ToString();
		string strJsonout;
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
		jsonOut.Parse(strJsonOut);
		string strPhoto;
		if (jsonOut.Get("Photo", strPhoto))
		{
			SaveSSCardPhoto(strMessage, strPhoto.c_str());
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

		QString strStyle = QString("border-image: url(%1);").arg(g_pDataCenter->strSSCardPhotoFile.c_str());
		ui->label_Photo->setStyleSheet(strStyle);
		nResult = 0;
	} while (0);

	if (QFailed(nResult))
	{
		((uc_ReadIDCard*)parent())->emit ShowMaskWidget("操作失败", strMessage, Fetal, Stay_CurrentPage);
	}
    ui->pushButton_OK->setEnabled(true);
	return;
}

void uc_InputIDCardInfo::on_pushButton_OK_clicked()
{
    ((uc_ReadIDCard*)parent())->emit ShowMaskWidget("操作成功", "操作成功", Success, Switch_NextPage);
}

//void uc_InputIDCardInfo::on_lineEdit_Nation_clicked()
//{
//	((uc_ReadIDCard*)parent())->emit ShowNationWidget(true);
//}

void uc_InputIDCardInfo::on_ShowWidgets(QWidget* pWidget, bool bShow)
{
	ShowWidgets(this, bShow);
	if (bShow && pNationWidget)
		pNationWidget->hide();
}
