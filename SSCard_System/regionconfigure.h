#ifndef REGIONCONFIGURE_H
#define REGIONCONFIGURE_H

#include <QWidget>

namespace Ui {
class RegionConfigure;
}

class RegionConfigure : public QWidget
{
    Q_OBJECT

public:
    explicit RegionConfigure(QWidget *parent = nullptr);
    ~RegionConfigure();

private:
    Ui::RegionConfigure *ui;
};

#endif // REGIONCONFIGURE_H
