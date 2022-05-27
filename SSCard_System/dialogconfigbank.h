#ifndef DIALOGCONFIGBANK_H
#define DIALOGCONFIGBANK_H

#include <QDialog>

namespace Ui {
class DialogConfigBank;
}

class DialogConfigBank : public QDialog
{
    Q_OBJECT

public:
    explicit DialogConfigBank(QWidget *parent = nullptr);
    ~DialogConfigBank();

private slots:
    void on_pushButton_OK_clicked();

    void on_pushButton_Cancel_clicked();

private:
    Ui::DialogConfigBank *ui;
};

#endif // DIALOGCONFIGBANK_H
