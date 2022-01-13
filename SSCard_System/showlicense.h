#ifndef SHOWLICENSE_H
#define SHOWLICENSE_H

#include <QWidget>

namespace Ui {
class ShowLicense;
}

class ShowLicense : public QWidget
{
    Q_OBJECT

public:
    explicit ShowLicense(QWidget *parent = nullptr);
    ~ShowLicense();

private slots:
    void on_pushButton_OK_clicked();

private:
    Ui::ShowLicense *ui;
};

#endif // SHOWLICENSE_H
