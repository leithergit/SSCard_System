#ifndef DIALOGIDCARDINFO_H
#define DIALOGIDCARDINFO_H

#include <QDialog>
#include "Gloabal.h"

namespace Ui {
class DialogIDCardInfo;
}

class DialogIDCardInfo : public QDialog
{
    Q_OBJECT

public:
    explicit DialogIDCardInfo(IDCardInfo& IDCard,QWidget *parent = nullptr);
    ~DialogIDCardInfo();

private slots:
    void on_pushButton_OK_clicked();

private:
    Ui::DialogIDCardInfo *ui;
};

#endif // DIALOGIDCARDINFO_H
