#ifndef UP_ENSUREPWD_H
#define UP_ENSUREPWD_H
#pragma execution_character_set("utf-8")
#include <QWidget>
#include "qstackpage.h"

namespace Ui {
	class EnsurePWD;
}

class up_EnsurePWD : public QStackPage
{
	Q_OBJECT

public:
	explicit up_EnsurePWD(QLabel* pTitle, int nTimeout = 30, QWidget* parent = nullptr);
	~up_EnsurePWD();
	virtual int ProcessBussiness() override;
	virtual void OnTimeout() override;
    int     m_bDevOpened = false;
    void    ThreadWork();
    void    ShutDownDevice();
    int     OpenDevice(QString &strError);
    int     CloseDevice(QString &strError);
    uchar   szPin[32];
    int     nPinSize = 0;
    QString  m_strDevPort;
    ushort  m_nBaudreate = 9600;
signals:
    void UpdatePin(char ch);
private slots:
    void on_pushButton_EnsurePWD_clicked();

    void OnUpdatePin(char ch);

private:
	Ui::EnsurePWD* ui;
};

#endif // UP_ENSUREPWD_H
