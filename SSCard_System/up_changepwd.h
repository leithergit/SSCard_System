#ifndef UP_CHANGEPWD_H
#define UP_CHANGEPWD_H
#pragma execution_character_set("utf-8")
#include <QWidget>
#include "qstackpage.h"
#include "qpinkeybroad.h"
#include <QLineEdit>

namespace Ui {
	class up_ChangePWD;
}

class up_ChangePWD : public QStackPage
{
	Q_OBJECT

public:
	explicit up_ChangePWD(QLabel* pTitle, int nTimeout = 30, QWidget* parent = nullptr);
	~up_ChangePWD();
	virtual int ProcessBussiness() override;
	virtual void OnTimeout() override;    
    void    ThreadWork();
    void    ShutDownDevice();	
    void    ClearPassword();
    int     CheckPassword(QString &strError);
    int     ChangePassword(QString &strError);
    uchar   m_szPin[2][16] = {{0}};
    int     m_nPinSize[2] = {0};
    QLineEdit*  m_pLineEdit[2] = {nullptr};
    QString  m_strDevPort;
    ushort  m_nBaudreate = 9600;
    int     m_nInputFocus = 0;
    int     m_nSSCardPWDSize=  6;
    QPinKeybroadPtr m_pPinKeybroad = nullptr;
    bool eventFilter(QObject *watched, QEvent *event) override;
private slots:
    void on_pushButton_OK_clicked();
    void OnInputPin(uchar ch);

private:
	Ui::up_ChangePWD* ui;
};

#endif // UP_CHANGEPWD_H
