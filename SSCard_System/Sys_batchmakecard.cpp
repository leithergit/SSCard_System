#include "Sys_batchmakecard.h"
#include "ui_Sys_batchmakecard.h"
#include "ComboDelegate.h"
#include "Gloabal.h"

#include "sys_dialogreadidcard.h"

enum class BatchTable_Column
{
	Col_No = 0,
	Col_Name,
	Col_Identity,
	Col_Gender,
	Col_Nation,
	Col_Career,
	Col_ServiceType,
	Col_SSCardNum,
	Col_Progress
};

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

	QStringList strServiceType = { "新制卡","补办卡" };

	ComboBoxDelegate* pDelegateServiceType = new ComboBoxDelegate(strServiceType);
	ui->tableWidget->setItemDelegateForColumn((int)BatchTable_Column::Col_ServiceType, pDelegateServiceType);
	int nWidthList[] =
	{
		80,
		120,
		240,
		60,
		200,
		200,
		150,
		180,
		150
	};
	for (int i = 0; i < sizeof(nWidthList) / sizeof(int); i++)
		ui->tableWidget->setColumnWidth(i, nWidthList[i]);
	ui->tableWidget->setRowHeight(0, 60);

	ui->tableWidget->horizontalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents);

	ui->tableWidget->horizontalHeader()->resizeSections(QHeaderView::ResizeToContents);

	ui->tableWidget->horizontalHeader()->setSectionResizeMode(QHeaderView::Interactive);
	SysConfigPtr& pConfigure = g_pDataCenter->GetSysConfigure();
	m_strDevPort = pConfigure->DevConfig.strIDCardReaderPort;
	transform(m_strDevPort.begin(), m_strDevPort.end(), m_strDevPort.begin(), ::toupper);
	pButttonGrp = new QButtonGroup(this);
	pButttonGrp->setExclusive(true);
	pButttonGrp->addButton(ui->radioButton_NewCard, 0);
	pButttonGrp->addButton(ui->radioButton_UpdateCard, 1);
	connect(this, &Sys_BatchMakeCard::AddNewIDCard, this, &Sys_BatchMakeCard::on_AddNewIDCard);
}

Sys_BatchMakeCard::~Sys_BatchMakeCard()
{
	delete ui;
}

int Sys_BatchMakeCard::ReaderIDCard(IDCardInfo* pIDCard)
{
	int nResult = IDCard_Status::IDCard_Succeed;
	bool bDevOpened = false;
	do
	{
		if (m_strDevPort == "AUTO" || !m_strDevPort.size())
		{
			gInfo() << "Try to open IDCard Reader auto!" << m_strDevPort;
			nResult = OpenReader(nullptr);
		}
		else
		{
			gInfo() << "Try to open IDCard Reader " << m_strDevPort;
			nResult = OpenReader(m_strDevPort.c_str());
		}

		if (nResult != IDCard_Status::IDCard_Succeed)
		{
			break;
		}
		bDevOpened = true;
		nResult = FindIDCard();
		if (nResult != IDCard_Status::IDCard_Succeed)
		{
			break;
		}

		nResult = ReadIDCard(*pIDCard);
		if (nResult != IDCard_Status::IDCard_Succeed)
		{
			break;
		}
		int nNationalityCode = strtol((char*)pIDCard->szNationaltyCode, nullptr, 10);
		if (nNationalityCode < 10)
			sprintf_s((char*)pIDCard->szNationaltyCode, sizeof(pIDCard->szNationaltyCode), "%02d", nNationalityCode);

	} while (0);
	if (bDevOpened)
	{
		gInfo() << "Try to close IDCard Reader!";
		CloseReader();
	}
	return nResult;
}

void Sys_BatchMakeCard::ThreadWork()
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
			int nResult = ReaderIDCard(pIDCard);
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

void Sys_BatchMakeCard::on_pushButton_StartReadIDCard_clicked()
{
	m_bWorkThreadRunning = true;
	m_pWorkThread = new std::thread(&Sys_BatchMakeCard::ThreadWork, this);
	Sys_DialogReadIDCard dlg;
	dlg.exec();
	m_bWorkThreadRunning = false;
	m_pWorkThread->join();
	delete m_pWorkThread;
	m_pWorkThread = nullptr;

}


void Sys_BatchMakeCard::on_pushButton_StopReadIDCard_clicked()
{

}


void Sys_BatchMakeCard::on_pushButton_ImportList_clicked()
{

}


void Sys_BatchMakeCard::on_pushButton_StartMakeCard_clicked()
{

}


void Sys_BatchMakeCard::on_pushButton_StopMakeCard_clicked()
{

}

void Sys_BatchMakeCard::on_pushButton__ExportList_clicked()
{

}

void Sys_BatchMakeCard::on_AddNewIDCard(IDCardInfo* pIDCard)
{
	shared_ptr<IDCardInfo> pIDCardPtr(pIDCard);
	auto [it, Inserted] = m_MapIDCardInfo.try_emplace(string((char*)pIDCard->szIdentity), pIDCardPtr);
	if (!Inserted)
		return;
	int nItems = m_MapIDCardInfo.size();
	ui->tableWidget->insertRow(nItems - 1);
	ui->tableWidget->setItem(nItems - 1, (int)BatchTable_Column::Col_No, new QTableWidgetItem(QString("%1").arg(nItems)));
	ui->tableWidget->setItem(nItems - 1, (int)BatchTable_Column::Col_Name, new QTableWidgetItem(QString::fromLocal8Bit((char*)pIDCard->szName)));
	ui->tableWidget->setItem(nItems - 1, (int)BatchTable_Column::Col_Identity, new QTableWidgetItem(QString("%1").arg((char*)pIDCard->szIdentity)));
	ui->tableWidget->setItem(nItems - 1, (int)BatchTable_Column::Col_Gender, new QTableWidgetItem(QString::fromLocal8Bit((char*)pIDCard->szGender)));
	ui->tableWidget->setItem(nItems - 1, (int)BatchTable_Column::Col_Nation, new QTableWidgetItem(QString::fromLocal8Bit((char*)pIDCard->szNationalty)));
	int nTServiceType = pButttonGrp->checkedId();
	ui->tableWidget->setItem(nItems - 1, (int)BatchTable_Column::Col_ServiceType, new QTableWidgetItem(nTServiceType == 0 ? "新办卡" : "补换卡"));
	ui->tableWidget->horizontalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents);
	ui->tableWidget->horizontalHeader()->resizeSections(QHeaderView::ResizeToContents);

}
