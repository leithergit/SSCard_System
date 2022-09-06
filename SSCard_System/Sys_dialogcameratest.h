#ifndef SYS_DIALOGCAMERATEST_H
#define SYS_DIALOGCAMERATEST_H

#include <QDialog>
//#include "../SDK/dvtldcamocx/dvtldcamocxlib.h"
#include <QImage>
#include <QButtonGroup>

namespace Ui {
	class DialogCameraTest;
}

class DialogCameraTest : public QDialog
{
	Q_OBJECT

public:

    explicit DialogCameraTest(bool bShowGuardian = false, QWidget* parent = nullptr);
	~DialogCameraTest();
	//DVTLDCamOCXLib::DVTLDCamOCX* m_pFaceDetectOcx = nullptr;
	QImage  ImageDetected;
	int    OpenCamara(QString& strError);
	int    CloseCamera(QString& strError);
	QString strHeaderPhoto;
    QButtonGroup* pButtonGrp = nullptr;

private slots:
	void OnLiveDetectStatusEvent(int nEventID, int nFrameStatus);
	//void   OnFaceCaptureSucceed();
	//void   OnFaceCaptureFailed();
	void on_pushButton_TakePhoto_clicked();

    void on_pushButton_OK_clicked();

private:
	Ui::DialogCameraTest* ui;
};

#endif // SYS_DIALOGCAMERATEST_H
