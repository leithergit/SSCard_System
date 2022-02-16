#ifndef NC_MAKEPHOTO_H
#define NC_MAKEPHOTO_H

#include <QWidget>
#include "qstackpage.h"
#include "../SDK/dvtldcamocx/dvtldcamocxlib.h"
namespace Ui {
class nc_MakePhoto;
}

class nc_MakePhoto : public QStackPage
{
    Q_OBJECT

public:
    explicit nc_MakePhoto(QLabel* pTitle, QString strStepImage, Page_Index nIndex, QWidget* parent = nullptr);
    ~nc_MakePhoto();
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
    Ui::nc_MakePhoto *ui;
};

#endif // NC_MAKEPHOTO_H
