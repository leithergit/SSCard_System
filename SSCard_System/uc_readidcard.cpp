#pragma execution_character_set("utf-8")
#include "uc_readidcard.h"
#include "ui_uc_readidcard.h"
#include "Gloabal.h"
#include <chrono>
using namespace std;
using namespace chrono;

#pragma comment(lib,"user32.lib")
#pragma comment(lib,"advapi32.lib")
#pragma comment(lib,"OleAut32.lib")
//#pragma comment(lib,"SDK/IDCard/IDCard_API")

uc_ReadIDCard::uc_ReadIDCard(QLabel* pTitle, int nTimeout, QWidget* parent) :
	QStackPage(pTitle, nTimeout, parent),
	ui(new Ui::ReadIDCard)
{
	ui->setupUi(this);
}

uc_ReadIDCard::~uc_ReadIDCard()
{
    ShutDownDevice();
	delete ui;
}


int uc_ReadIDCard::ProcessBussiness()
{
	SysConfigPtr& pConfigure = g_pDataCenter->GetSysConfigure();
	m_strDevPort = pConfigure->DevConfig.strIDCardReaderPort;
    m_bWorkThreadRunning = true;
    m_pWorkThread = make_shared<std::thread>(&uc_ReadIDCard::ThreadWork, this);
	return 0;
}

void uc_ReadIDCard::OnTimeout()
{
    ShutDownDevice();
}

void  uc_ReadIDCard::ShutDownDevice()
{
    m_bWorkThreadRunning = false;
    if (m_pWorkThread && m_pWorkThread->joinable())
    {
        m_pWorkThread->join();
        m_pWorkThread = nullptr;
    }
}

void uc_ReadIDCard::ThreadWork()
{
	auto tLast = high_resolution_clock::now();
    while (m_bWorkThreadRunning)
	{
		auto tDuration = duration_cast<milliseconds>(high_resolution_clock::now() - tLast);
		if (tDuration.count() >= 1000)
		{
			tLast = high_resolution_clock::now();
			int nResult = ReaderIDCard();
			if (nResult == IDCard_Status::IDCard_Succeed)
			{
                emit SwitchNextPage();
                gInfo() << QString(tr("读取身份证成功!")).toLocal8Bit().data();
				break;
			}
			else
			{
				char szText[256] = { 0 };
				GetErrorMessage((IDCard_Status)nResult, szText, sizeof(szText));
                gError() << QString("读取身份证失败:").toLocal8Bit().data() << QString(szText).toLocal8Bit().data();
			}
		}
		else
		{
			this_thread::sleep_for(chrono::milliseconds(200));
		}
	}
}

int  uc_ReadIDCard::GetIDImageStorePath(QString &strFilePath)
{
    QString strStorePath = QCoreApplication::applicationDirPath();
    strStorePath += "/IDImage/";
    strStorePath += QDateTime::currentDateTime().toString("yyyyMMdd/");
    QFileInfo fi(strStorePath);
    if (!fi.isDir())
    {// 当天目录不存在？则创建目录
        QDir storeDir;
        if (!storeDir.mkpath(strStorePath))
        {
            char szError[1024] = {0};
            _strerror_s(szError,1024,nullptr);
            QString Info = QString("创建身份证照片保存目录'%1'失败:%2").arg(strStorePath,szError);
            gInfo()<<Info.toLocal8Bit().data();
            return -1;
        }
    }
    strFilePath = strStorePath + QString("ID_%1.bmp").arg((const char *)g_pDataCenter->GetIDCardInfo()->szIdentify);
    return 0;
}

#define    Error_Not_IDCARD         (-1)

int uc_ReadIDCard::ReaderIDCard()
{
	int nResult = IDCard_Status::IDCard_Succeed;
	do
	{
		if (m_strDevPort.size())
			nResult = OpenReader(m_strDevPort.c_str());
		else
			nResult = OpenReader(nullptr);
		if (m_bSucceed && m_nDelayCount <= 3)
		{
			m_nDelayCount++;
			break;
		}
		else
		{
			m_nDelayCount = 0;
			m_bSucceed = false;
		}

		nResult = OpenReader(nullptr);;
		if (nResult != IDCard_Status::IDCard_Succeed)
		{
			break;
		}
		nResult = FindIDCard();
		if (nResult != IDCard_Status::IDCard_Succeed)
		{
			break;
		}


        nResult = ReadIDCard(*m_pIDCard.get());
		if (nResult != IDCard_Status::IDCard_Succeed)
		{
			break;
		}

//            ui->label_Name->setText(QString::fromLocal8Bit((const char *)IDCard.szName));
//            ui->label_Gender->setText(QString::fromLocal8Bit((const char *)IDCard.szGender));
//            ui->label_Nation->setText(QString::fromLocal8Bit((const char *)IDCard.szNation));
//            ui->label_Birthday->setText(QString::fromLocal8Bit((const char *)IDCard.szBirthday));
//            ui->label_Identify->setText(QString::fromLocal8Bit((const char *)IDCard.szIdentify));
//            ui->label_Address->setText(QString::fromLocal8Bit((const char *)IDCard.szAddress));
//            ui->label_IssueAuthority->setText(QString::fromLocal8Bit((const char *)IDCard.szIszssueAuthority));
//            ui->label_ExpirationDate->setText(QString::fromLocal8Bit((const char *)IDCard.szExpirationDate1) + "-" + QString::fromLocal8Bit((const char *)IDCard.szExpirationDate2));

        if (Succeed(GetIDImageStorePath(g_pDataCenter->strIDImageFile)))
        {
            QImage ImagePhoto = QImage::fromData(m_pIDCard->szPhoto,m_pIDCard->nPhotoSize);
            ImagePhoto.save(g_pDataCenter->strIDImageFile);
        }

		m_bSucceed = true;
		CloseReader();
	} while (0);
	return nResult;
}
void uc_ReadIDCard::timerEvent(QTimerEvent* event)
{

}
