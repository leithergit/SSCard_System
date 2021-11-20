#pragma execution_character_set("utf-8")
#include "up_changepwd.h"
#include "ui_up_changepwd.h"
#include "Gloabal.h"
//#include "mainwindow.h"

up_ChangePWD::up_ChangePWD(QLabel* pTitle, int nTimeout, QWidget* parent) :
	QStackPage(pTitle, nTimeout, parent),
	ui(new Ui::up_ChangePWD)
{
	ui->setupUi(this);
    ui->lineEdit_Password1->installEventFilter(this);
    ui->lineEdit_Password2->installEventFilter(this);
    m_pLineEdit[0] = ui->lineEdit_Password1;
    m_pLineEdit[1] = ui->lineEdit_Password2;
    connect(this,&QStackPage::InputPin,this,&up_ChangePWD::OnInputPin);
}

up_ChangePWD::~up_ChangePWD()
{
    ShutDown();
	delete ui;
}

bool up_ChangePWD::eventFilter(QObject *watched, QEvent *event)
{
    if (event->type() == QEvent::FocusIn)
    {
        if (watched == ui->lineEdit_Password1)
            m_nInputFocus = 0;

        if (watched == ui->lineEdit_Password2)
            m_nInputFocus = 1;
        return true;
    }
    else
        return false;
}

void  up_ChangePWD::ClearPassword()
{
    ZeroMemory(m_szPin,sizeof(m_szPin));
    ZeroMemory(m_nPinSize,sizeof(m_nPinSize));
    ui->lineEdit_Password1->setText("");
    ui->lineEdit_Password2->setText("");
}

int  up_ChangePWD::CheckPassword(QString &strError)
{
    if (QFailed(memcmp(m_szPin[0],m_szPin[1],m_nSSCardPWDSize)))
    {
        strError = u8"两次输入密码不一致,请重新输入密码";
        return -1;
    }
    else
        return 0;
}

int  up_ChangePWD::ChangePassword(QString &strError)
{
    return 0;
}

int up_ChangePWD::ProcessBussiness()
{
	ClearPassword();
    m_nInputFocus = 0;
    m_nSSCardPWDSize = g_pDataCenter->GetSysConfigure()->nSSCardPasswordSize;
    m_pLineEdit[0]->setFocus();
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
    m_pWorkThread = new std::thread(&up_ChangePWD::ThreadWork, this);
    if (!m_pWorkThread)
    {
        strError = QString("内存不足,创建密码键盘线程失败!");
        gError()<<strError.toLocal8Bit().data();
        emit ShowMaskWidget(strError,Fetal,Return_MainPage);
        return -1;
    }
	return 0;
}

void up_ChangePWD::OnTimeout()
{
    ShutDown();
}

void  up_ChangePWD::ShutDown()
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

void up_ChangePWD::OnInputPin(uchar ch)
{
    switch (ch)
    {
        case 0x1b:		// cancel
        {
            memset(m_szPin[m_nInputFocus],0,sizeof (m_szPin[m_nInputFocus]));
            m_nPinSize[m_nInputFocus] = 0;
            m_pLineEdit[m_nInputFocus]->setText("");
            gInfo()<<QString("取消密码1输入!").toLocal8Bit().data();
            break;
        }
        case 0x08:		// update
        {
            if (m_nPinSize[m_nInputFocus] > 0)
            {
                m_szPin[m_nInputFocus][m_nPinSize[m_nInputFocus] - 1] = '\0';
                m_nPinSize[m_nInputFocus] --;
            }

            qDebug("Update...");
            break;
        }
        case 0x0d:		// confirm
        {
            ui->pushButton_OK->emit click();


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
            if (m_nPinSize[m_nInputFocus] < m_nSSCardPWDSize)
            {
                m_szPin[m_nInputFocus][m_nPinSize[m_nInputFocus]++] = ch;
            }
            break;
        }
    }
    QString strOldPassword = (char *)m_szPin[m_nInputFocus];
    m_pLineEdit[m_nInputFocus]->setText(strOldPassword);
}
void up_ChangePWD::ThreadWork()
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

void up_ChangePWD::on_pushButton_OK_clicked()
{
    if (m_nPinSize[m_nInputFocus] < m_nSSCardPWDSize)
    {
        QString strMessage = QString("输入的密码位数不足%1位,请输入正确的密码!").arg(m_nSSCardPWDSize);
        emit ShowMaskWidget(strMessage,Error,Stay_CurrentPage);
    }
    else
    {
        if (m_nInputFocus == 1)
        {
            QString strMessage;
            if (QFailed(CheckPassword(strMessage)))
            {
                emit ShowMaskWidget(strMessage,Error,Stay_CurrentPage);
                return ;
            }
            if (QFailed(ChangePassword(strMessage)))
            {
                emit ShowMaskWidget(strMessage,Error,Stay_CurrentPage);
                return ;
            }
            emit ShowMaskWidget("密码修改成功,请妥善保管好您的新密码!",Success,Switch_NextPage);
        }
        else
        {
             m_nInputFocus = 1;
             m_pLineEdit[m_nInputFocus]->setFocus();
        }
    }
}

