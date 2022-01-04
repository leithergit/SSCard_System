#ifndef SYS_OTHEROPTIONS_H
#define SYS_OTHEROPTIONS_H

#include <QWidget>

namespace Ui {
class OtherOptions;
}

class OtherOptions : public QWidget
{
    Q_OBJECT

public:
    explicit OtherOptions(QWidget *parent = nullptr);
    ~OtherOptions();

private slots:
    void on_pushButton_TestUploadLog_clicked();

private:
    Ui::OtherOptions *ui;
};

#endif // SYS_OTHEROPTIONS_H
