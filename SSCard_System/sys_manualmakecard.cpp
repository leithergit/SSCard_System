#include "sys_manualmakecard.h"
#include "ui_sys_manualmakecard.h"
#include "Sys_dialogidcardinfo.h"
#include <QFileInfo>

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
		ui->label_Pohoto->setStyleSheet(strStyle);
	}
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
		ui->lineEdit_CardID->setText((char*)pIDCard->szIdentify);
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
	if (!ui->checkBox->isChecked())
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
	else
	{
		string strCardID, strName, strMobile;
		g_pDataCenter->bDebug = true;
		if (QSucceed(LoadTestData(strName, strCardID, strMobile)))
		{
			IDCardInfoPtr pIDCard = make_shared<IDCardInfo>(CardInfo);
			g_pDataCenter->SetIDCardInfo(pIDCard);
			strcpy((char*)pIDCard->szName, strName.c_str());
			strcpy((char*)pIDCard->szIdentify, strCardID.c_str());
			ui->lineEdit_CardID->setText((char*)pIDCard->szIdentify);
			ui->lineEdit_Name->setText(QString::fromLocal8Bit((const char*)pIDCard->szName));
		}
		else
		{
			QMessageBox_CN(QMessageBox::Information, tr("提示"), "加载测试数据失败,请确认数据是否存在!", QMessageBox::Ok, this);
		}
		g_pDataCenter->bDebug = false;
	}
}

int Sys_ManualMakeCard::ReadSSCardInfo(QString& strMessage, int& nStatus, SSCardInfoPtr& pSSCardInfo)
{
	int nResult = 0;
	char szStatus[1024] = { 0 };
	if (QFailed(nResult = queryPersonInfo(*pSSCardInfo, szStatus)))
	{
		QString strInfo = QString("queryPersonInfo失败:%1,CardID = %2\tName = %3\t").arg(nResult).arg((char*)pSSCardInfo->strCardID).arg((char*)pSSCardInfo->strName);
		gError() << gQStr(strInfo);
		strMessage = QString("查询社保卡信息失败!");
		return -1;
	}
	if ((szStatus[0] >= '0' && szStatus[0] <= '9') &&
		(szStatus[1] >= '0' && szStatus[1] <= '9'))
	{
		nStatus = strtolong(szStatus, 10, 2);
	}
	else
	{
		strMessage = szStatus;
		return -1;
	}
	//g_pDataCenter->SetSSCardInfo(pSSCardInfo);
	return 0;
}

int	 Sys_ManualMakeCard::QuerySSCardStatus(QString& strMessage, SSCardInfoPtr& pSSCardInfo)
{
	int nResult = 0;
	char* szStatus[1024] = { 0 };

	// 这里需要旧卡号
	if (QFailed(nResult = queryCardStatus(*pSSCardInfo, reinterpret_cast<char*>(szStatus))))
	{
		QString strInfo = QString("queryCardStatus失败:%1,CardID = %2\tName = %3\t").arg(nResult).arg(reinterpret_cast<char*>(pSSCardInfo->strCardID)).arg(reinterpret_cast<char*>(pSSCardInfo->strName));
		gError() << gQStr(strInfo);
		strMessage = QString("查询卡状态失败!");
		return -1;
	}

	if (strcmp(_strupr((char*)szStatus), "OK") == 0)
	{
		strcpy(pSSCardInfo->strCardStatus, "正常");
	}
	else
	{
		QString strStatus = QString::fromLocal8Bit((char*)szStatus);
		strcpy(pSSCardInfo->strCardStatus, (char*)strStatus.toStdString().c_str());
	}

	return 0;
}

void Sys_ManualMakeCard::on_pushButton_QueryCardStatus_clicked()
{
	QString strMessage;
	int nStatus = 0;
	int nResult = -1;
	QString strCardProgress;

	SSCardInfoPtr pSSCardInfo = make_shared<SSCardInfo>();
	IDCardInfoPtr& pIDCard = g_pDataCenter->GetIDCardInfo();
	if (!pIDCard)
	{
		QMessageBox_CN(QMessageBox::Information, tr("提示"), "身份证数据无效,请先读取身份证!", QMessageBox::Ok, this);
		return;
	}
	RegionInfo& Reginfo = g_pDataCenter->GetSysConfigure()->Region;
	string strBankName;
	do
	{
		strcpy((char*)pSSCardInfo->strName, (const char*)pIDCard->szName);
		strcpy((char*)pSSCardInfo->strCardID, (const char*)pIDCard->szIdentify);
		strcpy((char*)pSSCardInfo->strOrganID, Reginfo.strAgency.c_str());
		strcpy((char*)pSSCardInfo->strBankCode, Reginfo.strBankCode.c_str());
		strcpy((char*)pSSCardInfo->strTransType, "5");
		strcpy((char*)pSSCardInfo->strCity, Reginfo.strCityCode.c_str());
		strcpy((char*)pSSCardInfo->strSSQX, Reginfo.strCountry.c_str());
		strcpy((char*)pSSCardInfo->strCard, Reginfo.strCardVendor.c_str());
		strcpy((char*)pSSCardInfo->strBankCode, Reginfo.strBankCode.c_str());

		g_pDataCenter->SetSSCardInfo(pSSCardInfo);
		SSCardInfoPtr pTempSSCardInfo = make_shared<SSCardInfo>();
		strcpy((char*)pTempSSCardInfo->strName, (const char*)pIDCard->szName);
		strcpy((char*)pTempSSCardInfo->strCardID, (const char*)pIDCard->szIdentify);

		if (QFailed(QueryCardProgress(strMessage, nStatus, pTempSSCardInfo)))
		{
			break;
		}

		g_pDataCenter->strCardMakeProgress = QString::fromLocal8Bit(pTempSSCardInfo->strCardStatus).toStdString();
		if (g_pDataCenter->strCardMakeProgress == "制卡中")
		{
			// 可以获取新的社保卡一些数据
			if (QFailed(nResult = (ReadSSCardInfo(strMessage, nStatus, pSSCardInfo))))
				break;
			if (nStatus != 0 && nStatus != 1)
				break;
			nResult = 0;
			break;
		}

		// 可取旧社保卡号
		if (QFailed(ReadSSCardInfo(strMessage, nStatus, pSSCardInfo)))
			break;

		if (nStatus != 0 && nStatus != 1)
			break;

		if (QFailed(QuerySSCardStatus(strMessage, pSSCardInfo)))
			break;

		if (nStatus != 0)
		{
			break;
		}

		strcpy((char*)pSSCardInfo->strBankCode, pTempSSCardInfo->strBankCode);

		ui->lineEditl_SSCard->setText(QString(pSSCardInfo->strCardNum));


		if (QFailed(g_pDataCenter->GetBankName(pSSCardInfo->strBankCode, strBankName)))
		{
			strMessage = QString("未知的银行代码:%1").arg(pSSCardInfo->strBankCode);
			break;
		}


		string strCardStatus;
		ui->lineEdit_CardStatus->setText(QString("卡片状态:%1").arg(pSSCardInfo->strCardStatus));
		nResult = 0;
	} while (0);
	ui->lineEdit_Bank->setText(QString("服务银行:%1").arg(QString(strBankName.c_str())));
	ui->lineEdit_CardStatus->setText(g_pDataCenter->strCardMakeProgress.c_str());

	if (QFailed(nResult))
	{
		gError() << gQStr(strMessage);
		QMessageBox_CN(QMessageBox::Information, tr("提示"), strMessage, QMessageBox::Ok, this);
		return;
	}

}

int Sys_ManualMakeCard::LoadPersonSSCardData(QString& strMesssage)
{
	IDCardInfoPtr pIDCard = g_pDataCenter->GetIDCardInfo();
	SSCardInfoPtr pSSCardInfo = make_shared<SSCardInfo>();
	QString strAppPath = QCoreApplication::applicationDirPath();
	strAppPath += "/Debug";
	strAppPath += QString("/Carddata_%1.ini").arg((const char*)pIDCard->szIdentify);
	QString strMessage;

	QFileInfo ffile(strAppPath);
	if (!ffile.isFile())
	{
		strMesssage = "制卡数据不存在，可能制卡流程尚未开始!";
		return -1;
	}

	LoadSSCardData(pSSCardInfo, strAppPath);
	strcpy(pSSCardInfo->strName, (const char*)pIDCard->szName);
	GetImageStorePath(g_pDataCenter->strSSCardPhotoFile, 1);
	//SaveSSCardPhoto(strMessage, pSSCardInfo->strPhoto);
	g_pDataCenter->SetSSCardInfo(pSSCardInfo);
	return 0;
}

void Sys_ManualMakeCard::on_pushButton_LoadCardData_clicked()
{
    QString strMessage;

	if (QFailed(LoadPersonSSCardData(strMessage)))
	{
		QMessageBox_CN(QMessageBox::Information, tr("提示"), strMessage, QMessageBox::Ok, this);
		return;
	}
    SSCardInfoPtr pSSCardInfo = g_pDataCenter->GetSSCardInfo();
	QString strStyle = QString("border-image: url(%1);").arg(g_pDataCenter->strSSCardPhotoFile.c_str());
	ui->label_Pohoto->setStyleSheet(strStyle);
	ui->lineEdit_CardID->setText(pSSCardInfo->strCardID);
	ui->lineEdit_Name->setText(QString::fromLocal8Bit((const char*)pSSCardInfo->strName));
	ui->lineEditl_SSCard->setText(pSSCardInfo->strCardNum);
	ui->lineEdit_Datetime->setText(pSSCardInfo->strReleaseDate);
	string strBankName;
	g_pDataCenter->GetBankName(pSSCardInfo->strBankCode, strBankName);

	ui->lineEdit_Bank->setText(strBankName.c_str());
}

void Sys_ManualMakeCard::ProceBatchLock()
{
	int nResult = -1;
	QString strMessage;

	int nStatus = 0;
	QString strInfo;
	SSCardInfoPtr& pSSCardInfo = g_pDataCenter->GetSSCardInfo();

	do
	{
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
		nResult = CancelMarkCard(strMessage, nStatus, pSSCardInfo);
		if (QFailed(nResult = MarkCard(strMessage, nStatus, pSSCardInfo)))
		{
			break;
		}
		if (QFailed(nResult = GetCardData(strMessage, nStatus, pSSCardInfo)))
		{
			if (strMessage == "批次确认加锁失败(保存失败)")
				strMessage += ",需社保局后台更新数据,请在2小时后再尝试制卡!";

			break;
			/*if (QFailed(nResult = CancelCardReplacement(strMessage, nStatus)))
				break;*/
		}
		QString strStyle = QString("border-image: url(%1);").arg(g_pDataCenter->strSSCardPhotoFile.c_str());
		ui->label_Pohoto->setStyleSheet(strStyle);
		ui->lineEdit_CardID->setText(pSSCardInfo->strCardID);
		ui->lineEdit_Name->setText(QString::fromLocal8Bit((const char*)pSSCardInfo->strName));
		ui->lineEditl_SSCard->setText(pSSCardInfo->strCardNum);
		ui->lineEdit_Datetime->setText(pSSCardInfo->strReleaseDate);
		string strBankName;
		g_pDataCenter->GetBankName(pSSCardInfo->strBankCode, strBankName);

		ui->lineEdit_Bank->setText(strBankName.c_str());
		if (QFailed(g_pDataCenter->Depense(strMessage)))
			break;
		strInfo = "进卡成功";
		gInfo() << gQStr(strInfo);
		if (QFailed(g_pDataCenter->WriteCard(pSSCardInfo, strMessage)))
			break;
		strInfo = "写卡成功";
		gInfo() << gQStr(strInfo);
		if (QFailed(g_pDataCenter->PrintCard(pSSCardInfo, "", strMessage)))
			break;
		strInfo = "卡片打印成功";
		gInfo() << gQStr(strInfo);
		nResult = 0;
	} while (0);

	if (QFailed(nResult))
	{
		gInfo() << gQStr(strMessage);
		do
		{
			gInfo() << "Try to CancelMarkCard";
			if (QFailed(nResult = CancelMarkCard(strMessage, nStatus, pSSCardInfo)))
			{
				strMessage = QString("取消标注失败:%1").arg(strMessage);
				gInfo() << gQStr(strMessage);
				break;
			}

		} while (0);

		gError() << strMessage.toLocal8Bit().data();
		QMessageBox_CN(QMessageBox::Information, tr("提示"), strMessage, QMessageBox::Ok, this);
		return;
	}
	gInfo() << gQStr(QString("写卡，打印成功"));
	nResult = -1;
	do
	{
		// 数据回盘
		if (QFailed(nResult = ReturnCardData(strMessage, nStatus, pSSCardInfo, false)))
		{
			gError() << strMessage.toLocal8Bit().data();
			break;
		}
#pragma Warning("回盘失败如何处理？")
		if (nStatus != 0 && nStatus != 1)
			break;

		// 启用
		if (QFailed(nResult = EnalbeCard(strMessage, nStatus, pSSCardInfo)))
		{
			gError() << strMessage.toLocal8Bit().data();
			break;
		}
		if (nStatus != 0 && nStatus != 1)
			break;
#pragma Warning("启用卡片失败如何处理？")
		char* szResCode[128] = { 0 };
		if (QFailed(g_pDataCenter->GetPrinter()->Printer_Eject((char*)szResCode)))
		{
			strMessage = "出卡失败,稍后请转入设备管理页面手动出卡!";
			break;
		}
		nResult = 0;
	} while (0);

	QMessageBox_CN(QMessageBox::Information, tr("提示"), strMessage, QMessageBox::Ok, this);
}

void Sys_ManualMakeCard::ProcessPowerOnFailed()
{
	int nResult = -1;
	QString strMessage;

	int nStatus = 0;
	QString strInfo;
	SSCardInfoPtr pSSCardInfo;

	do
	{
		if (QFailed(LoadPersonSSCardData(strMessage)))
		{
			break;
		}
		pSSCardInfo = g_pDataCenter->GetSSCardInfo();

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
			break;
		strInfo = "卡片打印成功";
		gInfo() << gQStr(strInfo);
		nResult = 0;
	} while (0);

	if (QFailed(nResult))
	{
		gInfo() << gQStr(strMessage);
		do
		{
			gInfo() << "Try to CancelMarkCard";
			if (QFailed(nResult = CancelMarkCard(strMessage, nStatus, pSSCardInfo)))
			{
				strMessage = QString("取消标注失败:%1").arg(strMessage);
				gInfo() << gQStr(strMessage);
				break;
			}

		} while (0);

		gError() << strMessage.toLocal8Bit().data();
		QMessageBox_CN(QMessageBox::Information, tr("提示"), strMessage, QMessageBox::Ok, this);
		return;
	}
	gInfo() << gQStr(QString("写卡，打印成功"));
	nResult = -1;
	do
	{
		// 数据回盘
		if (QFailed(nResult = ReturnCardData(strMessage, nStatus, pSSCardInfo, false)))
		{
			gError() << strMessage.toLocal8Bit().data();
			break;
		}
#pragma Warning("回盘失败如何处理？")
		if (nStatus != 0 && nStatus != 1)
			break;

		// 启用
		if (QFailed(nResult = EnalbeCard(strMessage, nStatus, pSSCardInfo)))
		{
			gError() << strMessage.toLocal8Bit().data();
			break;
		}
		if (nStatus != 0 && nStatus != 1)
			break;
#pragma Warning("启用卡片失败如何处理？")
		char* szResCode[128] = { 0 };
		if (QFailed(g_pDataCenter->GetPrinter()->Printer_Eject((char *)szResCode)))
		{
			strMessage = "出卡失败,稍后请转入设备管理页面手动出卡!";
			break;
		}
		nResult = 0;
	} while (0);
	
	QMessageBox_CN(QMessageBox::Information, tr("提示"), strMessage, QMessageBox::Ok, this);
}

void Sys_ManualMakeCard::on_pushButton_MakeCard_clicked()
{
	QString strMessage;

	SSCardInfoPtr& pSSCardInfo = g_pDataCenter->GetSSCardInfo();
	RegionInfo& Reginfo = g_pDataCenter->GetSysConfigure()->Region;
	strcpy((char*)pSSCardInfo->strOrganID, Reginfo.strAgency.c_str());
	strcpy((char*)pSSCardInfo->strOrganID, Reginfo.strAgency.c_str());
	strcpy((char*)pSSCardInfo->strBankCode, Reginfo.strBankCode.c_str());
	strcpy((char*)pSSCardInfo->strTransType, "5");
	strcpy((char*)pSSCardInfo->strCity, Reginfo.strCityCode.c_str());
	strcpy((char*)pSSCardInfo->strSSQX, Reginfo.strCountry.c_str());
	strcpy((char*)pSSCardInfo->strCard, Reginfo.strCardVendor.c_str());
	strcpy((char*)pSSCardInfo->strBankCode, Reginfo.strBankCode.c_str());
	int nIndex = ui->comboBox_Failure->currentIndex();
	switch (nIndex)
	{
	case 0:     // 上电失败
		ProcessPowerOnFailed();
		break;
	case 1:
		ProceBatchLock();
		break;
	case 2:
	{
		int nResult = 0;
		int nStatus = 0;
		if (QFailed(LoadPersonSSCardData(strMessage)))
		{
			break;
		}

		//SSCardInfoPtr& pSSCardInfo = g_pDataCenter->GetSSCardInfo();
		QString strCardProgress = QString::fromLocal8Bit(pSSCardInfo->strCardStatus);
		if (QFailed(nResult = ReturnCardData(strMessage, nStatus, pSSCardInfo, true)))
		{
			gError() << strMessage.toLocal8Bit().data();
			break;
		}
		break;
	}

	}
}
