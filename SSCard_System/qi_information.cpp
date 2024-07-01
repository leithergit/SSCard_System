#include "qi_information.h"
#include "ui_qi_information.h"
#include "mainwindow.h"
#include "Payment.h"

qi_Information::qi_Information(QLabel* pTitle, QString strStepImage, Page_Index nIndex, QWidget* parent) :
	QStackPage(pTitle, strStepImage, nIndex, parent),
	ui(new Ui::qi_Information)
{
	ui->setupUi(this);
}

qi_Information::~qi_Information()
{
	delete ui;
}

void qi_Information::ShutDown()
{

}

void qi_Information::ResetPage()
{
	ui->label_Photo;
	ui->lineEdit_Name->clear();
	ui->lineEdit_CardID->clear();
	ui->lineEditl_SSCard->clear();
	ui->lineEdit_CardStatus->clear();
	ui->lineEdit_Bank->clear();
}

int qi_Information::ProcessBussiness()
{
	QString strMessage;
	int nResult = -1;
	QString strCardProgress;
	SSCardBaseInfoPtr& pSSCardInfo = g_pDataCenter->GetSSCardInfo();
	if (!pSSCardInfo)
		pSSCardInfo = make_shared<SSCardBaseInfo>();
	IDCardInfoPtr& pIDCard = g_pDataCenter->GetIDCardInfo();
	RegionInfo& Reginfo = g_pDataCenter->GetSysConfigure()->Region;
	//g_pDataCenter->nCardStratus = CardStatus::Card_Unknow;
	do
	{
		if (g_pDataCenter->bDebug)
		{
			string strMobile;
			if (QFailed(LoadTestIDData(pIDCard, pSSCardInfo)))
			{
				//pSSCardInfo->strMobile = strMobile.c_str();
				strMessage = "测试用户数据不存在!";
				break;
			}
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
		jsonIn.Add("CardID", (char*)pIDCard->szIdentity);
		jsonIn.Add("Name", (const char*)pIDCard->szName);
		jsonIn.Add("City", g_pDataCenter->GetSysConfigure()->Region.strCityCode);
		jsonIn.Add("BankCode", g_pDataCenter->GetSysConfigure()->Region.strBankCode);
		jsonIn.Add("DocType", pSSCardInfo->strCardType);
		string strJsonIn = jsonIn.ToString();
		string strJsonOut;
		switch (g_pDataCenter->nCardStratus)
		{
		case CardStatus::Card_Normal:
			if (QFailed(pService->QueryCardInfo(strJsonIn, strJsonOut)))
			{
				CJsonObject jsonOut(strJsonOut);
				string strText;
				int nErrCode = -1;
				jsonOut.Get("Result", nErrCode);
				jsonOut.Get("Message", strText);
				strMessage = QString("查询信息失败:%1").arg(QString::fromLocal8Bit(strText.c_str()));
				break;
			}
			break;
		case CardStatus::Card_Release:
			if (QFailed(pService->QueryCardInfo0(strJsonIn, strJsonOut)))
			{
				CJsonObject jsonOut(strJsonOut);
				string strText;
				int nErrCode = -1;
				jsonOut.Get("Result", nErrCode);
				jsonOut.Get("Message", strText);
				strMessage = QString("查询信息失败:%1").arg(QString::fromLocal8Bit(strText.c_str()));
				break;
			}
			break;
		case CardStatus::Card_RegisterLost:
			if (QFailed(pService->QueryCardInfo2(strJsonIn, strJsonOut)))
			{
				CJsonObject jsonOut(strJsonOut);
				string strText;
				int nErrCode = -1;
				jsonOut.Get("Result", nErrCode);
				jsonOut.Get("Message", strText);
				strMessage = QString("查询信息失败:%1").arg(QString::fromLocal8Bit(strText.c_str()));
				break;
			}
			break;
		case CardStatus::Card_CanceledOnRegisterLost:
			jsonIn.Add("StatustoQuery","3");
			if (QFailed(pService->QueryCardInfoS(strJsonIn, strJsonOut)))
			{
				CJsonObject jsonOut(strJsonOut);
				string strText;
				int nErrCode = -1;
				jsonOut.Get("Result", nErrCode);
				jsonOut.Get("Message", strText);
				strMessage = QString("查询信息失败:%1").arg(QString::fromLocal8Bit(strText.c_str()));
				break;
			}
			break;
		case CardStatus::Card_RegisgerTempLost:
			jsonIn.Add("StatustoQuery", "B");
			if (QFailed(pService->QueryCardInfoS(strJsonIn, strJsonOut)))
			{
				CJsonObject jsonOut(strJsonOut);
				string strText;
				int nErrCode = -1;
				jsonOut.Get("Result", nErrCode);
				jsonOut.Get("Message", strText);
				strMessage = QString("查询信息失败:%1").arg(QString::fromLocal8Bit(strText.c_str()));
				break;
			}
			break;
		case CardStatus::Card_Canceled:
			jsonIn.Add("StatustoQuery", "4");
			if (QFailed(pService->QueryCardInfoS(strJsonIn, strJsonOut)))
			{
				CJsonObject jsonOut(strJsonOut);
				string strText;
				int nErrCode = -1;
				jsonOut.Get("Result", nErrCode);
				jsonOut.Get("Message", strText);
				strMessage = QString("查询信息失败:%1").arg(QString::fromLocal8Bit(strText.c_str()));
				break;
			}
			break;
		case CardStatus::Card_DeActived:
			jsonIn.Add("StatustoQuery", "A");
			if (QFailed(pService->QueryCardInfoS(strJsonIn, strJsonOut)))
			{
				CJsonObject jsonOut(strJsonOut);
				string strText;
				int nErrCode = -1;
				jsonOut.Get("Result", nErrCode);
				jsonOut.Get("Message", strText);
				strMessage = QString("查询信息失败:%1").arg(QString::fromLocal8Bit(strText.c_str()));
				break;
			}
			break;
		default:
			break;
		}
		

		CJsonObject jsonOut(strJsonOut);
		string strSSCardNum, strBankCode, strMobile;
		int nCardStatus;
		if (strMessage.contains("未查询到"))
		{
			break;
		}

		if (!jsonOut.Get("CardNum", strSSCardNum) ||
			!jsonOut.Get("CardStatus", nCardStatus) ||
			!jsonOut.Get("BankCode", strBankCode) ||
			!jsonOut.Get("Mobile", strMobile))
		{
			strMessage = "姓名,身份证,社保卡号,社保卡状态,银行代码存在无效字段!";
			gInfo() << strJsonOut;
			break;
		}
		string chipNum;
		jsonOut.Get("ChipNum", chipNum);
		jsonIn.Add("ChipNum", chipNum);

		if (QFailed(pService->QueryCardBalance(strJsonIn, strJsonOut)))
		{
			CJsonObject jsonOut(strJsonOut);
			string strText;
			int nErrCode = -1;
			jsonOut.Get("Result", nErrCode);
			jsonOut.Get("Message", strText);
			strMessage = QString("查询余额失败:%1").arg(QString::fromLocal8Bit(strText.c_str()));
			emit ShowMaskWidget("操作失败", strMessage, Fetal, Return_MainPage);
			return -1;
		}
		CJsonObject jsonBalanceOut(strJsonOut);
		
		gInfo() << strJsonOut << std::endl;
		string yuan;
		jsonBalanceOut.Get("Yuan", yuan);
		gInfo() << "yuan : " << yuan;
		QString Yuan = QString::fromStdString(yuan).toLocal8Bit();
		ui->lineEdit_Balance->setText(Yuan);

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
		}
		else
		{
			strMessage = QString("社保后台未返回个人照片!");
			break;
		}

		QString strStyle = QString("border-image: url(%1);").arg(g_pDataCenter->strSSCardPhotoFile.c_str());
		ui->label_Photo->setStyleSheet(strStyle);

		g_pDataCenter->nCardStratus = (CardStatus)nCardStatus;
		pSSCardInfo->strCardNum = strSSCardNum;
		pSSCardInfo->strBankCode = strBankCode;
		pSSCardInfo->strOrganID = Reginfo.strAgency;
		pSSCardInfo->strTransType = "5";
		pSSCardInfo->strCity = Reginfo.strCityCode;
		pSSCardInfo->strSSQX = Reginfo.strCountry;
		pSSCardInfo->strCardVender = Reginfo.strCardVendor;
		nResult = 0;
	} while (0);

	if (QFailed(nResult))
	{
		gError() << gQStr(strMessage);
		emit ShowMaskWidget("操作失败", strMessage, Fetal, Return_MainPage);
		return nResult;
	}

	pSSCardInfo->strName = (const char*)pIDCard->szName;
	pSSCardInfo->strGender = (const char*)pIDCard->szGender;
	pSSCardInfo->strNation = (const char*)pIDCard->szNationalty;
	pSSCardInfo->strNationCode = (const char*)pIDCard->szNationaltyCode;
	pSSCardInfo->strBirthday = (const char*)pIDCard->szBirthday;
	pSSCardInfo->strIdentity = (const char*)pIDCard->szIdentity;
	pSSCardInfo->strAddress = (const char*)pIDCard->szAddress;

	g_pDataCenter->SetSSCardInfo(pSSCardInfo);

	QString strCardStatus2 = "未知";
	switch (g_pDataCenter->nCardStratus)
	{
	case CardStatus::Card_Release:
		strCardStatus2 = "放号";
		break;
	case CardStatus::Card_Normal:
		strCardStatus2 = "正常";
		break;
	case CardStatus::Card_RegisterLost:
		strCardStatus2 = "挂失";
		break;
	case CardStatus::Card_RegisgerTempLost:
		strCardStatus2 = "临时挂失";
		break;
	case CardStatus::Card_CanceledOnRegisterLost:
		strCardStatus2 = "挂失后注销";
		break;
	case CardStatus::Card_Canceled:
		strCardStatus2 = "注销";
		break;
	case CardStatus::Card_DeActived:
		strCardStatus2 = "未启用";
		break;
	case CardStatus::Card_Making:
		strCardStatus2 = "制卡中";
		break;
	default:
	case CardStatus::Card_Unknow:
		strCardStatus2 = "未知";
		break;
	}
	ui->lineEditl_SSCard->setText(QString::fromLocal8Bit(pSSCardInfo->strCardNum.c_str()));
	string strBankName;
	if (QFailed(g_pDataCenter->GetBankName(pSSCardInfo->strBankCode, strBankName)))
	{
		strBankName = pSSCardInfo->strBankCode;
	}
	ui->lineEdit_Bank->setText(strBankName.c_str());
	ui->lineEdit_CardStatus->setText(strCardStatus2);
	ui->lineEdit_Name->setText(QString::fromLocal8Bit(pSSCardInfo->strName.c_str()));
	ui->lineEdit_CardID->setText(pSSCardInfo->strIdentity.c_str());
	if (g_pDataCenter->nCardServiceType == ServiceType::Service_QueryInformation)
	{
		ui->pushButton_ModifyInfo->show();
	}
	else
	{
		ui->pushButton_ModifyInfo->hide();
	}
	return nResult;
}

void qi_Information::OnTimeout()
{
	ShutDown();
}

void qi_Information::on_pushButton_OK_clicked()
{
	((MainWindow*)qApp->activeWindow())->on_pushButton_MainPage_clicked();
}

void qi_Information::on_pushButton_ModifyInfo_clicked()
{
	emit ShowMaskWidget("请稍等", "", Success, Goto_Page, Page_ModifyInfo);
}


void qi_Information::on_pushButton_ModifPhoto_clicked()
{
	gInfo() << __FUNCTION__;
	QString strMessage;
	int nResult = -1;
	SSCardBaseInfoPtr& pSSCardInfo = g_pDataCenter->GetSSCardInfo();
	if (!pSSCardInfo)
		pSSCardInfo = make_shared<SSCardBaseInfo>();
	IDCardInfoPtr& pIDCard = g_pDataCenter->GetIDCardInfo();
	RegionInfo& Reginfo = g_pDataCenter->GetSysConfigure()->Region;
	try
	{
		do
		{
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
			jsonIn.Add("CardID", (char*)pIDCard->szIdentity);
			jsonIn.Add("Name", (const char*)pIDCard->szName);
			jsonIn.Add("City", g_pDataCenter->GetSysConfigure()->Region.strCityCode);
			jsonIn.Add("BankCode", g_pDataCenter->GetSysConfigure()->Region.strBankCode);
			jsonIn.Add("DocType", pSSCardInfo->strCardType);
			jsonIn.Add("Operator",pSSCardInfo->strOperator);
			jsonIn.Add("SystemType", "26");
			string strJsonIn = jsonIn.ToString();
			string strJsonOut;

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
			if QFailed(g_pDataCenter->LoadPhoto(pSSCardInfo->strPhoto, strMessage))
				break;
			//发送修改请求
			jsonIn.Add("Photo", pSSCardInfo->strPhoto);
			pService->ModifyPhoto(strJsonIn, strJsonOut);

			//发送查询照片请求，确认修改结果
			string strCommand = "QueryPersonPhoto";
			CJsonObject jsonOut(strJsonOut);
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
			string Photo;
			if (jsonOut.Get("Photo", Photo))
			{
				SaveSSCardPhoto(strMessage, Photo.c_str());
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


void qi_Information::on_pushButton_ActiveCard_clicked()
{
	gInfo() << __FUNCTION__;
	QString strMessage;
	int nResult = -1;
	SSCardBaseInfoPtr& pSSCardInfo = g_pDataCenter->GetSSCardInfo();
	if (!pSSCardInfo)
		pSSCardInfo = make_shared<SSCardBaseInfo>();
	IDCardInfoPtr& pIDCard = g_pDataCenter->GetIDCardInfo();
	RegionInfo& Reginfo = g_pDataCenter->GetSysConfigure()->Region;
	try
	{
		do
		{
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
			jsonIn.Add("CardID", (char*)pIDCard->szIdentity);
			jsonIn.Add("Name", (const char*)pIDCard->szName);
			jsonIn.Add("City", g_pDataCenter->GetSysConfigure()->Region.strCityCode);
			jsonIn.Add("BankCode", g_pDataCenter->GetSysConfigure()->Region.strBankCode);
			jsonIn.Add("DocType", pSSCardInfo->strCardType);
			jsonIn.Add("Operator", pSSCardInfo->strOperator);
			jsonIn.Add("CardNum", pSSCardInfo->strCardNum);
			jsonIn.Add("SystemType", "26");
			string strJsonIn = jsonIn.ToString();
			string strJsonOut;

			
			if QFailed(pService->ActiveCard(strJsonIn, strJsonOut))
			{
				CJsonObject jsonOut(strJsonOut);
				string strText;
				int nErrCode = -1;
				jsonOut.Get("Result", nErrCode);
				jsonOut.Get("Message", strText);
				strMessage = QString("启用社保卡失败:%1").arg(QString::fromLocal8Bit(strText.c_str()));
				break;
			}

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

