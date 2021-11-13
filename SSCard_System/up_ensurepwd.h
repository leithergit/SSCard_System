#ifndef UP_ENSUREPWD_H
#define UP_ENSUREPWD_H

#include <QWidget>
#include "qstackpage.h"

namespace Ui {
class EnsurePWD;
}

class up_EnsurePWD :  public QStackPage
{
    Q_OBJECT

public:
    explicit up_EnsurePWD(QLabel *pTitle,int nTimeout = 30,QWidget *parent = nullptr);
    ~up_EnsurePWD();
    virtual int ProcessBussiness() override;
    virtual void OnTimeout() override;

private slots:


    void on_pushButton_EnsurePWD_clicked();

private:
    Ui::EnsurePWD *ui;
};

#endif // UP_ENSUREPWD_H
