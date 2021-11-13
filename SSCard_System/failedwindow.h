#ifndef FAILEDWINDOW_H
#define FAILEDWINDOW_H

#include <QWidget>

namespace Ui {
class FailedWindow;
}

class FailedWindow : public QWidget
{
    Q_OBJECT

public:
    explicit FailedWindow(QWidget *parent = nullptr);
    ~FailedWindow();
    virtual void timerEvent(QTimerEvent *event);
    void Popup(QString strTitle,int nTimeout = 5);
signals:
    void FailedWindowTimeout();
private:
    QString m_strFailedText = "";
    int m_nTimerID = 0;
    int m_nTimeout = 0;
    Ui::FailedWindow *ui;
};

#endif // FAILEDWINDOW_H
