#pragma execution_character_set("utf-8")
#include "Sys_logmanager.h"
#include "ui_Sys_logmanager.h"
#include <QDirIterator>

enum logTable_Column
{
	logTable_No = 0,
	logTable_FileName,
	logTable_StartTime,
	//logTable_StopTime,
	logTable_Size
};

void logManager::Showlog(QStringList& strLogList)
{
	if (strLogList.size())
	{
		int nRowCount = ui->tableWidget->rowCount();
		for (int i = nRowCount - 1; i >= 0; i--)
			ui->tableWidget->removeRow(i);
		int nRow = 0;
		for (auto var : strLogList)
		{
			QFileInfo fi(var);
			if (!fi.isFile())
				continue;
			if (!fi.size())     // 0 size文件，可能文件正在被使用，忽略
				continue;
			ui->tableWidget->insertRow(nRow);
			ui->tableWidget->setItem(nRow, logTable_No, new QTableWidgetItem(QString("%1").arg(nRow + 1)));
			ui->tableWidget->setItem(nRow, logTable_StartTime, new QTableWidgetItem(fi.birthTime().toString("yyyy-MM-dd hh:mm:ss")));
			//ui->tableWidget->setItem(nRow, logTable_StopTime, new QTableWidgetItem(fi.lastModified().toString("yyyy-MM-dd hh:mm:ss")));
			QTableWidgetItem* pItem = new QTableWidgetItem(fi.fileName());
			pItem->setData(Qt::UserRole, fi.absoluteFilePath());
			ui->tableWidget->setItem(nRow, logTable_FileName, pItem);
			QString strFileSize;
			if (fi.size() < 1024)
				strFileSize = QString("%1 Bytes").arg(fi.size());
			else if (fi.size() >= 1024 && fi.size() < 1024 * 1024)
				strFileSize = QString("%1 KB").arg(fi.size() / 1024);
			else
				strFileSize = QString("%1 MB").arg(fi.size() / (1024 * 1024));
			ui->tableWidget->setItem(nRow, logTable_Size, new QTableWidgetItem(strFileSize));
			nRow++;
		}
	}
}

logManager::logManager(QWidget* parent) :
	QWidget(parent),
	ui(new Ui::logManager)
{
	ui->setupUi(this);
	QDateTime tStartTime = QDateTime::currentDateTime();
	QString strStart = tStartTime.toString("yyyy/MM/dd");
	QString strStop = strStart;
	strStart += " 00:00:00";
	tStartTime = QDateTime::fromString(strStart, "yyyy/MM/dd HH:mm:ss");

	strStop += " 23:59:00";
	QDateTime tStopTime = QDateTime::fromString(strStop, "yyyy/MM/dd HH:mm:ss");

	ui->dateTimeEdit_Start->setDateTime(tStartTime);
	ui->dateTimeEdit_Stop->setDateTime(tStopTime);
	ui->tableWidget->setStyleSheet("selection-background-color:rgb(255,209,128)");
	QString strAppPath = QCoreApplication::applicationDirPath();
	strAppPath += "/log";
	auto FileList = SearchFiles(strAppPath);
	if (FileList.size())
	{
		Showlog(FileList);
	}
	ui->tableWidget->setSelectionBehavior(QAbstractItemView::SelectRows);
	//ui->tableWidget->setSelectionMode(QAbstractItemView::SingleSelection);
	int nCols = ui->tableWidget->columnCount();
	for (int nIndex = 0; nIndex < nCols; nIndex++)
	{
		ui->tableWidget->horizontalHeader()->setSectionResizeMode(nIndex, QHeaderView::ResizeToContents);
	}
	ui->tableWidget->horizontalHeader()->setSectionResizeMode(logTable_FileName, QHeaderView::Stretch);
	ui->tableWidget->verticalHeader()->setSectionResizeMode(QHeaderView::Stretch);
}

logManager::~logManager()
{
	delete ui;
}

void logManager::on_pushButton_Search_clicked()
{
	if (ui->dateTimeEdit_Start->dateTime() > ui->dateTimeEdit_Stop->dateTime())
	{
		QMessageBox_CN(QMessageBox::Information, "提示", "截止时间必须大于起始时间", QMessageBox::Ok, this);
		return;
	}
	QString strAppPath = QCoreApplication::applicationDirPath();
	strAppPath += "/log";
	auto strFileList = SearchFiles(strAppPath, &ui->dateTimeEdit_Start->dateTime(), &ui->dateTimeEdit_Stop->dateTime());
	if (strFileList.size())
		Showlog(strFileList);
}

void logManager::on_pushButton_Export_clicked()
{
	QList<QTableWidgetItem*> ItemList = ui->tableWidget->selectedItems();
	if (!ItemList.size())
	{
		QMessageBox_CN(QMessageBox::Information, "提示", "尚未选中需要导出的日志文件!", QMessageBox::Ok, this);
		return;
	}
	QString selectedFilter;
	QString strFileName = QFileDialog::getSaveFileName(this,
		"导出日志",
		QCoreApplication::applicationDirPath(),
		tr("压缩文件(*.zip);;All Files (*)"),
		&selectedFilter);
	qDebug() << selectedFilter.toStdWString();
	if (strFileName.isEmpty())
	{
		return;
	}
	vector<wstring> vecFiles;
	for (auto var : ItemList)
	{
		if (var->column() != logTable_FileName)
			continue;
		vecFiles.push_back(var->data(Qt::UserRole).toString().toStdWString());
	}
	QWaitCursor Wait;
	QString str7ZLib = QCoreApplication::applicationDirPath() + "/7z.dll";
	Bit7zLibrary lib7z{ str7ZLib.toStdWString() };
	BitCompressor compressor{ lib7z, BitFormat::Zip };
	compressor.compress(vecFiles, strFileName.toStdWString());
}

void logManager::on_pushButton_Del_clicked()
{
	QList<QTableWidgetItem*> ItemList = ui->tableWidget->selectedItems();
	if (!ItemList.size())
	{
		QMessageBox_CN(QMessageBox::Information, "提示", "尚未选中需要删除的日志文件!", QMessageBox::Ok, this);
		return;
	}

	if (QMessageBox_CN(QMessageBox::Information, "提示", "是否删除所选日志,删除后将无法恢复,是否继续?", QMessageBox::Yes | QMessageBox::No, this) == QMessageBox::No)
		return;
	QWaitCursor Wait;
	vector<int>vecRows;
	for (auto var : ItemList)
	{
		if (var->column() != logTable_FileName)
			continue;
		QFile file2Del(var->data(Qt::UserRole).toString());
		file2Del.remove();
		vecRows.push_back(var->row());
	}
	for (auto it = vecRows.rbegin(); it != vecRows.rend(); it++)
		ui->tableWidget->removeRow(*it);
	// 调整序号
	int nRows = ui->tableWidget->rowCount();
	for (int i = 0; i < nRows; i++)
	{
		QTableWidgetItem* pItem = ui->tableWidget->item(i, logTable_No);
		pItem->setText(QString("%1").arg(i + 1));
	}
}
