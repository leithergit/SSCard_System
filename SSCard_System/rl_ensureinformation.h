#ifndef RL_ENSUREINFORMATION_H
#define RL_ENSUREINFORMATION_H

#include <QWidget>
#include "qstackpage.h"

namespace Ui {
class rl_EnsureInformation;
}

class rl_EnsureInformation : public QStackPage
{
    Q_OBJECT

public:
    explicit rl_EnsureInformation(QLabel *pTitle,int nTimeout = 30,QWidget *parent = nullptr);
    ~rl_EnsureInformation();
    virtual int ProcessBussiness() override;
    virtual void OnTimeout() override;
private slots:
    void on_pushButton_MainPage_clicked();

private:
    Ui::rl_EnsureInformation *ui;
};

#endif // RL_ENSUREINFORMATION_H
