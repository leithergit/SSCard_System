#ifndef QWIDGETREADIDCARD_H
#define QWIDGETREADIDCARD_H
#include "Gloabal.h"

#include <QWidget>

namespace Ui {
class QWidgetReadIDCard;
}

class QWidgetReadIDCard : public QWidget
{
    Q_OBJECT

public:
    explicit QWidgetReadIDCard(IDCardInfo &CardInfo, QWidget *parent = nullptr);
    ~QWidgetReadIDCard();

private slots:
    void on_pushButton_OK_clicked();

private:
    Ui::QWidgetReadIDCard *ui;
};

#endif // QWIDGETREADIDCARD_H
