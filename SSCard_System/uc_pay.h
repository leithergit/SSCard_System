#ifndef UC_PAY_H
#define UC_PAY_H

#include <QWidget>
#include "qstackpage.h"

namespace Ui {
class Pay;
}

class uc_Pay : public QStackPage
{
    Q_OBJECT

public:
    explicit uc_Pay(QLabel *pTitle,int nTimeout = 300,QWidget *parent = nullptr);
    ~uc_Pay();
    virtual int ProcessBussiness() override;
    virtual void OnTimeout() override;
private:
    Ui::Pay *ui;
};

#endif // UC_PAY_H
