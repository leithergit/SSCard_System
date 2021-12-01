#ifndef DIALOG_H
#define DIALOG_H

#include <QDialog>
#include <thread>
using namespace std;

QT_BEGIN_NAMESPACE
namespace Ui { class Dialog; }
QT_END_NAMESPACE

class Dialog : public QDialog
{
    Q_OBJECT

public:
    Dialog(QWidget *parent = nullptr);
    ~Dialog();
    uint ThreadUpdatePin();
    std::thread  ThreadUpdate;
    bool    bThreadUpdatePinRun = false;
    QString     strPin;
    uchar   szPin[32];
    int     nPinSize = 0;

signals:
    void UpdatePin(char ch);

private slots:
    void on_pushButton_Open_clicked();

    void on_pushButton_Close_clicked();

    void OnUpdatePin(char ch);


private:
    Ui::Dialog *ui;
};

#endif // DIALOG_H
