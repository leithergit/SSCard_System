#ifndef UPDATEPASSWORD_H
#define UPDATEPASSWORD_H

#include <QWidget>
#include "qmainstackpage.h"
#include "qstackpage.h"
#include "QStackedWidget"

enum    UpdatePassword_Widget
{
    _ReadSSCard = 0,
    _EnsurePWD,
    _ChangePWD,
};

namespace Ui {
class UpdatePassword;
}

class UpdatePassword : public QMainStackPage
{
    Q_OBJECT

public:
    explicit UpdatePassword(QWidget *parent = nullptr);
    ~UpdatePassword();
     virtual void OnTimerEvent() override;

private slots:

private:
    Ui::UpdatePassword *ui;
};

#endif // UPDATEPASSWORD_H
