#ifndef SYS_DIALOGCAMERATEST_H
#define SYS_DIALOGCAMERATEST_H

#include <QDialog>
#include "./SDK/dvtldcamocx/dvtldcamocxlib.h"
#include <QImage>

namespace Ui {
class DialogCameraTest;
}

class DialogCameraTest : public QDialog
{
    Q_OBJECT

public:
    explicit DialogCameraTest(QWidget *parent = nullptr);
    ~DialogCameraTest();
    DVTLDCamOCXLib::DVTLDCamOCX* m_pFaceDetectOcx = nullptr;
    QImage  ImageDetected;
    void OpenCamera();

    void CloseCamera();

private slots:
    void on_pushButton_clicked();
    void OnLiveDetectStatusEvent(int nEventID, int nFrameStatus);

private:
    Ui::DialogCameraTest *ui;
};

#endif // SYS_DIALOGCAMERATEST_H
