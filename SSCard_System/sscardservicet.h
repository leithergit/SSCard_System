#ifndef SSCARDSERVICET_H
#define SSCARDSERVICET_H

#include <QWidget>
#include "../SSCardService/SSCardService.h"

namespace Ui {
class SSCardServiceT;
}

class SSCardServiceT : public QWidget
{
    Q_OBJECT

public:
    explicit SSCardServiceT(QWidget *parent = nullptr);
    ~SSCardServiceT();

private slots:
    void on_pushButton_LoadCardID_clicked();

    void on_pushButton_QueryCardStatus_clicked();

    void on_pushButton_RestisterLost_clicked();

    void on_pushButton_CommitInfo_clicked();

    void on_pushButton_CreateService_clicked();

    void on_pushButton_PremakeCard_clicked();

    void on_pushButton_WriteCard_clicked();

    void on_pushButton_ReturnCard_clicked();

    void on_pushButton_ActiveCard_clicked();

    void on_pushButton_LoadPreMakeCard_clicked();

private:
    Ui::SSCardServiceT *ui;
};

#endif // SSCARDSERVICET_H
