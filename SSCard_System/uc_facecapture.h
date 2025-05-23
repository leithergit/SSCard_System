﻿#ifndef UC_FACECAPTURE_H
#define UC_FACECAPTURE_H
#pragma execution_character_set("utf-8")
#include <QWidget>
#include "qstackpage.h"

namespace Ui {
	class FaceCapture;
}

class uc_FaceCapture : public QStackPage
{
	Q_OBJECT

public:
	explicit uc_FaceCapture(QLabel* pTitle, QString strStepImage, Page_Index nIndex, QWidget* parent = nullptr);
	~uc_FaceCapture();
	virtual int ProcessBussiness() override;
	virtual void OnTimeout() override;
	virtual void ShutDown() override;
	// DVTLDCamOCXLib::DVTLDCamOCX* m_pFaceDetectOcx = nullptr;
	//QImage* m_pImageFaceDetected = nullptr;
	void    ShutDownDevice();

	int    OpenCamara(QString& strError);
	int    CloseCamera(QString& strError);
	bool   m_bOuputProductInfo = false;
	// nFull	0为全屏图像，1即人脸
	int    GetFaceCaptureStorePath(QString& strFilePath, bool bFull = 1);
	bool   m_bFaceDetectSucceed = false;
	bool   m_bDetectionStart = false;
	// nFull	true为全屏图像，false即人脸
	int   SaveImage(QString& strImagePath, QString& strMessage, bool nFull = false);

private slots:
	void   OnLiveDetectStatusEvent(int nEventID, int nFrameStatus);

	void   OnFaceCaptureSucceed();
	void   OnFaceCaptureFailed();

private:
	Ui::FaceCapture* ui;
};

#endif // UC_FACECAPTURE_H
