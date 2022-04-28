#ifndef UC_FACECAPTURE_OCX_H
#define UC_FACECAPTURE_OCX_H
#pragma execution_character_set("utf-8")
#include <QWidget>
#include "qstackpage.h"
#include "../SDK/dvtldcamocx/dvtldcamocxlib.h"

namespace Ui {
	class FaceCapture_ocx;
}

class uc_FaceCapture_ocx : public QStackPage
{
	Q_OBJECT

public:
	explicit uc_FaceCapture_ocx(QLabel* pTitle, QString strStepImage, Page_Index nIndex, QWidget* parent = nullptr);
	~uc_FaceCapture_ocx();
	virtual int ProcessBussiness() override;
	virtual void OnTimeout() override;
	DVTLDCamOCXLib::DVTLDCamOCX* m_pFaceDetectOcx = nullptr;
	//QImage* m_pImageFaceDetected = nullptr;
	void    ShutDownDevice();
	virtual void ShutDown() override;
	int    OpenCamara(QString& strError);
	int    CloseCamera(QString& strError);
	bool   m_bOuputProductInfo = false;
	// nFull	0为全屏图像，1即人脸
	int    GetFaceCaptureStorePath(QString& strFilePath, int bFull = 1);
	bool   m_bFaceDetectSucceed = false;
	bool   m_bDetectionStart = false;
	// nFull	0为全屏图像，1即人脸
	int   SaveImage(QString& strImagePath, QString& strMessage, int nFull = 1);
signals:
	void   FaceCaptureSucceed();
	void   FaceCaptureFailed();
private slots:
	void   OnLiveDetectStatusEvent(int nEventID, int nFrameStatus);

	void   OnFaceCaptureSucceed();
	void   OnFaceCaptureFailed();

private:
	Ui::FaceCapture_ocx* ui;
};

#endif // UC_FACECAPTURE_H
