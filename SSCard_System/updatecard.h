#ifndef UPDATECARD_H
#define UPDATECARD_H

#include <QWidget>
#include "qmainstackpage.h"
#include "qstackpage.h"
#include "QStackedWidget"
#include "uc_inputmobile.h"

enum    UpdateCard_Widget
{
    _ReadIDCard = 0,
    _FaceCapture,
    _EnsureInformation,
    _InputMobile,
    _Pay,
    _MakeCard,
    _AdforFinance,
    _UpdateCardSucceed
};

namespace Ui {
class UpdateCard;
}

class UpdateCard : public QMainStackPage
{
    Q_OBJECT

public:
    explicit UpdateCard(QWidget *parent = nullptr);
    ~UpdateCard();
    virtual void OnTimerEvent() override;
    uc_InputMobile *m_pInputMobile = nullptr;

private slots:
    //void on_pushButton_MainPage_clicked();
    //virtual void on_SwitchNextPage() override;

private:
    Ui::UpdateCard *ui;
};

#endif // UPDATECARD_H
