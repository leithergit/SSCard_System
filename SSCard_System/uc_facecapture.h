#ifndef UC_FACECAPTURE_H
#define UC_FACECAPTURE_H
#pragma execution_character_set("utf-8")
#include <QWidget>
#include "qstackpage.h"
#include "./SDK/dvtldcamocx/dvtldcamocxlib.h"
#include "ArcFaceEngine.h"

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
	virtual void timerEvent(QTimerEvent* event) override;
	DVTLDCamOCXLib::DVTLDCamOCX* m_pFaceDetectOcx = nullptr;
	//QImage* m_pImageFaceDetected = nullptr;
	void    ShutDownDevice();
	virtual void ShutDown() override;
	//int    OpenCamara(QString& strError);
	//int    CloseCamera(QString& strError);
	int    OpenCamara(/*int nIR_CameraID, int nRGBCameraID,*/ QString& strError);
	int    CloseCamera(QString& strError);
	bool   m_bOuputProductInfo = false;
	// nFull	0为全屏图像，1即人脸
	int    GetFaceCaptureStorePath(QString& strFilePath, int bFull = 1);
	bool   m_bFaceDetectSucceed = false;
	bool   m_bDetectionStart = false;
	int m_nTimerFaceDetect = 0;
	int m_nTimeoutFaceDetect = 25;
	std::thread ThreadShutdown;
	void ShutDownThread();
	// nFull	0为全屏图像，1即人脸

	int   SaveImage(QString& strImagePath, QString& strMessage, int nFull = 1);
public:

	void ThreadFaceFeature();
	int OpenCamera(/*int nIR_CameraID, int nRGBCameraID,*/ QString& strMessage);
	void CloseCamera();

	BOOL m_bLoadIconThreadRunning;
	BOOL m_bClearFeatureThreadRunning;

	string m_strEditThreshold;
	int RegisterFace(string strFaceFile);

	string m_curVideoShowString;
	string m_curIRVideoShowString;

signals:
	void   FaceCaptureSucceed();
	void   FaceCaptureFailed();
	void   PopMessage(QString strMessage, QMessageBox::Icon nIcon = QMessageBox::Icon::Information);
	void   UpdateVideo();
private slots:
	void   OnLiveDetectStatusEvent(int nEventID, int nFrameStatus);

	void   OnFaceCaptureSucceed();
	void   OnFaceCaptureFailed();
	void   on_PopMessage(QString strMessage, QMessageBox::Icon nIcon);
	void   on_UpdateVideo();

private:
	Ui::FaceCapture* ui;
};

#endif // UC_FACECAPTURE_H
