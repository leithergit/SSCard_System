#ifndef UC_ADFORFINANCE_H
#define UC_ADFORFINANCE_H

#include <QWidget>
#include "qstackpage.h"

namespace Ui {
class AdforFinance;
}

class uc_AdforFinance : public QStackPage
{
    Q_OBJECT

public:
    explicit uc_AdforFinance(QLabel *pTitle,int nTimeout = 10,QWidget *parent = nullptr);
    ~uc_AdforFinance();
    virtual void OnTimeout() override;
private:
    Ui::AdforFinance *ui;
};

#endif // UC_ADFORFINANCE_H
