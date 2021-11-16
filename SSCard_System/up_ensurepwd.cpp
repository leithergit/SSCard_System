#pragma execution_character_set("utf-8")
#include "up_ensurepwd.h"
#include "ui_up_ensurepwd.h"
#include "Gloabal.h"
#include <QString>
#include <chrono>
#include <QMessageBox>

#pragma comment(lib,"user32.lib")
#pragma comment(lib,"advapi32.lib")
#pragma comment(lib,"OleAut32.lib")


up_EnsurePWD::up_EnsurePWD(QLabel* pTitle, int nTimeout, QWidget* parent) :
	QStackPage(pTitle, nTimeout, parent),
	ui(new Ui::EnsurePWD)
{
	ui->setupUi(this);
    connect(this,&up_EnsurePWD::UpdatePin,this,&up_EnsurePWD::OnUpdatePin);
}

up_EnsurePWD::~up_EnsurePWD()
{
    ShutDownDevice();
	delete ui;
}

int up_EnsurePWD::ProcessBussiness()
{
    m_bDevOpened = false;
    m_strDevPort = g_pDataCenter->GetSysConfigure()->DevConfig.PinKeybroadPort.c_str();
    m_strDevPort = m_strDevPort.toUpper();
    m_nBaudreate = QString(g_pDataCenter->GetSysConfigure()->DevConfig.nPinKeybroadBaudrate.c_str()).toUShort();

    QString strError;
    if (!Succeed(OpenDevice(strError)))
    {
        gError()<<strError.toLocal8Bit().data();
        return -1;
    }
    m_bWorkThreadRunning = true;
    m_pWorkThread = new std::thread(&up_EnsurePWD::ThreadWork, this);
	return 0;
}

void up_EnsurePWD::OnTimeout()
{
    ShutDownDevice();
}

void up_EnsurePWD::on_pushButton_EnsurePWD_clicked()
{
    qDebug("confirm...");
    if (nPinSize < 6)
    {
        emit ShowMaskWidget(tr("输入的密码位数不足6位,请继续输入密码!"),Warning,1000);
    }
    else
    {
        g_pDataCenter->strSSCardOldPassword = (const char *)szPin;
        emit SwitchNextPage();
    }
}

void  up_EnsurePWD::ShutDownDevice()
{
    m_bWorkThreadRunning = false;
    if (m_pWorkThread && m_pWorkThread->joinable())
    {
        m_pWorkThread->join();
        delete m_pWorkThread;
        m_pWorkThread = nullptr;
    }
    QString strError;
    CloseDevice(strError);
}


int up_EnsurePWD::OpenDevice(QString &strError)
{
    int nOffset = m_strDevPort.indexOf("COM");
    if (nOffset < 0)
    {
        strError = QString("密码键盘端口参数错误:%1").arg(m_strDevPort);
        return -1;
    }

    QString strNum = m_strDevPort.right(m_strDevPort.size() - 3 );
    int nPort = strNum.toInt();

    if(SUNSON_OpenCom(nPort, m_nBaudreate))
    {
        strError = QString("打开密码键盘失败:%1,Baudrate:%2").arg(m_strDevPort).arg(m_nBaudreate);
        return -1;
    }

    unsigned char szRetInfo[255] = { 0 };
    if (SUNSON_UseEppPlainTextMode(0x03, szRetInfo))
    {
        SUNSON_CloseCom();
        strError = QString("密码键盘设置输入模式失败!");
        return -1;
    }
    memset(szPin,0,sizeof(szPin));
    nPinSize = 0;
    m_bDevOpened = true;
    return 0;
}

int  up_EnsurePWD::CloseDevice(QString &strError)
{
    Q_UNUSED(strError);
    if (m_bDevOpened)
    {
        m_bDevOpened = false;
        SUNSON_CloseCom();
    }

    return 0;
}
void up_EnsurePWD::OnUpdatePin(char ch)
{
    switch (ch)
    {
        case 0x1b:		// cancel
        {
            memset(szPin,0,sizeof (szPin));
            nPinSize = 0;
            qDebug("Cancel...");
            break;
        }
        case 0x08:		// update
        {
            if (nPinSize > 0)
            {
                szPin[nPinSize - 1] = '\0';
                nPinSize --;
            }

            qDebug("Update...");
            break;
        }
        case 0x0d:		// confirm
        {
            qDebug("confirm...");
            if (nPinSize < 6)
            {
                emit ShowMaskWidget(tr("输入的密码位数不足6位,请继续输入密码!"),Warning,1000);
            }
            else
            {
                g_pDataCenter->strSSCardOldPassword = (const char *)szPin;
                emit SwitchNextPage();
            }
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
        }
        case 0xaa:
        {
            qDebug("Input end...");
            break;
        }
        default:
        {
            if (nPinSize < 6)
                szPin[nPinSize++] = ch;
        }
    }
    QString strOldPassword = (char *)szPin;
    ui->lineEdit_OldPassword->setText(strOldPassword);
}
void up_EnsurePWD::ThreadWork()
{
    uchar szTemp[16] = {0};
    int nRet = 0;

    while (m_bWorkThreadRunning)
    {
        nRet = SUNSON_ScanKeyPress(szTemp);
        if (nRet == 0)
            emit UpdatePin(szTemp[0]);
        this_thread::sleep_for(std::chrono::milliseconds(50));
    }
}
