#include "Sys_batchmakecard.h"
#include "ui_Sys_batchmakecard.h"

#include "Gloabal.h"
#include "ComboDelegate.h"

#include "Payment.h"
#include "sys_dialogreadidcard.h"
#include "mainwindow.h"
#include <QApplication>

enum class BatchTable_Column
{
	Col_No = 0,
	Col_Name,
	Col_Identity,
	Col_Gender,
	Col_Nation,
	Col_Mobile,
	Col_Career,
	Col_ServiceType,
	Col_SSCardNum,
	Col_Progress
};

int nColumnWidthList[] =
{
	40,
	110,
	240,
	40,
	200,
	230,
	300,
	150,
	180,
	250
};
QStringList ListServiceType = { "新办","补换" };
Sys_BatchMakeCard::Sys_BatchMakeCard(QWidget* parent) :
	QWidget(parent),
	ui(new Ui::Sys_BatchMakeCard)
{
	ui->setupUi(this);

	QStringList strCareerList;
	for (auto& var : vecCareer)
		strCareerList << var.strName;

	ComboBoxDelegate* pDelegateCareer = new ComboBoxDelegate(strCareerList);
	ui->tableWidget->setItemDelegateForColumn((int)BatchTable_Column::Col_Career, pDelegateCareer);
	//connect(pDelegateCareer, &ComboBoxDelegate::ModelIndexChanged, this, &Sys_BatchMakeCard::on_ComboxoDeledateModelIndexChanged);

	ComboBoxDelegate* pDelegateServiceType = new ComboBoxDelegate(ListServiceType);
	ui->tableWidget->setItemDelegateForColumn((int)BatchTable_Column::Col_ServiceType, pDelegateServiceType);
	//connect(pDelegateServiceType, &ComboBoxDelegate::ModelIndexChanged, this, &Sys_BatchMakeCard::on_ComboxoDeledateModelIndexChanged);

	for (int i = 0; i < sizeof(nColumnWidthList) / sizeof(int); i++)
		ui->tableWidget->setColumnWidth(i, nColumnWidthList[i]);
	//ui->tableWidget->setColumnWidth((int)BatchTable_Column::Col_Career, 360);
	ui->tableWidget->setRowHeight(0, 60);

	ui->tableWidget->horizontalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents);

	ui->tableWidget->horizontalHeader()->resizeSections(QHeaderView::ResizeToContents);

	ui->tableWidget->horizontalHeader()->setSectionResizeMode(QHeaderView::Interactive);

	pButttonGrp = new QButtonGroup(this);
	pButttonGrp->setExclusive(true);
	pButttonGrp->addButton(ui->radioButton_NewCard, 0);
	pButttonGrp->addButton(ui->radioButton_UpdateCard, 1);

	connect(this, &Sys_BatchMakeCard::ShowMessage, this, &Sys_BatchMakeCard::on_ShowMessage);
	connect(this, &Sys_BatchMakeCard::UpdateTableWidget, this, &Sys_BatchMakeCard::on_UpdateTableWidget, Qt::QueuedConnection);
	connect(this, &Sys_BatchMakeCard::AddNewIDCard, this, &Sys_BatchMakeCard::on_AddNewIDCard, Qt::QueuedConnection);
}

void Sys_BatchMakeCard::on_ShowMessage(QString strMessage)
{
	QMessageBox_CN(QMessageBox::Critical, "提示", strMessage, QMessageBox::Ok, this);
}

void Sys_BatchMakeCard::on_UpdateTableWidget(int nRow, int nCol, QString strMessage)
{
	ui->tableWidget->item(nRow, nCol)->setText(strMessage);
	QString strDateTime = QDateTime::currentDateTime().toString("HH:MM:SS");
	ui->textEdit->append(strDateTime + " " + strMessage);
	QTextCursor cursor = ui->textEdit->textCursor();
	cursor.movePosition(QTextCursor::End);
	ui->textEdit->setTextCursor(cursor);
	ui->tableWidget->horizontalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents);
	ui->tableWidget->horizontalHeader()->resizeSections(QHeaderView::ResizeToContents);
	ui->tableWidget->horizontalHeader()->setSectionResizeMode(QHeaderView::Interactive);
}

Sys_BatchMakeCard::~Sys_BatchMakeCard()
{
	delete ui;
}

//int Sys_BatchMakeCard::ReaderIDCard(IDCardInfo* pIDCard)
//{
//	int nResult = IDCard_Status::IDCard_Succeed;
//	bool bDevOpened = false;
//	do
//	{
//		if (m_strDevPort == "AUTO" || !m_strDevPort.size())
//		{
//			gInfo() << "Try to open IDCard Reader auto!" << m_strDevPort;
//			nResult = OpenReader(nullptr);
//		}
//		else
//		{
//			gInfo() << "Try to open IDCard Reader " << m_strDevPort;
//			nResult = OpenReader(m_strDevPort.c_str());
//		}
//
//		if (nResult != IDCard_Status::IDCard_Succeed)
//		{
//			break;
//		}
//		bDevOpened = true;
//		nResult = FindIDCard();
//		if (nResult != IDCard_Status::IDCard_Succeed)
//		{
//			break;
//		}
//
//		nResult = ReadIDCard(pIDCard);
//		if (nResult != IDCard_Status::IDCard_Succeed)
//		{
//			break;
//		}
//		int nNationalityCode = strtol((char*)pIDCard->szNationaltyCode, nullptr, 10);
//		if (nNationalityCode < 10)
//			sprintf_s((char*)pIDCard->szNationaltyCode, sizeof(pIDCard->szNationaltyCode), "%02d", nNationalityCode);
//
//	} while (0);
//	if (bDevOpened)
//	{
//		gInfo() << "Try to close IDCard Reader!";
//		CloseReader();
//	}
//	return nResult;
//}

void Sys_BatchMakeCard::ThreadReadIDCard()
{
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

int Sys_BatchMakeCard::BuildNewCardInfo(QString& strMessage)
{
	int nResult = -1;
	IDCardInfoPtr pIDCard = g_pDataCenter->GetIDCardInfo();
	SSCardBaseInfoPtr pSSCardInfo = g_pDataCenter->GetSSCardInfo();
	RegionInfo& Reginfo = g_pDataCenter->GetSysConfigure()->Region;

	do
	{
		if (!pIDCard)
		{
			strMessage = "身份证信息无效!";
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

		/*
		"CardID":"33333333333333",
		 "Name":"",
		 "City":"",			// 行政区域
		 "BankCode":""
		*/
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
				strMessage = QString::fromLocal8Bit(strText.c_str());
				nResult = 0;
				break;
			}
			else
			{
				strMessage = QString("查询制卡状态失败:%1").arg(QString::fromLocal8Bit(strText.c_str()));
				break;
			}
		}
		if (QFailed(pService->QueryCardInfo(strJsonIn, strJsonOut)))
		{
			CJsonObject jsonOut(strJsonOut);
			string strText;
			int nErrCode = -1;
			jsonOut.Get("Result", nErrCode);
			jsonOut.Get("Message", strText);
			strMessage = QString("查询卡信息失败:%1").arg(QString::fromLocal8Bit(strText.c_str()));
			break;
		}

		CJsonObject jsonOut(strJsonOut);
		string strBankCode, strMobile;
		if (!jsonOut.Get("CardNum", pSSCardInfo->strCardNum) ||
			!jsonOut.Get("BankCode", pSSCardInfo->strBankCode))
		{
			strMessage = "社保卡号或银行代码无效!";
			gInfo() << strJsonOut;
			break;
		}

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

		jsonIn.Clear();
		jsonIn.Add("CardID", pSSCardInfo->strIdentity);
		jsonIn.Add("Name", pSSCardInfo->strName);
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
		nResult = 0;
	} while (0);
	return nResult;
}

int Sys_BatchMakeCard::BuildUpdateCardInfo(QString& strMessage)
{
	SSCardBaseInfoPtr pSSCardInfo = make_shared<SSCardBaseInfo>();
	IDCardInfoPtr& pIDCard = g_pDataCenter->GetIDCardInfo();
	RegionInfo& Reginfo = g_pDataCenter->GetSysConfigure()->Region;
	g_pDataCenter->nCardStratus = CardStatus::Card_Unknow;
	int nResult = -1;
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
		string strJsonIn = jsonIn.ToString();
		string strJsonOut;
		if (QFailed(pService->QueryCardInfo(strJsonIn, strJsonOut)))
		{
			CJsonObject jsonOut(strJsonOut);
			string strText;
			int nErrCode = -1;
			jsonOut.Get("Result", nErrCode);
			jsonOut.Get("Message", strText);
			strMessage = QString("查询制卡信息失败:%1").arg(QString::fromLocal8Bit(strText.c_str()));
			break;
		}

		CJsonObject jsonOut(strJsonOut);
		string strSSCardNum, strBankCode, strMobile;
		int nCardStatus;
		if (!jsonOut.Get("CardNum", strSSCardNum) ||
			!jsonOut.Get("CardStatus", nCardStatus) ||
			!jsonOut.Get("BankCode", strBankCode) ||
			!jsonOut.Get("Mobile", strMobile))
		{
			strMessage = "姓名,身份证,社保卡号,社保卡状态,银行代码存在无效字段!";
			gInfo() << strJsonOut;
			break;
		}
		g_pDataCenter->nCardStratus = (CardStatus)nCardStatus;
		pSSCardInfo->strCardNum = strSSCardNum;
		pSSCardInfo->strBankCode = Reginfo.strBankCode;
		pSSCardInfo->strOrganID = Reginfo.strAgency;
		pSSCardInfo->strTransType = "5";
		pSSCardInfo->strCity = Reginfo.strCityCode;
		pSSCardInfo->strSSQX = Reginfo.strCountry;
		pSSCardInfo->strCardVender = Reginfo.strCardVendor;
		pSSCardInfo->strName = (const char*)pIDCard->szName;
		pSSCardInfo->strGender = (const char*)pIDCard->szGender;
		pSSCardInfo->strNation = (const char*)pIDCard->szNationalty;
		pSSCardInfo->strNationCode = (const char*)pIDCard->szNationaltyCode;
		pSSCardInfo->strBirthday = (const char*)pIDCard->szBirthday;
		pSSCardInfo->strIdentity = (const char*)pIDCard->szIdentity;
		pSSCardInfo->strAddress = (const char*)pIDCard->szAddress;

		jsonIn.Clear();
		jsonIn.Add("CardID", pSSCardInfo->strIdentity);
		jsonIn.Add("Name", pSSCardInfo->strName);
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
		nResult = 0;
	} while (0);
	return nResult;
}

void Sys_BatchMakeCard::ThreadBatchMakeCard()
{
	QString strMessage;
	int nRow = 0;
	int nDelay = 250;
	char szRCode[128] = { 0 };
	bool bTerminate = false;
	for (auto var : vecMakeCardInfo)
	{
		if (var->bFinished)
			continue;
		if (!m_bWorkThreadRunning)
			break;
		g_pDataCenter->SetIDCardInfo(var->pIDCard);
		g_pDataCenter->SetSSCardInfo(var->pSSCardInfo);
		int nResult = -1;
		g_pDataCenter->bGuardian = false;
		do
		{
			if (QFailed(nResult = g_pDataCenter->TestPrinter(strMessage)))
			{
				bTerminate = true;
				break;
			}

			if (QFailed(nResult = g_pDataCenter->TestCard(strMessage)))
			{
				bTerminate = true;
				break;
			}
			g_pDataCenter->nCardServiceType = var->nServiceType;
			if (QFailed(nResult = BuildNewCardInfo(strMessage)))
				break;
			emit UpdateTableWidget(nRow, (int)BatchTable_Column::Col_Progress, "信息生成");
			this_thread::sleep_for(chrono::milliseconds(nDelay));
			if (var->nServiceType == ServiceType::Service_ReplaceCard)
			{
				if (QFailed(nResult = g_pDataCenter->RegisterLost(strMessage)))
					break;
				emit UpdateTableWidget(nRow, (int)BatchTable_Column::Col_Progress, "注销原卡号");
				this_thread::sleep_for(chrono::milliseconds(nDelay));
			}

			if (QFailed(nResult = g_pDataCenter->CommitPersionInfo(strMessage)))
				break;
			emit UpdateTableWidget(nRow, (int)BatchTable_Column::Col_Progress, "提交信息");
			this_thread::sleep_for(chrono::milliseconds(nDelay));

			if (QFailed(nResult = g_pDataCenter->PremakeCard(strMessage)))
				break;
			emit UpdateTableWidget(nRow, (int)BatchTable_Column::Col_Progress, "预制卡");
			emit UpdateTableWidget(nRow, (int)BatchTable_Column::Col_SSCardNum, var->pSSCardInfo->strCardNum.c_str());

			if (QFailed(nResult = g_pDataCenter->Depense(strMessage)))
				break;

			emit UpdateTableWidget(nRow, (int)BatchTable_Column::Col_Progress, "移卡");
			this_thread::sleep_for(chrono::milliseconds(nDelay));


			if (QFailed(nResult = g_pDataCenter->SafeReadCard(strMessage)))
				break;
			emit UpdateTableWidget(nRow, (int)BatchTable_Column::Col_Progress, "读卡");
			this_thread::sleep_for(chrono::milliseconds(nDelay));

			if (!g_pDataCenter->bSkipWriteCard)
			{
				if (QFailed(nResult = g_pDataCenter->SafeWriteCard(strMessage)))
					break;
				emit UpdateTableWidget(nRow, (int)BatchTable_Column::Col_Progress, "写卡");
			}
			else
			{
				emit UpdateTableWidget(nRow, (int)BatchTable_Column::Col_Progress, "根据配置,跳过写卡");
			}

			this_thread::sleep_for(chrono::milliseconds(nDelay));
			if (!g_pDataCenter->bSkipPrintCard)
			{
				if (QFailed(nResult = g_pDataCenter->PrintCard(var->pSSCardInfo, "", strMessage)))
					break;
				emit UpdateTableWidget(nRow, (int)BatchTable_Column::Col_Progress, "打印");
			}
			else
			{
				emit UpdateTableWidget(nRow, (int)BatchTable_Column::Col_Progress, "根据配置,跳过打印");
			}

			this_thread::sleep_for(chrono::milliseconds(nDelay));

			if (QFailed(nResult = g_pDataCenter->EnsureData(strMessage)))
				break;

			emit UpdateTableWidget(nRow, (int)BatchTable_Column::Col_Progress, "回盘");
			this_thread::sleep_for(chrono::milliseconds(nDelay));

			if (QFailed(nResult = g_pDataCenter->ActiveCard(strMessage)))
				break;

			emit UpdateTableWidget(nRow, (int)BatchTable_Column::Col_Progress, "激活");
			this_thread::sleep_for(chrono::milliseconds(nDelay));

			g_pDataCenter->GetPrinter()->Printer_Retract(1, (char*)szRCode);
			emit UpdateTableWidget(nRow, (int)BatchTable_Column::Col_Progress, "已出卡");
			this_thread::sleep_for(chrono::milliseconds(nDelay));
			var->bFinished = true;

		} while (0);
		if (bTerminate)
		{
			emit ShowMessage(strMessage);
			break;
		}
		else
		{
			if (QFailed(nResult))
			{
				emit UpdateTableWidget(nRow, (int)BatchTable_Column::Col_Progress, strMessage);
				this_thread::sleep_for(chrono::milliseconds(nDelay));
			}
			nRow++;
		}
	}
	ui->pushButton_StopMakeCard->emit QPushButton::clicked();
}

void Sys_BatchMakeCard::on_pushButton_StartReadIDCard_clicked()
{
	if (pButttonGrp->checkedId() == -1)
	{
		QMessageBox_CN(QMessageBox::Information, "提示", "请选择批量制卡的业务类型!", QMessageBox::Ok, this);
		return;
	}
	m_bWorkThreadRunning = true;
	m_pWorkThread = new std::thread(&Sys_BatchMakeCard::ThreadReadIDCard, this);
	Sys_DialogReadIDCard dlg("请刷身份证以添加制卡人信息", true, this);
	dlg.exec();
	m_bWorkThreadRunning = false;
	m_pWorkThread->join();
	delete m_pWorkThread;
	m_pWorkThread = nullptr;
}

void Sys_BatchMakeCard::Reset()
{
	int nRowCount = ui->tableWidget->rowCount();
	//if (nRowCount > 0)
	{
		for (int nRow = nRowCount - 1; nRow >= 0; nRow--)
		{
			ui->tableWidget->removeRow(nRow);
		}
	}

	ui->textEdit->clear();
	vecMakeCardInfo.clear();
	ui->radioButton_NewCard->setChecked(false);
	ui->radioButton_UpdateCard->setChecked(false);
}

int	Sys_BatchMakeCard::ImportNewIDCard(vector<QString>& vecField)
{
	IDCardInfoPtr pIDCard = make_shared<IDCardInfo>();
	strcpy((char*)pIDCard->szName, vecField[(int)BatchTable_Column::Col_Name].toLocal8Bit().data());
	strcpy((char*)pIDCard->szIdentity, vecField[(int)BatchTable_Column::Col_Identity].toStdString().c_str());
	strcpy((char*)pIDCard->szGender, vecField[(int)BatchTable_Column::Col_Gender].toLocal8Bit().data());
	strcpy((char*)pIDCard->szNationalty, vecField[(int)BatchTable_Column::Col_Nation].toLocal8Bit().data());

	auto var = find_if(vecMakeCardInfo.begin(), vecMakeCardInfo.end(), [pIDCard](MakeCardInfoPtr pItem)
		{
			return strcmp((char*)pItem->pIDCard->szIdentity, (char*)pIDCard->szIdentity) == 0;
		});
	if (var != vecMakeCardInfo.end())
		return -1;
	int nItems = vecMakeCardInfo.size();
	vecMakeCardInfo.push_back(make_shared<MakeCardInfo>(pIDCard));

	ui->tableWidget->insertRow(nItems);
	ui->tableWidget->setItem(nItems, (int)BatchTable_Column::Col_No, new QTableWidgetItem(QString("%1").arg(nItems + 1)));
	ui->tableWidget->setItem(nItems, (int)BatchTable_Column::Col_Name, new QTableWidgetItem(QString::fromLocal8Bit((char*)pIDCard->szName)));
	ui->tableWidget->setItem(nItems, (int)BatchTable_Column::Col_Identity, new QTableWidgetItem(QString("%1").arg((char*)pIDCard->szIdentity)));
	ui->tableWidget->setItem(nItems, (int)BatchTable_Column::Col_Gender, new QTableWidgetItem(QString::fromLocal8Bit((char*)pIDCard->szGender)));
	ui->tableWidget->setItem(nItems, (int)BatchTable_Column::Col_Nation, new QTableWidgetItem(QString::fromLocal8Bit((char*)pIDCard->szNationalty)));
	ui->tableWidget->setItem(nItems, (int)BatchTable_Column::Col_Mobile, new QTableWidgetItem(""));
	ui->tableWidget->setItem(nItems, (int)BatchTable_Column::Col_Mobile, new QTableWidgetItem(vecField[(int)BatchTable_Column::Col_Mobile]));
	ui->tableWidget->setItem(nItems, (int)BatchTable_Column::Col_Career, new QTableWidgetItem(vecField[(int)BatchTable_Column::Col_Career]));
	ui->tableWidget->setItem(nItems, (int)BatchTable_Column::Col_ServiceType, new QTableWidgetItem(vecField[(int)BatchTable_Column::Col_ServiceType]));
	ui->tableWidget->setItem(nItems, (int)BatchTable_Column::Col_SSCardNum, new QTableWidgetItem(""));
	ui->tableWidget->setItem(nItems, (int)BatchTable_Column::Col_Progress, new QTableWidgetItem(""));

	ui->tableWidget->horizontalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents);
	ui->tableWidget->horizontalHeader()->resizeSections(QHeaderView::ResizeToContents);
	ui->tableWidget->horizontalHeader()->setSectionResizeMode(QHeaderView::Interactive);
	ui->tableWidget->setColumnWidth((int)BatchTable_Column::Col_Career, nColumnWidthList[(int)BatchTable_Column::Col_Career]);
	ui->tableWidget->setColumnWidth((int)BatchTable_Column::Col_Mobile, nColumnWidthList[(int)BatchTable_Column::Col_Mobile]);
	return 0;
}

void Sys_BatchMakeCard::on_pushButton_StartMakeCard_clicked()
{
	QString strMessage;
	if (!g_pDataCenter->GetPrinter())
	{
		if (QFailed(g_pDataCenter->OpenPrinter(strMessage)))
		{
			gError() << gQStr(strMessage);
			QMessageBox_CN(QMessageBox::Critical, "操作失败", strMessage, QMessageBox::Ok, this);
			return;
		}
	}
	if (!g_pDataCenter->GetSSCardReader())
	{
		if (QFailed(g_pDataCenter->OpenSSCardReader(strMessage)))
		{
			gError() << gQStr(strMessage);
			QMessageBox_CN(QMessageBox::Critical, "操作失败", strMessage, QMessageBox::Ok, this);
			return;
		}
	}

	if (QFailed(g_pDataCenter->TestPrinter(strMessage)))
	{
		QMessageBox_CN(QMessageBox::Critical, "操作失败", strMessage, QMessageBox::Ok, this);
		return;
	}

	if (m_pWorkThread)
	{
		m_bWorkThreadRunning = false;
		m_pWorkThread->join();
		delete m_pWorkThread;
	}

	bool bFault = false;
	int nCount = ui->tableWidget->rowCount();
	for (int nRow = 0; nRow < nCount; nRow++)
	{
		QString strOccupation = ui->tableWidget->item(nRow, (int)BatchTable_Column::Col_Career)->text();
		QString strServiceType = ui->tableWidget->item(nRow, (int)BatchTable_Column::Col_ServiceType)->text();
		QString strMobile = ui->tableWidget->item(nRow, (int)BatchTable_Column::Col_Mobile)->text();
		if (strOccupation.isEmpty() || strServiceType.isEmpty() || strMobile.isEmpty())
		{
			bFault = true;
			strMessage = QString("'%1'的职业,业务类型或手机号码未选择/输入!").arg(QString::fromLocal8Bit((char*)vecMakeCardInfo[nRow]->pIDCard->szName));
			QMessageBox_CN(QMessageBox::Critical, "提示", strMessage, QMessageBox::Ok, this);
			break;
		}
		for (auto var : vecCareer)
		{
			if (strOccupation == var.strName)
				vecMakeCardInfo[nRow]->pSSCardInfo->strOccupType = var.strCode.toStdString();
		}

		for (int nIndex = 0; nIndex < ListServiceType.size(); nIndex++)
		{
			if (ListServiceType[nIndex] == strServiceType)
			{
				vecMakeCardInfo[nRow]->nServiceType = (ServiceType)nIndex;
			}
		}
		vecMakeCardInfo[nRow]->pSSCardInfo->strMobile = strMobile.toStdString();
	}
	if (bFault)
		return;

	m_bWorkThreadRunning = true;
	m_pWorkThread = new std::thread(&Sys_BatchMakeCard::ThreadBatchMakeCard, this);
	if (m_pWorkThread)
	{
		ui->pushButton_StartReadIDCard->setEnabled(false);
		ui->pushButton_ImportList->setEnabled(false);
		ui->pushButton_StartMakeCard->setEnabled(false);
		ui->pushButton_ExportList->setEnabled(false);
		ui->pushButton_StartMakeCard->setEnabled(false);
		ui->pushButton_StopMakeCard->setEnabled(true);
		ui->pushButton_MainPage->setEnabled(false);
	}
}

void Sys_BatchMakeCard::on_pushButton_StopMakeCard_clicked()
{
	QWaitCursor Wait;
	if (m_pWorkThread)
	{
		m_bWorkThreadRunning = false;
		m_pWorkThread->join();
		delete m_pWorkThread;
		m_pWorkThread = nullptr;
	}
	ui->pushButton_StartReadIDCard->setEnabled(true);
	ui->pushButton_ImportList->setEnabled(true);
	ui->pushButton_StartMakeCard->setEnabled(true);
	ui->pushButton_ExportList->setEnabled(true);
	ui->pushButton_StartMakeCard->setEnabled(true);
	ui->pushButton_StopMakeCard->setEnabled(false);
	ui->pushButton_MainPage->setEnabled(true);
}

void Sys_BatchMakeCard::on_pushButton_ImportList_clicked()
{
	//int nCols = ui->tableWidget->columnCount();
	//for (int i = 0; i < nCols; i++)
	//{
	//	qDebug() << "Column[" << i << "] Width = " << ui->tableWidget->columnWidth(i);
	//}
	QString strXlsxFile = QFileDialog::getOpenFileName(this, tr("导入批量制卡名单"), ".", tr("Excel表格文件(*.xlsx)"));
	if (strXlsxFile.isEmpty())
		return;
	QXlsx::Document xlsx(strXlsxFile);
	xlsx.selectSheet("Sheet1");
	QString strTitle[] = { "姓名","身份证","性别","民族","手机","职业","业务类型" };
	bool bValid = true;
	for (char Col = 'A'; Col <= 'G'; Col++)
	{
		QString strCell = Col;
		strCell += '1';
		QString strValue = xlsx.read(strCell).toString();
		if (strValue != strTitle[Col - 'A'])
		{
			bValid = false;
			break;
		}
	}
	if (!bValid)
	{
		QMessageBox_CN(QMessageBox::Critical, "提示", "所选制卡名单无效,请先择符合要求的名单文件!", QMessageBox::Ok, this);
		return;
	}
	int nRow = 2;
	bool bEmpty = false;
	do
	{
		vector<QString> vecField;
		vecField.push_back("");
		for (char Col = 'A'; Col <= 'G'; Col++)
		{
			QString strCell = QString("%1%2").arg(Col).arg(nRow);
			QString strValue = xlsx.read(strCell).toString();
			if (strValue.isEmpty())
			{
				bEmpty = true;
				break;
			}
			vecField.push_back(strValue);
		}
		nRow++;
		if (vecField.size() == 8)
			ImportNewIDCard(vecField);

	} while (!bEmpty);
}

void Sys_BatchMakeCard::on_pushButton_ExportList_clicked()
{
	int nRowCount = ui->tableWidget->rowCount();
	int nColCount = ui->tableWidget->columnCount();
	if (nRowCount == 0)
	{
		QMessageBox_CN(QMessageBox::Information, "提示", "没有可以导出的制卡名单!", QMessageBox::Ok, this);
		return;
	}
	QString strXlsxFile = QFileDialog::getSaveFileName(this, tr("保存批量制卡名单"), "制卡名单", tr("Excel表格文件(*.xlsx)"));
	if (strXlsxFile.isEmpty())
		return;
	QXlsx::Document xlsx(strXlsxFile);
	QString strTitle[] = { "姓名","身份证","性别","民族","手机","职业","业务类型","卡号","状态" };
	for (int i = 0; i < sizeof(strTitle) / sizeof(QString); i++)
	{
		QString strCell = 'A' + i;
		strCell += '1';
		xlsx.write(strCell, strTitle[i]);
	}
	for (int nRow = 0; nRow < nRowCount; nRow++)
	{
		for (int nCol = 1; nCol < nColCount; nCol++)
		{
			QString strCell = 'A' + nCol - 1;
			strCell += to_string(nRow + 2).c_str();
			xlsx.write(strCell, ui->tableWidget->item(nRow, nCol)->text());
		}
	}
	xlsx.save();
}

void Sys_BatchMakeCard::on_AddNewIDCard(IDCardInfo* pIDCard)
{
	shared_ptr<IDCardInfo> pIDCardPtr(pIDCard);
	//auto [it, Inserted] = m_MapIDCardInfo.try_emplace(string((char*)pIDCard->szIdentity), pIDCardPtr);
	//if (!Inserted)
	//	return;
	if (!g_pDataCenter->bDebug)
	{
		auto var = find_if(vecMakeCardInfo.begin(), vecMakeCardInfo.end(), [pIDCard](MakeCardInfoPtr pItem)
			{
				return strcmp((char*)pItem->pIDCard->szIdentity, (char*)pIDCard->szIdentity) == 0;
			});

		if (var != vecMakeCardInfo.end())
			return;
	}

	int nItems = vecMakeCardInfo.size();
	vecMakeCardInfo.push_back(make_shared<MakeCardInfo>(pIDCardPtr));

	ui->tableWidget->insertRow(nItems);
	ui->tableWidget->setItem(nItems, (int)BatchTable_Column::Col_No, new QTableWidgetItem(QString("%1").arg(nItems + 1)));
	ui->tableWidget->setItem(nItems, (int)BatchTable_Column::Col_Name, new QTableWidgetItem(QString::fromLocal8Bit((char*)pIDCard->szName)));
	ui->tableWidget->setItem(nItems, (int)BatchTable_Column::Col_Identity, new QTableWidgetItem(QString("%1").arg((char*)pIDCard->szIdentity)));
	ui->tableWidget->setItem(nItems, (int)BatchTable_Column::Col_Gender, new QTableWidgetItem(QString::fromLocal8Bit((char*)pIDCard->szGender)));
	ui->tableWidget->setItem(nItems, (int)BatchTable_Column::Col_Nation, new QTableWidgetItem(QString::fromLocal8Bit((char*)pIDCard->szNationalty)));
	ui->tableWidget->setItem(nItems, (int)BatchTable_Column::Col_Mobile, new QTableWidgetItem(""));
	int nServiceType = pButttonGrp->checkedId();
	ui->tableWidget->setItem(nItems, (int)BatchTable_Column::Col_Career, new QTableWidgetItem("其他从业人员"));
	ui->tableWidget->setItem(nItems, (int)BatchTable_Column::Col_ServiceType, new QTableWidgetItem(ListServiceType[nServiceType]));
	ui->tableWidget->setItem(nItems, (int)BatchTable_Column::Col_SSCardNum, new QTableWidgetItem(""));
	ui->tableWidget->setItem(nItems, (int)BatchTable_Column::Col_Progress, new QTableWidgetItem(""));

	ui->tableWidget->horizontalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents);
	ui->tableWidget->horizontalHeader()->resizeSections(QHeaderView::ResizeToContents);
	ui->tableWidget->horizontalHeader()->setSectionResizeMode(QHeaderView::Interactive);
	ui->tableWidget->setColumnWidth((int)BatchTable_Column::Col_No, nColumnWidthList[(int)BatchTable_Column::Col_No]);
	ui->tableWidget->setColumnWidth((int)BatchTable_Column::Col_Gender, nColumnWidthList[(int)BatchTable_Column::Col_Gender]);
	ui->tableWidget->setColumnWidth((int)BatchTable_Column::Col_Mobile, nColumnWidthList[(int)BatchTable_Column::Col_Mobile]);
	ui->tableWidget->setColumnWidth((int)BatchTable_Column::Col_Career, nColumnWidthList[(int)BatchTable_Column::Col_Career]);
	ui->tableWidget->setColumnWidth((int)BatchTable_Column::Col_Mobile, nColumnWidthList[(int)BatchTable_Column::Col_Mobile]);
}

void Sys_BatchMakeCard::on_pushButton_MainPage_clicked()
{
	bool bReturnMain = true;

	if (vecMakeCardInfo.size())
	{
		int nRes = QMessageBox_CN(QMessageBox::Information, "提示", "返回主页将会放弃制卡列表中的数据,是否继续?", QMessageBox::Yes | QMessageBox::No, this);
		if (nRes == QMessageBox::No)
		{
			bReturnMain = false;
		}
	}
	if (bReturnMain)
	{
		MainWindow* pMainWind = (MainWindow*)qApp->activeWindow();
		pMainWind->on_pushButton_MainPage_clicked();
	}
}
