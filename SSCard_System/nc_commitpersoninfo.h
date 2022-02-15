#ifndef NC_COMMITPERSONINFO_H
#define NC_COMMITPERSONINFO_H

#include <QWidget>
#include "qstackpage.h"

namespace Ui {
class nc_commitPersonInfo;
}

class nc_commitPersonInfo : public QStackPage
{
    Q_OBJECT

public:
    explicit nc_commitPersonInfo(QWidget *parent = nullptr);
    ~nc_commitPersonInfo();

private slots:
    void on_pushButton_OK_clicked();

private:
    Ui::nc_commitPersonInfo *ui;
};

#endif // NC_COMMITPERSONINFO_H
