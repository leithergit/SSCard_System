#ifndef NEWCARD_H
#define NEWCARD_H
#pragma execution_character_set("utf-8")

#include <QWidget>
#include "qmainstackpage.h"
#include "qstackpage.h"
#include "QStackedWidget"

namespace Ui {
    class NewCard;
}
class NewCard : public QMainStackPage
{
    Q_OBJECT
public:
    NewCard(QWidget* parent = nullptr);
    ~NewCard();
    virtual void OnTimerEvent() override;
    virtual void  ShowTimeOut(int nTimeout) override;
private slots:

private:
    Ui::NewCard* ui;
};

#endif // NEWCARD_H
