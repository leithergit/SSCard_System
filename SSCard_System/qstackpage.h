#ifndef QSTACKPAGE_H
#define QSTACKPAGE_H

#include <QObject>
#include <QWidget>
#include <QLabel>
#include <QStackedWidget>
#include <QThread>

class QStackPage : public QWidget
{
    Q_OBJECT
public:
    explicit QStackPage(QLabel *pTitle,int nTimeout = 30,QWidget *parent = nullptr):
            QWidget(parent),
            m_pTitle(pTitle),
            m_nTimeout(nTimeout)
    {
    }
    // 处理业务接口，成功时返回0，超时返回-1，其它值为具体错误码
    virtual int ProcessBussiness(){return 0;};
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
    QLabel  *m_pTitle = nullptr;
    int m_nTimeout = 0;
    int m_nCountDown = 0;

signals:
    void SwitchNextPage();
};

#endif // QSTACKPAGE_H
