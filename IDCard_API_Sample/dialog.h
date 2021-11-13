#ifndef DIALOG_H
#define DIALOG_H

#include <QDialog>

QT_BEGIN_NAMESPACE
namespace Ui { class Dialog; }
QT_END_NAMESPACE


class Dialog : public QDialog
{
    Q_OBJECT

public:
    Dialog(QWidget *parent = nullptr);
    ~Dialog();


    void timerEvent(QTimerEvent *event);

    int m_nTimerID = -1;
    int nPort = -1;
    int     nDelayCount = 0;
    int     bSucceed = false;

private slots:
    void on_pushButton_Start_clicked();

    void on_pushButton_Stop_clicked();

private:
    Ui::Dialog *ui;
};
#endif // DIALOG_H
