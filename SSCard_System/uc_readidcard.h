#ifndef UC_READIDCARD_H
#define UC_READIDCARD_H

#include <QWidget>
#include "qstackpage.h"
#include "./SDK/IDCard/idcard_api.h"

namespace Ui {
class ReadIDCard;
}

class uc_ReadIDCard : public QStackPage
{
    Q_OBJECT

public:
    explicit uc_ReadIDCard(QLabel *pTitle,int nTimeout = 30,QWidget *parent = nullptr);
    ~uc_ReadIDCard();
    virtual int ProcessBussiness() override;
    virtual void OnTimeout() override;
    int     m_nTimerReaderIDCard = 0;
    virtual void timerEvent(QTimerEvent *event);
    int     nPort = -1;
    int     nDelayCount = 0;
    int     bSucceed = false;
    int     ReaderIDCard();
    void    ThreadWork();
    std::thread ThreadReadCard;

    IDCardInfo IDCard;
    bool    bRunning = false;

private:
    Ui::ReadIDCard *ui;
};

#endif // UC_READIDCARD_H
