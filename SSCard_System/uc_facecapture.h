#ifndef UC_FACECAPTURE_H
#define UC_FACECAPTURE_H

#include <QWidget>
#include "qstackpage.h"

namespace Ui {
class FaceCapture;
}

class uc_FaceCapture : public QStackPage
{
    Q_OBJECT

public:
    explicit uc_FaceCapture(QLabel *pTitle,int nTimeout = 30,QWidget *parent = nullptr);
    ~uc_FaceCapture();
    virtual int ProcessBussiness() override;
    virtual void OnTimeout() override;
private:
    Ui::FaceCapture *ui;
};

#endif // UC_FACECAPTURE_H
