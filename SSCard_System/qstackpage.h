#ifndef QSTACKPAGE_H
#define QSTACKPAGE_H
#pragma execution_character_set("utf-8")
#include <QObject>
#include <QWidget>
#include <QLabel>
#include <QStackedWidget>
#include <QThread>
#include <thread>
#include <string>
#include <memory>
#include <QMessageBox>
#include <QDir>
#include <QFile>
#include "SDK/IDCard/idcard_api.h"
#include "SDK/PinKeybroad/XZ_F10_API.h"

enum MaskStatus
{
    Succeed = 1,
    Failed = 2,
    Warning = 3
};

using namespace std;
class QStackPage : public QWidget
{
	Q_OBJECT
public:
	explicit QStackPage(QLabel* pTitle, int nTimeout = 30, QWidget* parent = nullptr) :
		QWidget(parent),
		m_pTitle(pTitle),
		m_nTimeout(nTimeout)
	{
	}
    ~QStackPage()
    {
        m_bWorkThreadRunning = false;
        if (m_pWorkThread && m_pWorkThread->joinable())
        {
            m_pWorkThread->join();
            delete m_pWorkThread;
            m_pWorkThread = nullptr;
        }
    }
	// 处理业务接口，成功时返回0，超时返回-1，其它值为具体错误码
	virtual int ProcessBussiness() { return 0; };
	virtual void OnTimeout() = 0;

	virtual void ActiveTitle(QString strTitle = "")
	{
		m_nCountDown = m_nTimeout;
		if (m_pTitle)
		{
			if (strTitle.size())
				m_pTitle->setText(strTitle);
			m_pTitle->setStyleSheet(QString::fromUtf8("font: 25 24pt \"Microsoft YaHei UI\";color: rgb(0, 0, 0);"));
		}
	}

	virtual void DisActiveTitle()
	{
		if (m_pTitle)
			m_pTitle->setStyleSheet(QString::fromUtf8("font: 25 24pt \"Microsoft YaHei UI\";color: rgb(200, 200, 200);"));
	}

	QLabel* m_pTitle = nullptr;
    int     m_nTimeout = 0;
    int     m_nCountDown = 0;
    bool    m_bWorkThreadRunning = false;
    std::thread *m_pWorkThread = nullptr;
public slots:
    // 显示遮罩层
    // strMessage   遮罩层消息
    // 遮罩层图标状态  0为成功，1为失败，2为警告
    // 遮罩层滞留时间  单位毫秒
    virtual void  OnShowMaskWidget(QString strMessage,MaskStatus nStatus,int nTimeoutms )
    {
        return;
    }

signals:
	void SwitchNextPage();
    void ErrorMessage(QString strMessage);
    void ShowMaskWidget(QString strMessage,MaskStatus nStatus ,int nTimeoutms );
};

#endif // QSTACKPAGE_H
