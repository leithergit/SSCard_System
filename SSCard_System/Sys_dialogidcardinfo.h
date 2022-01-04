#ifndef SYS_DIALOGIDCARDINFO_H
#define SYS_DIALOGIDCARDINFO_H

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

#endif // SYS_DIALOGIDCARDINFO_H
