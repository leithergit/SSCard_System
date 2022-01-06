#include "dialog.h"
#include "ui_dialog.h"
#include <windows.h>
#include <winreg.h>
#include <QString>
#include <chrono>
#include <QMessageBox>
#include "./SDK/XZ_F31_API.h"
#include "../utility/Utility.h"

#pragma comment(lib,"./SDK/XZ_F31_API.lib")
#pragma comment(lib,"user32.lib")
#pragma comment(lib,"advapi32.lib")
#pragma comment(lib,"OleAut32.lib")

Dialog::Dialog(QWidget* parent)
	: QDialog(parent)
	, ui(new Ui::Dialog)
{
	ui->setupUi(this);
	WORD nPortcount = 0;
	wchar_t szPort[256][8] = { 0 };
	connect(this, &Dialog::UpdatePin, this, &Dialog::OnUpdatePin);
	if (EnumSerialPortW((wchar_t*)szPort, sizeof(szPort), nPortcount))
	{
		for (int i = 0; i < nPortcount; i++)
		{
			ui->comboBox_Port->addItem(QString::fromStdWString(wstring(szPort[i])));
		}
	}
	ui->comboBox_Baudrate->setCurrentIndex(2);

}

Dialog::~Dialog()
{
	delete ui;
}

uint Dialog::ThreadUpdatePin()
{
	uchar szTemp[16] = { 0 };
	int nRet = 0;
	while (bThreadUpdatePinRun)
	{
		nRet = SUNSON_ScanKeyPress(szTemp);
		if (nRet > 0)
			emit UpdatePin(szTemp[0]);
		this_thread::sleep_for(std::chrono::milliseconds(100));
	}
	return 0;
}

void Dialog::OnUpdatePin(char ch)
{
	ui->label_Status->setText("正在输入....");
	switch (ch)
	{
	case 0x1b:		// cancel
	{
		memset(szPin, 0, sizeof(szPin));
		nPinSize = 0;
		qDebug("Cancel...");
		break;
	}
	case 0x08:		// update
	{
		if (nPinSize > 0)
		{
			szPin[nPinSize - 1] = '\0';
			nPinSize--;
		}

		qDebug("Update...");
		break;
	}
	case 0x0d:		// confirm
	{
		qDebug("confirm...");
		break;
	}
	case 0x41:
	case 0x42:
	case 0x43:
	case 0x44:
	case 0x45:
	case 0x46:
	case 0x47:
	case 0x48:
	{
		qDebug("function key...");
		break;
		break;
	}
	case 0xaa:
	{
		qDebug("Input end...");
		ui->label_Status->setText("输入结束");
	}
	default:
	{
		szPin[nPinSize++] = ch;
	}
	}
	strPin = (const char*)szPin;
	ui->lineEdit_Pin->setText(strPin);
	ui->lineEdit_Text->setText(strPin);
}

void Dialog::on_pushButton_Open_clicked()
{

	QString strPort = ui->comboBox_Port->currentText();

	int nOffset = strPort.indexOf("COM");
	if (nOffset < 0)
		return;

	QString strNum = strPort.right(strPort.size() - 3);
	int nPort = strNum.toInt();
	int nBaudrate = ui->comboBox_Baudrate->currentText().toUInt();

	ui->label_Status->setText("等待输入");

	int ret = SUNSON_OpenCom(nPort, nBaudrate);
	if (ret == 0)
	{
		QMessageBox::information(nullptr, tr("提示"), "打开键盘失败!");
		return;
	}

	memset(szPin, 0, sizeof(szPin));
	nPinSize = 0;

	unsigned char szRetInfo[255] = { 0 };
	byte nMaxLength = (byte)ui->lineEdit_PinSize->text().toUInt();
	ret = SUNSON_UseEppPlainTextMode(0x06, 1, szRetInfo);
	if (ret == 0)
	{
		QMessageBox::information(nullptr, tr("提示"), "设置输入模式失败!");
		return;
	}
	strPin = "";
	ui->lineEdit_PinSize->setEnabled(false);
	ui->pushButton_Open->setEnabled(false);
	ui->pushButton_Close->setEnabled(true);
	ui->comboBox_Port->setEnabled(false);
	ui->comboBox_Baudrate->setEnabled(false);

	bThreadUpdatePinRun = true;
	ThreadUpdate = std::thread(&Dialog::ThreadUpdatePin, this);
}


void Dialog::on_pushButton_Close_clicked()
{
	bThreadUpdatePinRun = false;
	if (ThreadUpdate.joinable())
	{
		ThreadUpdate.join();

	}

	SUNSON_CloseCom();
	ui->pushButton_Open->setEnabled(true);
	ui->pushButton_Close->setEnabled(false);
	ui->lineEdit_PinSize->setEnabled(true);
	ui->comboBox_Port->setEnabled(true);
	ui->comboBox_Baudrate->setEnabled(true);
}
