#ifndef SYS_READIDCARD_H
#define SYS_READIDCARD_H

#include <QWidget>

namespace Ui {
class Sys_ReadIDCard;
}

class Sys_ReadIDCard : public QWidget
{
    Q_OBJECT

public:
    explicit Sys_ReadIDCard(QWidget *parent = nullptr);
    ~Sys_ReadIDCard();

private:
    Ui::Sys_ReadIDCard *ui;
};

#endif // SYS_READIDCARD_H
