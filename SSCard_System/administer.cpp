#include "administer.h"
#include "ui_administer.h"
#include "sys_dialogreadidcard.h"
#include <QJsonDocument>
#include <QJsonArray>
#include <QJsonObject>
#include <QJsonValue>
#include <QJsonObject>
#include <QJsonParseError>

#include <QDebug>
#include "aestools.h"

Administer::Administer(QWidget* parent) :
	QWidget(parent),
	ui(new Ui::Administer)
{
	ui->setupUi(this);

	ui->tableWidget->setRowHeight(0, 60);
	ui->tableWidget->horizontalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents);
	ui->tableWidget->horizontalHeader()->resizeSections(QHeaderView::ResizeToContents);
	ui->tableWidget->horizontalHeader()->setSectionResizeMode(QHeaderView::Interactive);

	ui->tableWidget->setColumnWidth(0, 80);
	ui->tableWidget->setColumnWidth(1, 120);
	ui->tableWidget->setColumnWidth(2, 300);
	ui->tableWidget->setColumnWidth(3, 100);
	LoadConfigure();

	connect(this, &Administer::AddNewIDCard, this, &Administer::on_AddNewIDCard, Qt::QueuedConnection);
}

Administer::~Administer()
{
	SaveConfigure();
	//LoadConfigure();
	delete ui;
}

bool Administer::LoadConfigure()
{
	int nItems = 0;
	g_pDataCenter->LoadAdminConfigure();
	for (auto var : g_pDataCenter->GetAdminConfigure())
	{
		ui->tableWidget->insertRow(nItems);
		ui->tableWidget->setItem(nItems, 0, new QTableWidgetItem(QString("%1").arg(nItems + 1)));
		ui->tableWidget->setItem(nItems, 1, new QTableWidgetItem(QString::fromLocal8Bit((char*)var->szName)));
		ui->tableWidget->setItem(nItems, 2, new QTableWidgetItem((char*)var->szIdentity));
		ui->tableWidget->setItem(nItems, 3, new QTableWidgetItem(QString::fromLocal8Bit((char*)var->szGender)));
		nItems++;
	}
	ui->tableWidget->horizontalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents);
	ui->tableWidget->horizontalHeader()->resizeSections(QHeaderView::ResizeToContents);
	ui->tableWidget->horizontalHeader()->setSectionResizeMode(QHeaderView::Interactive);
	return true;
}

/*{
	[{
		"Name":"Test1",
		"ID":"12345567",
		"Gender":"男",
	},
	{
		"Name":"Test2",
		"ID":"12345563",
		"Gender":"男",
	}]
}*/

void Administer::SaveConfigure()
{
	int nCount = ui->tableWidget->rowCount();
	QJsonArray jsArray;
	for (int nItem = 0; nItem < nCount; nItem++)
	{
		QJsonObject jsObj;
		jsObj.insert("Name", ui->tableWidget->item(nItem, 1)->text());
		jsObj.insert("ID", ui->tableWidget->item(nItem, 2)->text());
		jsObj.insert("Gender", ui->tableWidget->item(nItem, 3)->text());
		jsArray.append(jsObj);
	}
	QJsonDocument jsonDoc;
	jsonDoc.setArray(jsArray);
	QByteArray ba = jsonDoc.toJson();

	//AesTools AesDecrypt((unsigned char*)szAesKey, 16);
	//unsigned char* pBuffer = new unsigned char[ba.size() * 2 + 1];
	//ZeroMemory(pBuffer, ba.size() * 2 + 1);

	//shared_ptr<unsigned char> pBufferFree(pBuffer);
	//int nDataLen = AesDecrypt.Encrypt((unsigned char*)ba.data(), ba.size(), pBuffer);

	QString strDocPath = qApp->applicationDirPath() + "/Data";
	QFileInfo fi(strDocPath);
	bool bCreateNewDir = true;
	if (fi.exists())
	{
		if (fi.isFile())
			QFile::remove(strDocPath);
		else
			bCreateNewDir = false;
	}
	if (bCreateNewDir)
	{
		QDir dir;
		dir.mkpath(strDocPath);
	}

	strDocPath += "/Adminster.json";
	QFile jsfile(strDocPath);

	jsfile.open(QIODevice::ReadWrite);
	jsfile.write((char*)ba.data(), ba.size());
	jsfile.close();
}

void Administer::on_pushButton_Add_clicked()
{
	m_bWorkThreadRunning = true;
	m_pWorkThread = new std::thread(&Administer::ThreadReadIDCard, this);
	Sys_DialogReadIDCard dlg("请刷身份证以添加管理员信息", true, this);
	dlg.exec();
	m_bWorkThreadRunning = false;
	m_pWorkThread->join();
	delete m_pWorkThread;
	m_pWorkThread = nullptr;
}

void Administer::on_pushButton_Del_clicked()
{
	int nRes = QMessageBox_CN(QMessageBox::Question, "提示", "是否确定删除所选管理员,删除后将无法恢复,是否继续?", QMessageBox::Yes | QMessageBox::No, this);
	if (nRes == QMessageBox::No)
		return;
	QList<QTableWidgetItem*> items = ui->tableWidget->selectedItems();
	vector<IDCardInfoPtr>& vecAdminister = g_pDataCenter->GetAdminConfigure();

	map<int, int> mapRows;
	for (auto var : items)
		auto [it, bInserted] = mapRows.try_emplace(var->row(), 0);

	// 从后往前删，不会导致行号重新调整
	for (auto it = mapRows.rbegin(); it != mapRows.rend(); it++)
	{
		vecAdminister.erase(vecAdminister.begin() + it->first);
		ui->tableWidget->removeRow(it->first);
	}
	int nRow = ui->tableWidget->rowCount();
	for (int i = 0; i < nRow; i++)
	{
		ui->tableWidget->setItem(i, 0, new QTableWidgetItem(QString("%1").arg(i + 1)));
	}
}

void Administer::ThreadReadIDCard()
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

void Administer::on_AddNewIDCard(IDCardInfo* pIDCard)
{
	shared_ptr<IDCardInfo> pIDCardPtr(pIDCard);
	vector<IDCardInfoPtr>& vecAdminister = g_pDataCenter->GetAdminConfigure();
	if (!g_pDataCenter->bDebug)
	{
		auto var = find_if(vecAdminister.begin(), vecAdminister.end(), [pIDCard](IDCardInfoPtr pItem)
			{
				return strcmp((char*)pItem->szIdentity, (char*)pIDCard->szIdentity) == 0;
			});

		if (var != vecAdminister.end())
			return;
	}

	int nItems = vecAdminister.size();
	vecAdminister.push_back(pIDCardPtr);

	ui->tableWidget->insertRow(nItems);
	ui->tableWidget->setItem(nItems, 0, new QTableWidgetItem(QString("%1").arg(nItems + 1)));
	ui->tableWidget->setItem(nItems, 1, new QTableWidgetItem(QString::fromLocal8Bit((char*)pIDCard->szName)));
	ui->tableWidget->setItem(nItems, 2, new QTableWidgetItem(QString("%1").arg((char*)pIDCard->szIdentity)));
	ui->tableWidget->setItem(nItems, 3, new QTableWidgetItem(QString::fromLocal8Bit((char*)pIDCard->szGender)));

	ui->tableWidget->horizontalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents);
	ui->tableWidget->horizontalHeader()->resizeSections(QHeaderView::ResizeToContents);
	ui->tableWidget->horizontalHeader()->setSectionResizeMode(QHeaderView::Interactive);
}
