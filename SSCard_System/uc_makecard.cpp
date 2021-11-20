#pragma execution_character_set("utf-8")
#include "uc_makecard.h"
#include "ui_uc_makecard.h"
#include "Gloabal.h"

uc_MakeCard::uc_MakeCard(QLabel* pTitle, int nTimeout, QWidget* parent) :
	QStackPage(pTitle, nTimeout, parent),
	ui(new Ui::MakeCard)
{
	ui->setupUi(this);
}

uc_MakeCard::~uc_MakeCard()
{
	delete ui;
}


int uc_MakeCard::ProcessBussiness()
{
    QString strMessage;
    if (QFailed(OpenDevice(strMessage)))
    {
        gError()<<strMessage.toLocal8Bit().data();
        emit ShowMaskWidget(strMessage,Fetal,Return_MainPage);
        return -1;
    }
    m_pSSCardInfo = g_pDataCenter->GetSSCardInfo();
    m_bWorkThreadRunning = true;
    m_pWorkThread = new std::thread(&uc_MakeCard::ThreadWork, this);
	return 0;
}

int uc_MakeCard::OpenDevice(QString &strMessage)
{
    try
    {
        DeviceConfig & DevConfig = g_pDataCenter->GetSysConfigure()->DevConfig;
        m_pPrinterlib = make_shared<KT_Printerlib>();
        if (!m_pPrinterlib)
        {
            strMessage = u8"内存不足，创建‘KT_Printerlib’实例失败！";
            return -1;
        }
        m_pPrinter = m_pPrinterlib->Instance();
        if (!m_pPrinter)
        {
            strMessage = u8"创建‘KT_Printer’实例失败！";
            return -1;
        }
        char szRCode[32] = {0};
        if (QFailed(m_pPrinter->Printer_Init(DevConfig.nPrinterType,szRCode)))
        {
            strMessage = QString("打开打印‘%1’失败,错误代码:%2").arg(DevConfig.strPrinterType.c_str()).arg(szRCode);
            return -1;
        }
    }
    catch (std::exception &e)
    {
        strMessage = e.what();
        return -1;
    }
    return 0;
}

void uc_MakeCard::CloseDevice()
{
    if (!m_pPrinter)
        return;
    char szRCode[32] = {0};
    m_pPrinter->Printer_Close(szRCode);
}

void uc_MakeCard::OnTimeout()
{
    ShutDown();
}

void  uc_MakeCard::ShutDown()
{
    CloseDevice();
    m_bWorkThreadRunning = false;
    if (m_pWorkThread && m_pWorkThread->joinable())
    {
        m_pWorkThread->join();
        m_pWorkThread = nullptr;
    }
}

/*
    typedef struct _PRINTERSTATUS
    {
        WORD fwDevice;			//打印机状态, 0-Ready；1-Busy；2-Offline；3-ErrMachine；4-Printing
        WORD fwMedia;			//介质状态，0-无卡；1-卡在门口；2-卡在内部；3-卡在上电位，4-卡在闸门外；5-堵卡；6-卡片未知（根据硬件特性返回,必须支持有无卡检测）
        WORD fwToner;			//平印色带状态,0-FLLL;1-LOW;2-OUT;3-NOTSUPP;4-UNKNOW
    }PRINTERSTATUS, * LPPRINTERSTATUS;
*/
void uc_MakeCard::ThreadWork()
{
    char szRCode[32] = {0};
    QString strMessage;
    Kingaotech::_PRINTERBOX boxesUnit[10];
    Kingaotech::BOXINFO boxesInfo = {10, boxesUnit};
    Kingaotech::LPBOXINFO lpBoxInfo =&boxesInfo;
    Kingaotech::PRINTERSTATUS PrinterStatus;
    Kingaotech::LPPRINTERSTATUS lpPrinterStatus = &PrinterStatus ;

    SSCardInfoPtr &pCardInfo = g_pDataCenter->GetSSCardInfo();
    CardFormPtr &pCardForm = g_pDataCenter->GetCardForm();
    while(m_bWorkThreadRunning)
    {
        if (!m_pPrinter)
            return ;
        bool bSucceed = false;
        do
        {
            if (QFailed(m_pPrinter->Printer_BoxStatus(lpBoxInfo,szRCode)))
            {
                strMessage = QString("Printer_BoxStatus失败:%1").arg(szRCode);
                break;
            }
            if (lpBoxInfo->BoxList[0].BoxStatus == 2)
            {
                strMessage = QString("卡箱:%1无卡!").arg(szRCode);
                break;
            }

            if (QFailed(m_pPrinter->Printer_Status(lpPrinterStatus,szRCode)))
            {
                strMessage = QString("Printer_Status失败，错误代码:%1!").arg(szRCode);
                break;
            }
            if (lpPrinterStatus->fwDevice != 0)
            {
                strMessage = QString("打印机未就绪,状态代码:%1!").arg(lpPrinterStatus->fwDevice);
                break;
            }

            if (lpPrinterStatus->fwMedia != 0)
            {
                strMessage = QString("打印机色带耗尽或未安装色带,状态代码:%1!").arg(lpPrinterStatus->fwToner);
                if (QFailed(m_pPrinter->Printer_Retract(1,szRCode)))
                {
                    strMessage = QString("打印机尚有未取走卡片,尝试将其移动到回收箱失败，错误代码:%1!").arg(szRCode);
                    break;
                }
            }
            if (lpPrinterStatus->fwToner == 2)
            {
                strMessage = QString("打印机色带耗尽或未安装色带,状态代码:%1!").arg(lpPrinterStatus->fwToner);
                break;
            }

            if (QFailed(m_pPrinter->Printer_Dispense(1,4,szRCode)))
            {
                 strMessage = QString("Printer_Dispense失败，错误代码:%1!").arg(szRCode);
                break;
            }

            if (QFailed(m_pPrinter->Printer_InitPrint(nullptr,szRCode)))
            {
                strMessage = QString("Printer_InitPrint失败，错误代码:%1!").arg(szRCode);
                break;
            }
            // int Printer_AddText(char* Text, int Angle, float Xpos, float Ypos, char* FontName, int FontSize, int FontStyle, int TextColor, char* resRcCode) = 0;
            TextPosition *pFieldPos = &pCardForm->posName;
            m_pPrinter->Printer_AddText((char *)pCardInfo->strName.c_str(), pFieldPos->nAngle, pFieldPos->fxPos, pFieldPos->fyPos ,(char *)pFieldPos->strFontName.c_str(), pFieldPos->nFontSize , pFieldPos->nFontStyle, pFieldPos->nColor, szRCode);
            pFieldPos = &pCardForm->posIDNumber;
            m_pPrinter->Printer_AddText((char *)pCardInfo->strIDNumber.c_str(), pFieldPos->nAngle, pFieldPos->fxPos, pFieldPos->fyPos ,(char *)pFieldPos->strFontName.c_str(), pFieldPos->nFontSize , pFieldPos->nFontStyle, pFieldPos->nColor, szRCode);
            pFieldPos = &pCardForm->posSSCardID;
            m_pPrinter->Printer_AddText((char *)pCardInfo->strSSCardNumber.c_str(), pFieldPos->nAngle, pFieldPos->fxPos, pFieldPos->fyPos ,(char *)pFieldPos->strFontName.c_str(), pFieldPos->nFontSize , pFieldPos->nFontStyle, pFieldPos->nColor, szRCode);
            pFieldPos = &pCardForm->posIssueDate;
            m_pPrinter->Printer_AddText((char *)pCardInfo->strIssuedDate.c_str(), pFieldPos->nAngle, pFieldPos->fxPos, pFieldPos->fyPos ,(char *)pFieldPos->strFontName.c_str(), pFieldPos->nFontSize , pFieldPos->nFontStyle, pFieldPos->nColor, szRCode);
            ImagePosition &ImgPos = pCardForm->posImage;
            m_pPrinter->Printer_AddImage((char *)pCardInfo->strPhotoPath.c_str(), ImgPos.nAngle, ImgPos.fxPos, ImgPos.fyPos, ImgPos.fHeight, ImgPos.fWidth,szRCode);
            if (QFailed(m_pPrinter->Printer_StartPrint(szRCode)))
            {
                strMessage = QString("Printer_StartPrint失败，错误代码:%1!").arg(szRCode);
                break;
            }
            if (QFailed(m_pPrinter->Printer_Eject(szRCode)))
            {
                strMessage = QString("Printer_Eject失败，错误代码:%1!").arg(szRCode);
                break;
            }
            bSucceed = true;

        }while(0);
        if (!bSucceed)
        {
            gError()<< strMessage.toLocal8Bit().data();
            emit ShowMaskWidget(strMessage,Error,Stay_CurrentPage);
        }
        this_thread::sleep_for(chrono::milliseconds(200));
    }
}
