#ifndef CARDMANGER_H
#define CARDMANGER_H

#include <QWidget>

namespace Ui {
class CardManger;
}

class CardManger : public QWidget
{
    Q_OBJECT

public:
    explicit CardManger(QWidget *parent = nullptr);
    ~CardManger();

private:
    Ui::CardManger *ui;
};

#endif // CARDMANGER_H
