#ifndef LOGMANAGER_H
#define LOGMANAGER_H

#include <QWidget>

namespace Ui {
class logManager;
}

class logManager : public QWidget
{
    Q_OBJECT

public:
    explicit logManager(QWidget *parent = nullptr);
    ~logManager();

private:
    Ui::logManager *ui;
};

#endif // LOGMANAGER_H
