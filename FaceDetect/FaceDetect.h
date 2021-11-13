#pragma once

#include <QtWidgets/QDialog>
#include "ui_FaceDetect.h"
#include "dvtldcamocxlib.h"
#include <QImage>

enum FaceDetectStatus
{
    FD_Succeed,
    FD_Binocularcamera_OpenFailed,
};
class FaceDetect : public QDialog
{
    Q_OBJECT

public:
    int     GetErrorMessage(int nErrorCode, QString strMessage);
    FaceDetect(QWidget *parent = Q_NULLPTR);
    DVTLDCamOCXLib::DVTLDCamOCX*m_pFaceDetectOcx = nullptr;
    QVBoxLayout *pVerticalLayout = nullptr;
    QImage  ImageDetected;
private slots:
    void on_pushButton_CameraOpen_clicked();

    void on_pushButton_DetectStart_clicked();

    void on_pushButton_DetectStop_clicked();

    void on_pushButton_CameraClose_clicked();

    void on_pushButton_PhotoCapture_clicked();

    void on_pushButton_PhotoBrowse_clicked();

    void on_pushButton_PhotoCompare_clicked();

    void OnLiveDetectStatusEvent(int nEventID, int nFrameStatus);

private:
    Ui::FaceDetectClass ui;
};
