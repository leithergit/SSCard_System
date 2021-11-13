#ifndef UC_INPUTMOBILE_H
#define UC_INPUTMOBILE_H

#include <QWidget>
#include "qstackpage.h"

#define MaxMobileSize   11

namespace Ui {
class InputMobile;
}

class uc_InputMobile : public QStackPage
{
    Q_OBJECT

public:
    explicit uc_InputMobile(QLabel *pTitle,int nTimeout = 60,QWidget *parent = nullptr);
    ~uc_InputMobile();
    virtual int ProcessBussiness() override;
    virtual void OnTimeout() override;
    QString  m_strMobile = "";
private slots:
    void on_pushButton_0_clicked();
    void on_pushButton_1_clicked();
    void on_pushButton_2_clicked();
    void on_pushButton_3_clicked();
    void on_pushButton_4_clicked();
    void on_pushButton_5_clicked();
    void on_pushButton_6_clicked();
    void on_pushButton_7_clicked();
    void on_pushButton_8_clicked();
    void on_pushButton_9_clicked();

    void on_pushButton_Clear_clicked();

    void on_pushButton_Backspace_clicked();

    void on_pushButton_Ensure_clicked();

private:
    Ui::InputMobile *ui;
};

#endif // UC_INPUTMOBILE_H
