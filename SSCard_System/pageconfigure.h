#ifndef PAGECONFIGURE_H
#define PAGECONFIGURE_H

#include <QWidget>

namespace Ui {
class PageConfigure;
}

class PageConfigure : public QWidget
{
    Q_OBJECT

public:
    explicit PageConfigure(QWidget *parent = nullptr);
    ~PageConfigure();

private:
    Ui::PageConfigure *ui;
};

#endif // PAGECONFIGURE_H
