#include "widget.h"
#include "ui_widget.h"

Widget::Widget(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::Widget)
{
    ui->setupUi(this);
}

Widget::~Widget()
{
    delete ui;
}


void Widget::on_pushButton_Open_clicked()
{

}


void Widget::on_pushButton_Close_clicked()
{

}


void Widget::on_pushButton_Query_clicked()
{

}


void Widget::on_pushButton_Insert_clicked()
{

}


void Widget::on_pushButton_Update_clicked()
{

}


void Widget::on_pushButton_Delete_clicked()
{

}

