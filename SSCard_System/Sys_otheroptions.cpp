#include "Sys_otheroptions.h"
#include "ui_Sys_otheroptions.h"


OtherOptions::OtherOptions(QWidget* parent) :
	QWidget(parent),
	ui(new Ui::OtherOptions)
{
	ui->setupUi(this);
	auto& pBankMap = g_pDataCenter->GetSysConfigure()->MapBankSupported;
	int nRows = 0;
	for (auto var : pBankMap)
	{
		ui->tableWidgetBank->insertRow(nRows);
		ui->tableWidgetBank->setItem(nRows, 0, new QTableWidgetItem(QString("%1").arg(nRows + 1)));
		ui->tableWidgetBank->setItem(nRows, 1, new QTableWidgetItem(QString("%1").arg(var.second.c_str())));
		ui->tableWidgetBank->setItem(nRows, 2, new QTableWidgetItem(QString("%1").arg(var.first.c_str())));
		nRows++;
	}
	int nRowCount = ui->tableWidgetBank->rowCount();
	for (int nItem = 0; nItem < nRowCount; nItem++)
	{
		QTableWidgetItem* pItem = ui->tableWidgetBank->item(nItem, 0);
		pItem->setFlags(pItem->flags() & (~Qt::ItemIsEditable));

		pItem = ui->tableWidgetBank->item(nItem, 1);
		pItem->setFlags(pItem->flags() & (~Qt::ItemIsEditable));
	}


	SysConfigPtr& pConfig = g_pDataCenter->GetSysConfigure();
	int nRow = 0;
	ui->tableWidget_Other->setItem(nRow++, 2, new QTableWidgetItem(pConfig->PaymentConfig.strHost.c_str()));
	ui->tableWidget_Other->setItem(nRow++, 2, new QTableWidgetItem(pConfig->PaymentConfig.strPort.c_str()));
	ui->tableWidget_Other->setItem(nRow++, 2, new QTableWidgetItem(pConfig->strDBPath.c_str()));
	ui->tableWidget_Other->setItem(nRow++, 2, new QTableWidgetItem(QString("%1").arg(pConfig->dfFaceSimilarity)));
	ui->tableWidget_Other->setItem(nRow++, 2, new QTableWidgetItem(QString("%1").arg(pConfig->nLogSavePeroid)));
	ui->tableWidget_Other->setItem(nRow++, 2, new QTableWidgetItem(QString("%1").arg(pConfig->nDiskFreeSpace)));
	ui->tableWidget_Other->setItem(nRow++, 2, new QTableWidgetItem(pConfig->strLogServer.c_str()));
	//ui->tableWidget_Other->item(7,2)->setText(QString("%1").arg(pConfig->nLogServerPort));

	m_pCheckBoxGroup = new QButtonGroup(this);
	m_pCheckBoxGroup->setExclusive(false);

	ui->tableWidget_Other->setItem(nRow, 2, new QTableWidgetItem(""));
	SetTableWidgetItemChecked(ui->tableWidget_Other, nRow++, 2, m_pCheckBoxGroup, 0, pConfig->bUpoadlog);     // 启用日志上传
	ui->tableWidget_Other->setItem(nRow, 2, new QTableWidgetItem(""));
	SetTableWidgetItemChecked(ui->tableWidget_Other, nRow++, 2, m_pCheckBoxGroup, 1, pConfig->bDeletelogUploaded); // 日志上传后删除
	ui->tableWidget_Other->setItem(nRow, 2, new QTableWidgetItem(""));
	SetTableWidgetItemChecked(ui->tableWidget_Other, nRow++, 2, m_pCheckBoxGroup, 2, pConfig->bDebug);            // 启用调试

	connect(m_pCheckBoxGroup, &QButtonGroup::idClicked, this, &OtherOptions::on_TableCheckBoxStateChanged);

	nRowCount = ui->tableWidget_Other->rowCount();
	for (int nItem = 0; nItem < nRowCount; nItem++)
	{
		QTableWidgetItem* pItem = ui->tableWidget_Other->item(nItem, 0);
		pItem->setFlags(pItem->flags() & (~Qt::ItemIsEditable));

		pItem = ui->tableWidget_Other->item(nItem, 1);
		pItem->setFlags(pItem->flags() & (~Qt::ItemIsEditable));
	}

	int nCols = ui->tableWidgetBank->columnCount();
	for (int nIndex = 0; nIndex < nCols; nIndex++)
	{
		ui->tableWidgetBank->horizontalHeader()->setSectionResizeMode(nIndex, QHeaderView::ResizeToContents);
	}
	ui->tableWidgetBank->verticalHeader()->setSectionResizeMode(QHeaderView::Stretch);

	nCols = ui->tableWidget_Other->columnCount();
	for (int nIndex = 0; nIndex < nCols; nIndex++)
	{
		ui->tableWidget_Other->horizontalHeader()->setSectionResizeMode(nIndex, QHeaderView::ResizeToContents);
	}
	ui->tableWidget_Other->verticalHeader()->setSectionResizeMode(QHeaderView::Stretch);
}

void OtherOptions::on_TableCheckBoxStateChanged(int nIndex)
{
	QCheckBox* pCheckBox = (QCheckBox*)m_pCheckBoxGroup->button(nIndex);
	bool bStatus = false;
	if (pCheckBox->checkState() == Qt::Checked)
	{
		bStatus = true;
	}
	qDebug() << "Item[" << nIndex << "]" << bStatus;
}


OtherOptions::~OtherOptions()
{
	delete ui;
}

void OtherOptions::on_pushButton_TestUploadLog_clicked()
{

}
