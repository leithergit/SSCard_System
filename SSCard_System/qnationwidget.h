#ifndef QNATIONWIDGET_H
#define QNATIONWIDGET_H

#include <QWidget>

namespace Ui {
class QNationWidget;
}

class QNationWidget : public QWidget
{
    Q_OBJECT

public:
    explicit QNationWidget(QWidget *parent = nullptr);
    ~QNationWidget();

private slots:
    void on_pushButton_Ok_clicked();

private:
    Ui::QNationWidget *ui;
};

#endif // QNATIONWIDGET_H
