﻿#ifndef UC_FACECAPTURE_H
#define UC_FACECAPTURE_H
#pragma execution_character_set("utf-8")
#include <QWidget>
#include "qstackpage.h"
#include "./SDK/dvtldcamocx/dvtldcamocxlib.h"

namespace Ui {
	class FaceCapture;
}

class uc_FaceCapture : public QStackPage
{
	Q_OBJECT

public:
	explicit uc_FaceCapture(QLabel* pTitle, int nTimeout = 30, QWidget* parent = nullptr);
	~uc_FaceCapture();
	virtual int ProcessBussiness() override;
	virtual void OnTimeout() override;
    DVTLDCamOCXLib::DVTLDCamOCX*m_pFaceDetectOcx = nullptr;
    QImage  *m_pImageFaceDetected = nullptr;
    void    ShutDownDevice();
    int    OpenCamara(QString &strError);
    int    CloseCamera(QString &strError);
    bool   m_bOuputProductInfo = false;
    int    GetFaceCaptureStorePath(QString &strFilePath);
    bool   m_bFaceDetectSucceed = false;

signals:
    void   FaceCaptureSucceed();
private slots:
    void   OnLiveDetectStatusEvent(int nEventID, int nFrameStatus);
    void   OnFaceCaptureSucceed();

private:
	Ui::FaceCapture* ui;
};

#endif // UC_FACECAPTURE_H