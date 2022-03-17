#ifndef WIDGET_H
#define WIDGET_H

#include <QWidget>

QT_BEGIN_NAMESPACE
namespace Ui { class Widget; }
QT_END_NAMESPACE

class Widget : public QWidget
{
    Q_OBJECT

public:
    Widget(QWidget *parent = nullptr);
    ~Widget();

private slots:
    void on_pushButton_Open_clicked();

    void on_pushButton_Close_clicked();

    void on_pushButton_Query_clicked();

    void on_pushButton_Insert_clicked();

    void on_pushButton_Update_clicked();

    void on_pushButton_Delete_clicked();

private:
    Ui::Widget *ui;
};
#endif // WIDGET_H
