#ifndef UP_READSSCARD_H
#define UP_READSSCARD_H

#include <QWidget>
#include "qstackpage.h"

namespace Ui {
class ReadSSCard;
}

class up_ReadSSCard : public QStackPage
{
    Q_OBJECT

public:
    explicit up_ReadSSCard(QLabel *pTitle,int nTimeout = 30,QWidget *parent = nullptr);
    ~up_ReadSSCard();
    virtual int ProcessBussiness() override;
    virtual void OnTimeout() override;
private:
    Ui::ReadSSCard *ui;
};

#endif // UP_READSSCARD_H
