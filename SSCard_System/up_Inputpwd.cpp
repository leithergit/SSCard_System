#pragma execution_character_set("utf-8")
#include "up_Inputpwd.h"
#include "ui_up_Inputpwd.h"
#include "Gloabal.h"
#include <QString>
#include <chrono>
//#include "mainwindow.h"

up_InputPWD::up_InputPWD(QLabel* pTitle, int nTimeout, QWidget* parent) :
	QStackPage(pTitle, nTimeout, parent),
    ui(new Ui::InputPWD)
{
	ui->setupUi(this);
    connect(this,&QStackPage::InputPin,this,&up_InputPWD::OnInputPin);
}

up_InputPWD::~up_InputPWD()
{
    ShutDown();
	delete ui;
}

void  up_InputPWD::ClearPassword()
{
    ZeroMemory(m_szPin,sizeof(m_szPin));
    m_nPinSize = 0;
    ui->lineEdit_OldPassword->setText("");
}

int up_InputPWD::ProcessBussiness()
{
    ClearPassword();
    m_nSSCardPWDSize = g_pDataCenter->GetSysConfigure()->nSSCardPasswordSize;
    m_strDevPort = g_pDataCenter->GetSysConfigure()->DevConfig.PinKeybroadPort.c_str();
    m_nBaudreate = QString(g_pDataCenter->GetSysConfigure()->DevConfig.nPinKeybroadBaudrate.c_str()).toUShort();
    m_pPinKeybroad = make_shared<QPinKeybroad>(m_strDevPort,m_nBaudreate);
    QString strError;
    if (!m_pPinKeybroad)
    {
        strError = QString("内存不足,初始化密码键盘对象失败!");
        gError()<<strError.toLocal8Bit().data();
        emit ShowMaskWidget(strError,Fetal,Return_MainPage);
        return -1;
    }

    if (QFailed(m_pPinKeybroad->OpenDevice(strError)))
    {
        gError()<<strError.toLocal8Bit().data();
        emit ShowMaskWidget(strError,Fetal,Return_MainPage);
        return -1;
    }
    m_bWorkThreadRunning = true;
    m_pWorkThread = new std::thread(&up_InputPWD::ThreadWork, this);
    if (!m_pWorkThread)
    {
        strError = QString("内存不足,创建密码键盘线程失败!");
        gError()<<strError.toLocal8Bit().data();
        emit ShowMaskWidget(strError,Fetal,Return_MainPage);
        return -1;
    }
	return 0;
}

void up_InputPWD::OnTimeout()
{
    ShutDown();
}

void  up_InputPWD::ShutDown()
{
    m_bWorkThreadRunning = false;
    if (m_pWorkThread && m_pWorkThread->joinable())
    {
        m_pWorkThread->join();
        delete m_pWorkThread;
        m_pWorkThread = nullptr;
    }
    m_pPinKeybroad = nullptr;
}

void up_InputPWD::OnInputPin(char ch)
{
    switch (ch)
    {
        case 0x1b:		// cancel
        {
            ClearPassword();
            break;
        }
        case 0x08:		// update
        {
            if (m_nPinSize > 0)
            {
                m_szPin[m_nPinSize - 1] = '\0';
                m_nPinSize --;
            }
            break;
        }
        case 0x0d:		// confirm
        {
            qDebug("confirm...");
            if (m_nPinSize < m_nSSCardPWDSize)
            {
                QString strMessage = QString("输入的密码位数不足%1位,请输入正确的密码!").arg(m_nSSCardPWDSize);
                emit ShowMaskWidget(strMessage,Error,Stay_CurrentPage);
            }
            else
            {
                g_pDataCenter->strSSCardOldPassword = (const char *)m_szPin;
                ui->pushButton_OK->emit click();
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
            if (m_nPinSize < 6)
                m_szPin[m_nPinSize++] = ch;
        }
    }
    QString strOldPassword = (char *)m_szPin;
    ui->lineEdit_OldPassword->setText(strOldPassword);
}
void up_InputPWD::ThreadWork()
{
    uchar szTemp[16] = {0};
    int nRet = 0;

    while (m_bWorkThreadRunning)
    {
        nRet = SUNSON_ScanKeyPress(szTemp);
        if (nRet == 0)
            emit InputPin(szTemp[0]);
        this_thread::sleep_for(std::chrono::milliseconds(50));
    }
}

int  up_InputPWD::CheckPassword(QString &strError)
{
    return 0;
}

void up_InputPWD::on_pushButton_OK_clicked()
{
    qDebug("confirm...");
    QString strError;
    if (m_nPinSize < 6)
    {
        strError = QString("输入的密码位数不足6位,请继续输入密码!");
        gError()<< strError.toLocal8Bit().data();
        emit ShowMaskWidget(strError,Error,Stay_CurrentPage);
    }
    else
    {
        QString strError;
        if (QFailed(CheckPassword(strError)))
        {
            strError = QString("密码校验失败,请重新输入密码!");
            gError()<< strError.toLocal8Bit().data();
            ui->lineEdit_OldPassword->setText("");
            emit ShowMaskWidget(strError,Error,Stay_CurrentPage);
        }
        else
            emit ShowMaskWidget(tr("密码校验成功,随后请输入新密码!"),Success,Switch_NextPage);
    }
}

