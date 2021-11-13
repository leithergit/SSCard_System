#ifndef UP_CHANGEPWD_H
#define UP_CHANGEPWD_H

#include <QWidget>
#include "qstackpage.h"

namespace Ui {
class up_ChangePWD;
}

class up_ChangePWD :  public QStackPage
{
    Q_OBJECT

public:
    explicit up_ChangePWD(QLabel *pTitle,int nTimeout = 30,QWidget *parent = nullptr);
    ~up_ChangePWD();
    virtual int ProcessBussiness() override;
    virtual void OnTimeout() override;
private slots:
    void on_pushButton_ChangePassword_clicked();

private:
    Ui::up_ChangePWD *ui;
};

#endif // UP_CHANGEPWD_H
