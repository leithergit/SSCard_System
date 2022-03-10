#pragma execution_character_set("utf-8")

#include "uc_facecapture.h"
#include "ui_uc_facecapture.h"
#include <QCoreApplication>
#include <QObject>
#include <QMovie>

#include <strmif.h>
#include <initguid.h>
#include "Gloabal.h"
#include "opencv/cv.h"
#include "opencv/highgui.h"
#include <opencv2\opencv.hpp>

#define RGB_TYPE 3              //R,G,B三通道
#define GRAY_TYPE 1             //格雷图，单通道

using namespace std;

#define VIDEO_FRAME_DEFAULT_WIDTH 1280
#define VIDEO_FRAME_DEFAULT_HEIGHT 720

#define FACE_FEATURE_SIZE 1032

#define THUMBNAIL_WIDTH  55
#define THUMBNAIL_HEIGHT  55
#define Threshold 0.80

#define VI_MAX_CAMERAS 20
DEFINE_GUID(CLSID_SystemDeviceEnum, 0x62be5d10, 0x60eb, 0x11d0, 0xbd, 0x3b, 0x00, 0xa0, 0xc9, 0x11, 0xce, 0x86);
DEFINE_GUID(CLSID_VideoInputDeviceCategory, 0x860bb310, 0x5d01, 0x11d0, 0xbd, 0x3b, 0x00, 0xa0, 0xc9, 0x11, 0xce, 0x86);
DEFINE_GUID(IID_ICreateDevEnum, 0x29840822, 0x5b84, 0x11d0, 0xbd, 0x3b, 0x00, 0xa0, 0xc9, 0x11, 0xce, 0x86);

#define SafeFree(p) { if ((p)) free(p); (p) = NULL; }
#define SafeArrayDelete(p) { if ((p)) delete [] (p); (p) = NULL; } 
#define SafeDelete(p) { if ((p)) delete (p); (p) = NULL; } 

ArcFaceEngine g_imageFaceEngine;
ArcFaceEngine g_videoFaceEngine;
cv::VideoCapture g_IR_Camera;
cv::VideoCapture g_RGB_Camera;
list<IplImage*> ListImage;
std::mutex mutexListImage;
bool g_bActiveFaceEngine = false;
int  g_nRGB_CameraID = -1;
int  g_nIR_CameraID = -1;
BOOL g_bFDThreadRunning = FALSE;
BOOL g_bFRThreadRunning = FALSE;
int g_nCameraCount = 0;
std::thread* g_pThreadFaceDetect = nullptr;
std::thread* g_pThreadFaceFeature = nullptr;
ASF_SingleFaceInfo g_curFaceInfo;
std::vector<ASF_FaceFeature> g_vceFacefeatures;
ASF_FaceFeature g_curStaticImageFeature;	//当前图片的人脸特征
BOOL g_curStaticImageFRSucceed = FALSE;
IplImage* g_curVideoImage = nullptr;
IplImage* g_curIrVideoImage = nullptr;
QImage* g_pQImage = nullptr;
bool g_dataValid = false;
bool g_irDataValid = false;
bool g_videoOpened = false;
std::mutex g_mutexVideo;
int nRGBLiveThreshold;
int nIR_LiveThreshold;
void IplImageToQImage(IplImage* srcImage, QImage*);
void ThreadFaceDetect(QStackPage* p);

//列出硬件设备
int EnumCameras(vector<string>& list)
{
	ICreateDevEnum* pDevEnum = NULL;
	IEnumMoniker* pEnum = NULL;
	int deviceCounter = 0;
	CoInitialize(NULL);

	HRESULT hr = CoCreateInstance(
		CLSID_SystemDeviceEnum,
		NULL,
		CLSCTX_INPROC_SERVER,
		IID_ICreateDevEnum,
		reinterpret_cast<void**>(&pDevEnum)
	);

	if (SUCCEEDED(hr))
	{
		hr = pDevEnum->CreateClassEnumerator(CLSID_VideoInputDeviceCategory, &pEnum, 0);
		if (hr == S_OK) {

			IMoniker* pMoniker = NULL;
			while (pEnum->Next(1, &pMoniker, NULL) == S_OK)
			{
				IPropertyBag* pPropBag;
				hr = pMoniker->BindToStorage(0, 0, IID_IPropertyBag,
					(void**)(&pPropBag));

				if (FAILED(hr)) {
					pMoniker->Release();
					continue; // Skip this one, maybe the next one will work.
				}

				VARIANT varName;
				VariantInit(&varName);
				hr = pPropBag->Read(L"Description", &varName, 0);
				if (FAILED(hr))
				{
					hr = pPropBag->Read(L"FriendlyName", &varName, 0);
				}

				if (SUCCEEDED(hr))
				{
					hr = pPropBag->Read(L"FriendlyName", &varName, 0);
					int count = 0;
					char tmp[255] = { 0 };
					while (varName.bstrVal[count] != 0x00 && count < 255)
					{
						tmp[count] = (char)varName.bstrVal[count];
						count++;
					}
					list.push_back(tmp);
				}

				pPropBag->Release();
				pPropBag = NULL;
				pMoniker->Release();
				pMoniker = NULL;

				deviceCounter++;
			}

			pDevEnum->Release();
			pDevEnum = NULL;
			pEnum->Release();
			pEnum = NULL;
		}
	}
	return deviceCounter;
}

void ReadSetting(char* appID, char* sdkKey, char* activeKey, char* tag,
	char* rgbLiveThreshold, char* irLiveThreshold, char* rgbCameraId, char* irCameraId)
{
	const char* iniPath = ".\\setting.ini";

	char resultStr[MAX_PATH] = "";

	GetPrivateProfileStringA("tag", "tag", NULL, resultStr, MAX_PATH, iniPath);
	memcpy(tag, resultStr, MAX_PATH);

	GetPrivateProfileStringA(tag, "APPID", NULL, resultStr, MAX_PATH, iniPath);
	memcpy(appID, resultStr, MAX_PATH);

	GetPrivateProfileStringA(tag, "SDKKEY", NULL, resultStr, MAX_PATH, iniPath);
	memcpy(sdkKey, resultStr, MAX_PATH);

	GetPrivateProfileStringA(tag, "ACTIVE_KEY", NULL, resultStr, MAX_PATH, iniPath);
	memcpy(activeKey, resultStr, MAX_PATH);

	GetPrivateProfileStringA(tag, "rgbLiveThreshold", NULL, resultStr, MAX_PATH, iniPath);
	memcpy(rgbLiveThreshold, resultStr, MAX_PATH);

	GetPrivateProfileStringA(tag, "irLiveThreshold", NULL, resultStr, MAX_PATH, iniPath);
	memcpy(irLiveThreshold, resultStr, MAX_PATH);

	GetPrivateProfileStringA(tag, "rgbCameraId", NULL, resultStr, MAX_PATH, iniPath);
	memcpy(rgbCameraId, resultStr, MAX_PATH);

	GetPrivateProfileStringA(tag, "irCameraId", NULL, resultStr, MAX_PATH, iniPath);
	memcpy(irCameraId, resultStr, MAX_PATH);
}

//#include "mainwindow.h"

uc_FaceCapture::uc_FaceCapture(QLabel* pTitle, QString strStepImage, Page_Index nIndex, QWidget* parent) :
	QStackPage(pTitle, strStepImage, nIndex, parent),
	ui(new Ui::FaceCapture)
{
	ui->setupUi(this);
	qsrand(time(0));
	//QMovie* movie = new QMovie("./Image/FaceDetect.gif");
	//QSize si(ui->label_FaceDetect->width(), ui->label_FaceDetect->height());
	//movie->setScaledSize(si);
	//ui->label_FaceDetect->setMovie(movie);
	//movie->start();
	//connect(this, &uc_FaceCapture::FaceCaptureSucceed, this, &uc_FaceCapture::OnFaceCaptureSucceed);
	//connect(this, &uc_FaceCapture::FaceCaptureFailed, this, &uc_FaceCapture::OnFaceCaptureFailed);

	char tag[MAX_PATH] = "";
	char appID[MAX_PATH] = "";
	char  sdkKey[MAX_PATH] = "";
	char  activeKey[MAX_PATH] = "";
	char rgbLiveThreshold[MAX_PATH] = "";
	char irLiveThreshold[MAX_PATH] = "";
	char rgbCameraId[MAX_PATH] = "";
	char irCameraId[MAX_PATH] = "";
	if (!g_bActiveFaceEngine)
	{
		ReadSetting(appID, sdkKey, activeKey, tag, rgbLiveThreshold, irLiveThreshold, rgbCameraId, irCameraId);

		g_nRGB_CameraID = atoi(rgbCameraId);
		g_nIR_CameraID = atoi(irCameraId);

		nRGBLiveThreshold = atof(rgbLiveThreshold);
		nIR_LiveThreshold = atof(irLiveThreshold);

		MRESULT faceRes = g_imageFaceEngine.ActiveSDK(appID, sdkKey, activeKey);

		//获取激活文件信息
		ASF_ActiveFileInfo activeFileInfo = { 0 };
		g_imageFaceEngine.GetActiveFileInfo(activeFileInfo);

		if (faceRes == MOK)
		{
			faceRes = g_imageFaceEngine.InitEngine(ASF_DETECT_MODE_IMAGE);//Image
			QString resStr = QString("IMAGE模式下初始化结果: %1").arg(faceRes);

			resStr = "";
			faceRes = g_videoFaceEngine.InitEngine(ASF_DETECT_MODE_VIDEO);//Video
			resStr = QString("VIDEO模式下初始化结果: %1").arg(faceRes);

		}

		g_curStaticImageFeature.featureSize = FACE_FEATURE_SIZE;
		g_curStaticImageFeature.feature = (MByte*)malloc(g_curStaticImageFeature.featureSize * sizeof(MByte));
		g_bActiveFaceEngine = true;
	}

	//connect(this, &uc_FaceCapture::UpdateVideo, this, &uc_FaceCapture::on_UpdateVideo, Qt::BlockingQueuedConnection);
	//connect(this, &uc_FaceCapture::PopMessage, this, &uc_FaceCapture::on_PopMessage);
}

uc_FaceCapture::~uc_FaceCapture()
{
	ShutDownDevice();
	delete ui;
}

void  uc_FaceCapture::ShutDownDevice()
{

	QString strError;
	CloseCamera(strError);

}

void uc_FaceCapture::ShutDownThread()
{

}

void uc_FaceCapture::ShutDown()
{
	gInfo() << __FUNCTION__;
	g_bFDThreadRunning = FALSE;
	disconnect(this, &uc_FaceCapture::UpdateVideo, this, &uc_FaceCapture::on_UpdateVideo);
	disconnect(this, &uc_FaceCapture::PopMessage, this, &uc_FaceCapture::on_PopMessage);

	if (g_pThreadFaceDetect)
	{
		g_pThreadFaceDetect->join();
		delete g_pThreadFaceDetect;
		g_pThreadFaceDetect = nullptr;
		qDebug() << __FUNCTION__ << " ThreadFaceDetect exit!";
	}

	//ShutDownDevice();		// 不关闭摄像机，防止后开摄像机等待时间过长
	//if (m_pFaceDetectOcx)
	//	m_pFaceDetectOcx->EndLiveDectection();
}

int uc_FaceCapture::ProcessBussiness()
{
	m_bFaceDetectSucceed = false;
	QString strError;
	int nMiniTime = 10;
	int nMaxTime = 20;
	m_nTimeoutFaceDetect = qrand() % (nMaxTime - nMiniTime);
	m_nTimeoutFaceDetect += nMiniTime;
	m_nTimerFaceDetect = startTimer(1000);
	if (QFailed(OpenCamara(/*nIR_CameraID, nRGB_CameraID, */strError)))
	{
		gError() << strError.toLocal8Bit().data();
		///QMessageBox::critical(this,tr("打开摄像机失败"),strError,QMessageBox::Ok);
		emit ShowMaskWidget("操作失败", strError, Fetal, Return_MainPage);
		return -1;
	}
	if (!g_pThreadFaceDetect)
	{
		connect(this, &uc_FaceCapture::UpdateVideo, this, &uc_FaceCapture::on_UpdateVideo);
		connect(this, &uc_FaceCapture::PopMessage, this, &uc_FaceCapture::on_PopMessage);
		g_bFDThreadRunning = true;
		g_pThreadFaceDetect = new std::thread(ThreadFaceDetect, this);
	}

	return 0;
}

void uc_FaceCapture::OnTimeout()
{
	//ShutDownDevice();
	ShutDown();
}

void uc_FaceCapture::timerEvent(QTimerEvent* event)
{
	if (event->timerId() == m_nTimerFaceDetect)
	{
		m_nTimeoutFaceDetect--;
		if (m_nTimeoutFaceDetect <= 1)
		{
			killTimer(m_nTimerFaceDetect);
			ShutDown();
			m_nTimeoutFaceDetect = 0;
			emit ShowMaskWidget("操作成功", "人脸识别成功,稍后请确认卡信息!", Success, Switch_NextPage);
		}
	}
}

//int uc_FaceCapture::OpenCamara(QString& strError)
//{
//	int nResult = 0;
//	do
//	{
//		if (!m_pFaceDetectOcx)
//		{
//			m_pFaceDetectOcx = new DVTLDCamOCXLib::DVTLDCamOCX(ui->label_FaceDetect);
//			if (!m_pFaceDetectOcx)
//			{
//				strError = "人脸检测组件初始化失败!";
//				nResult = -1;
//				break;
//			}
//			if (!m_bOuputProductInfo)
//			{
//				QString strProduct = "人脸检测组件产品信息:." + m_pFaceDetectOcx->GetProduct();
//				QString strVersion = "人脸检测组件版本:" + m_pFaceDetectOcx->GetVersion();
//				gInfo() << strProduct.toLocal8Bit().data();
//				gInfo() << strVersion.toLocal8Bit().data();
//				m_bOuputProductInfo = true;
//			}
//
//			m_pFaceDetectOcx->setObjectName(QString::fromUtf8("axWidget_FaceDetect"));
//			m_pFaceDetectOcx->setMinimumSize(ui->label_FaceDetect->size());
//			m_pFaceDetectOcx->setMaximumSize(ui->label_FaceDetect->size());
//			m_pFaceDetectOcx->show();
//			connect(m_pFaceDetectOcx, SIGNAL(LiveDetectStatusEvent(int, int)), this, SLOT(OnLiveDetectStatusEvent(int, int)));
//
//			//QString strDoc = m_pFaceDetectOcx->generateDocumentation();//导出所有ocx控件的所有信号、函数、属性等，可供开发参考
//			//QString strFaceDetectDoc = QDir::currentPath() +  "/axHelp.html";
//			//QFile file(strFaceDetectDoc);
//			//file.open(QIODevice::WriteOnly);
//			//file.write(strDoc.toLatin1(),strDoc.size());
//			//file.close();
//			int nResult = m_pFaceDetectOcx->OpenCamera();
//			if (nResult)
//			{
//				strError = QString("打开摄像机失败,错误代码:%1,请检查设备连接!").arg(nResult);
//				nResult = -1;
//				break;
//			}
//		}
//		m_pFaceDetectOcx->EndLiveDectection();
//
//		nResult = m_pFaceDetectOcx->StartLiveDetection(m_nTimeout);
//		if (nResult)
//		{
//			strError = QString("启动人脸检测失败,错误代码:%1,请检查设备连接!").arg(nResult);
//			nResult = -1;
//			break;
//		}
//		m_bDetectionStart = true;
//
//		//m_pImageFaceDetected = new QImage;
//	} while (0);
//	return nResult;
//}
//
//int uc_FaceCapture::CloseCamera(QString& strError)
//{
//	int nResult = 0;
//	do
//	{
//		if (!m_pFaceDetectOcx)
//			break;
//		nResult = m_pFaceDetectOcx->EndLiveDectection();
//		if (nResult)
//		{
//			strError = QString("停止人脸检测失败,错误代码:%1!").arg(nResult);
//			break;
//		}
//		nResult = m_pFaceDetectOcx->CloseCamera();
//		if (nResult)
//		{
//			strError = QString("关闭摄像机失败,错误代码:%1!").arg(nResult);
//			break;
//		}
//		/*if (m_pImageFaceDetected)
//		{
//			delete m_pImageFaceDetected;
//			m_pImageFaceDetected = nullptr;
//		}*/
//		delete m_pFaceDetectOcx;
//		m_pFaceDetectOcx = nullptr;
//
//	} while (0);
//	return nResult;
//}

int uc_FaceCapture::OpenCamara(/*int nIR_CameraID, int nRGB_CameraID,*/ QString& strMessage)
{
	vector<string> vecCamera;
	g_nCameraCount = EnumCameras(vecCamera);
	int nResult = -1;

	do
	{
		if (!g_nCameraCount)
		{
			strMessage = "未发现可用的摄像机!";
			break;
		}
		else
			g_nCameraCount = 1;
		if (g_nCameraCount >= 2)
		{
			if (!g_RGB_Camera.isOpened())
			{
				if (!g_RGB_Camera.open(g_nRGB_CameraID))
				{
					strMessage = "打开摄像机失败!";
					break;
				}

				if (!g_RGB_Camera.set(CV_CAP_PROP_FRAME_WIDTH, VIDEO_FRAME_DEFAULT_WIDTH) ||
					!g_RGB_Camera.set(CV_CAP_PROP_FRAME_HEIGHT, VIDEO_FRAME_DEFAULT_HEIGHT))
				{
					strMessage = "RGB摄像头初始化失败!";
					break;
				}
			}

			if (!g_IR_Camera.isOpened())
			{
				if (!g_IR_Camera.open(g_nIR_CameraID))
				{
					strMessage = "打开红外摄像机失败!";
					break;
				}

				if (!g_IR_Camera.set(CV_CAP_PROP_FRAME_WIDTH, VIDEO_FRAME_DEFAULT_WIDTH) ||
					!g_IR_Camera.set(CV_CAP_PROP_FRAME_HEIGHT, VIDEO_FRAME_DEFAULT_HEIGHT))
				{
					strMessage = "IR摄像头初始化失败!";
					break;
				}
			}
			nResult = 0;
		}
		else
		{
			if (g_RGB_Camera.isOpened())
			{
				nResult = 0;
				break;
			}

			if (!g_RGB_Camera.open(g_nRGB_CameraID))
			{
				strMessage = "打开摄像机失败!";
				break;
			}
			if (!g_RGB_Camera.set(CV_CAP_PROP_FRAME_WIDTH, VIDEO_FRAME_DEFAULT_WIDTH) ||
				!g_RGB_Camera.set(CV_CAP_PROP_FRAME_HEIGHT, VIDEO_FRAME_DEFAULT_HEIGHT))
			{
				strMessage = "RGB摄像头初始化失败!";
				break;
			}
			nResult = 0;
		}
	} while (0);
	return nResult;
}

int uc_FaceCapture::CloseCamera(QString& strError)
{
	g_RGB_Camera.release();
	g_IR_Camera.release();
	return 0;
}

void  uc_FaceCapture::OnFaceCaptureSucceed()
{
	QString strError;
	m_pFaceDetectOcx->EndLiveDectection();
	m_bDetectionStart = false;
	gInfo() << "OnFaceCaptureSucceed!";
	emit ShowMaskWidget("操作成功", "人脸识别成功,稍后请确认卡信息!", Success, Switch_NextPage);
	//emit SwitchNextPage();
}

void ThreadFaceDetect(QStackPage* p)
{
	uc_FaceCapture* pFaceCapture = (uc_FaceCapture*)p;
	cv::Mat irFrame;
	cv::Mat rgbFrame;

	QImage* pImage = nullptr;
	while (g_bFDThreadRunning)
	{
		if (g_nCameraCount == 2)
		{
			g_IR_Camera >> irFrame;
			g_RGB_Camera >> rgbFrame;
			ASF_SingleFaceInfo faceInfo = { 0 };

			IplImage rgbImage(rgbFrame);
			IplImage irImage(irFrame);

			MRESULT detectRes = g_videoFaceEngine.PreDetectFace(&rgbImage, faceInfo, true);

			if (MOK == detectRes)
			{
				cvRectangle(&rgbImage, cvPoint(faceInfo.faceRect.left, faceInfo.faceRect.top), cvPoint(faceInfo.faceRect.right, faceInfo.faceRect.bottom), cvScalar(0, 0, 255), 2);
				cvRectangle(&irImage, cvPoint(faceInfo.faceRect.left, faceInfo.faceRect.top), cvPoint(faceInfo.faceRect.right, faceInfo.faceRect.bottom), cvScalar(0, 0, 255), 2);

				g_curFaceInfo = faceInfo;
				g_dataValid = true;
			}
			else
			{
				g_dataValid = false;
			}

			ASF_SingleFaceInfo irFaceInfo = { 0 };
			MRESULT irRes = g_videoFaceEngine.PreDetectFace(&irImage, irFaceInfo, false);
			if (irRes == MOK)
			{
				if (abs(faceInfo.faceRect.left - irFaceInfo.faceRect.left) < 20 &&
					abs(faceInfo.faceRect.top - irFaceInfo.faceRect.top) < 20 &&
					abs(faceInfo.faceRect.right - irFaceInfo.faceRect.right) < 20 &&
					abs(faceInfo.faceRect.bottom - irFaceInfo.faceRect.bottom) < 20)
				{
					g_irDataValid = true;
					cvRectangle(&rgbImage, cvPoint(faceInfo.faceRect.left, faceInfo.faceRect.top), cvPoint(faceInfo.faceRect.right, faceInfo.faceRect.bottom), cvScalar(0, 0, 255), 2);
				}
				else
				{
					g_irDataValid = false;
				}
			}
			else
			{
				g_irDataValid = false;
			}
			lock_guard<mutex> lock(g_mutexVideo);
			if (g_curVideoImage)
				cvReleaseImage(&g_curVideoImage);

			g_curVideoImage = cvCloneImage(&rgbImage);
			if (g_curIrVideoImage)
				cvReleaseImage(&g_curIrVideoImage);
			g_curIrVideoImage = cvCloneImage(&irImage);
			if (!pImage)
				pImage = new QImage(rgbImage.width, rgbImage.height, QImage::Format_RGB32);

			//IplImageToQImage(&rgbImage, pImage);
			mutexListImage.lock();
			ListImage.push_back(cvCloneImage(&rgbImage));
			mutexListImage.unlock();
			pFaceCapture->emit UpdateVideo();
		}
		else if (g_nCameraCount == 1)
		{
			g_RGB_Camera >> rgbFrame;

			ASF_SingleFaceInfo faceInfo = { 0 };

			IplImage rgbImage(rgbFrame);

			MRESULT detectRes = g_videoFaceEngine.PreDetectFace(&rgbImage, faceInfo, true);
			if (MOK == detectRes)
			{
				cvRectangle(&rgbImage, cvPoint(faceInfo.faceRect.left, faceInfo.faceRect.top), cvPoint(faceInfo.faceRect.right, faceInfo.faceRect.bottom), cvScalar(0, 0, 255), 2);
			}
			/*lock_guard<mutex> lock(mutexVideo);
			cvReleaseImage(&m_curVideoImage);
			m_curVideoImage = cvCloneImage(&rgbImage);*/

			mutexListImage.lock();
			ListImage.push_back(cvCloneImage(&rgbImage));
			mutexListImage.unlock();
			pFaceCapture->emit UpdateVideo();
		}

		this_thread::sleep_for(std::chrono::milliseconds(40));
	}
	if (g_pQImage)
	{
		delete g_pQImage;
		g_pQImage = nullptr;
	}

	g_RGB_Camera.release();
	g_IR_Camera.release();
}

void IplImageToQImage(IplImage* srcImage, QImage* desImage)
{
	for (int i = 0; i < srcImage->height; i++)
	{
		for (int j = 0; j < srcImage->width; j++)
		{
			int r, g, b;
			if (RGB_TYPE == srcImage->nChannels)
			{
				b = (int)CV_IMAGE_ELEM(srcImage, uchar, i, j * 3 + 0);
				g = (int)CV_IMAGE_ELEM(srcImage, uchar, i, j * 3 + 1);
				r = (int)CV_IMAGE_ELEM(srcImage, uchar, i, j * 3 + 2);
			}
			else if (GRAY_TYPE == srcImage->nChannels)
			{
				b = (int)CV_IMAGE_ELEM(srcImage, uchar, i, j);
				g = b;
				r = b;
			}
			desImage->setPixel(j, i, qRgb(r, g, b));
		}
	}
}

void uc_FaceCapture::ThreadFaceFeature()
{
	//设置活体检测阈值，sdk内部默认RGB:0.5 IR:0.7,可选择是否调用该接口
	g_imageFaceEngine.SetLivenessThreshold(nRGBLiveThreshold, nIR_LiveThreshold);

	//初始化特征
	ASF_FaceFeature faceFeature = { 0 };
	faceFeature.featureSize = FACE_FEATURE_SIZE;
	faceFeature.feature = (MByte*)malloc(faceFeature.featureSize * sizeof(MByte));

	ASF_MultiFaceInfo multiFaceInfo = { 0 };
	multiFaceInfo.faceOrient = (MInt32*)malloc(sizeof(MInt32));
	multiFaceInfo.faceRect = (MRECT*)malloc(sizeof(MRECT));

	IplImage* tempImage = NULL;
	while (g_bFDThreadRunning)
	{
		if (!g_dataValid)
		{
			this_thread::sleep_for(std::chrono::milliseconds(20));
			continue;
		}

		multiFaceInfo.faceNum = 1;
		multiFaceInfo.faceOrient[0] = g_curFaceInfo.faceOrient;
		multiFaceInfo.faceRect[0] = g_curFaceInfo.faceRect;

		ASF_AgeInfo ageInfo = { 0 };
		ASF_GenderInfo genderInfo = { 0 };
		ASF_Face3DAngle angleInfo = { 0 };
		ASF_LivenessInfo liveNessInfo = { 0 };

		//IR活体检测
		bool isIRAlive = false;
		if (g_nCameraCount == 2)
		{
			IplImage* tempIRImage = NULL;
			lock_guard<mutex> lock(g_mutexVideo);
			g_mutexVideo.lock();
			if (g_curIrVideoImage)
			{
				tempIRImage = cvCloneImage(g_curIrVideoImage);
			}
			g_mutexVideo.unlock();

			if (g_irDataValid)
			{
				ASF_LivenessInfo irLiveNessInfo = { 0 };
				MRESULT irRes = g_imageFaceEngine.FaceASFProcess_IR(multiFaceInfo, tempIRImage, irLiveNessInfo);
				if (irRes == 0 && irLiveNessInfo.num > 0)
				{
					if (irLiveNessInfo.isLive[0] == 1)
					{
						m_curIRVideoShowString = "IR活体";
						isIRAlive = true;
					}
					else if (irLiveNessInfo.isLive[0] == 0)
					{
						m_curIRVideoShowString = "IR假体";
					}
					else
					{
						//-1：不确定；-2:传入人脸数>1； -3: 人脸过小；-4: 角度过大；-5: 人脸超出边界 
						m_curIRVideoShowString = "unknown";
					}
				}
				else
				{
					m_curIRVideoShowString = "";
				}
			}
			else
			{
				m_curIRVideoShowString = "";
			}

			cvReleaseImage(&tempIRImage);
		}
		else if (g_nCameraCount == 1)
		{
			isIRAlive = true;
		}
		else
		{
			break;
		}

		//RGB属性检测
		MRESULT detectRes = g_imageFaceEngine.FaceASFProcess(multiFaceInfo, tempImage, ageInfo, genderInfo, angleInfo, liveNessInfo);

		bool isRGBAlive = false;
		if (detectRes == 0 && liveNessInfo.num > 0)
		{
			if (liveNessInfo.isLive[0] == 1)
			{
				isRGBAlive = true;
			}
			else if (liveNessInfo.isLive[0] == 0)
			{
				m_curVideoShowString = "RGB假体";
			}
			else
			{
				//-1：不确定；-2:传入人脸数>1； -3: 人脸过小；-4: 角度过大；-5: 人脸超出边界 
				m_curVideoShowString = "unknown";
			}
		}
		else
		{
			m_curVideoShowString = "";
		}

		if (!(isRGBAlive && isIRAlive))
		{
			if (isRGBAlive && !isIRAlive)
			{
				m_curVideoShowString = "RGB活体";
			}
			cvReleaseImage(&tempImage);
			continue;
		}

		//特征提取
		detectRes = g_videoFaceEngine.PreExtractFeature(tempImage, faceFeature, g_curFaceInfo);

		cvReleaseImage(&tempImage);

		if (MOK != detectRes)
		{
			continue;
		}

		int maxIndex = 0;
		MFloat maxThreshold = 0.0;
		int curIndex = 0;

		for (auto regisFeature : g_vceFacefeatures)
		{
			curIndex++;
			MFloat confidenceLevel = 0;
			MRESULT pairRes = g_videoFaceEngine.FacePairMatching(confidenceLevel, faceFeature, regisFeature);

			if (MOK == pairRes && confidenceLevel > maxThreshold)
			{
				maxThreshold = confidenceLevel;
				maxIndex = curIndex;
			}
		}

		if (atof(m_strEditThreshold.c_str()) >= 0 &&
			maxThreshold >= atof(m_strEditThreshold.c_str()) &&
			isRGBAlive && isIRAlive)
		{
			QString resStr = QString("%1号:%2").arg(maxIndex).arg(maxThreshold);
			qDebug() << resStr;

			m_curVideoShowString = resStr.toStdString() + ",RGB活体";
		}
		else if (isRGBAlive)
		{
			m_curVideoShowString = "RGB活体";
		}
	}

	SafeFree(multiFaceInfo.faceOrient);
	SafeFree(multiFaceInfo.faceRect);
	SafeFree(faceFeature.feature);
}

int uc_FaceCapture::RegisterFace(string strFaceFile)
{
	IplImage* originImage = cvLoadImage(strFaceFile.c_str());

	if (!originImage)
	{
		cvReleaseImage(&originImage);
		return -1;
	}

	//FD 
	ASF_SingleFaceInfo faceInfo = { 0 };
	MRESULT detectRes = g_imageFaceEngine.PreDetectFace(originImage, faceInfo, true);
	if (MOK != detectRes)
	{
		cvReleaseImage(&originImage);
		return -1;
	}

	//FR
	ASF_FaceFeature faceFeature = { 0 };
	faceFeature.featureSize = FACE_FEATURE_SIZE;
	faceFeature.feature = (MByte*)malloc(faceFeature.featureSize * sizeof(MByte));
	detectRes = g_imageFaceEngine.PreExtractFeature(originImage, faceFeature, faceInfo);

	if (MOK != detectRes)
	{
		free(faceFeature.feature);
		cvReleaseImage(&originImage);
		return -1;
	}
	g_vceFacefeatures.push_back(faceFeature);
	return 0;
}

void uc_FaceCapture::OnFaceCaptureFailed()
{
	QString strError;
	m_pFaceDetectOcx->EndLiveDectection();
	m_bDetectionStart = false;
	gInfo() << "OnFaceCaptureFailed!";
	emit ShowMaskWidget("操作失败", "身份证照片与当前人脸对比相似度太低,匹配失败!", Failed, Return_MainPage);
}

int  uc_FaceCapture::SaveImage(QString& strFaceImageFile, QString& strMessage, int nFull)
{
	if (QFailed(GetFaceCaptureStorePath(strFaceImageFile, nFull)))
	{
		gError() << QString("无法访问人脸数据目录!").toLocal8Bit().data();
		return -1;
	}
	strMessage = QString("准备生成临时文件:%1").arg(strFaceImageFile);
	gInfo() << gQStr(strMessage);
	//LONG iDataClass：0 -- 全景数据 1 -- 人脸数据
	QString strPhotoBase64 = m_pFaceDetectOcx->GetImageData(nFull);
	if (!strPhotoBase64.size())
	{
		gError() << QString("获取人脸数据失败!").toLocal8Bit().data();
		return -1;
	}

	QImage ImageFace = QImage::fromData(QByteArray::fromBase64(strPhotoBase64.toLatin1()));
	ImageFace.save(strFaceImageFile);
	return 0;
}

int  uc_FaceCapture::GetFaceCaptureStorePath(QString& strFilePath, int nFull)
{
	QString strStorePath = QCoreApplication::applicationDirPath();
	strStorePath += "/FaceCapture/";
	strStorePath += QDateTime::currentDateTime().toString("yyyyMMdd/");
	QFileInfo fi(strStorePath);
	if (!fi.isDir())
	{// 当天目录不存在？则创建目录
		QDir storeDir;
		if (!storeDir.mkpath(strStorePath))
		{
			char szError[1024] = { 0 };
			_strerror_s(szError, 1024, nullptr);
			QString Info = QString("创建照片保存目录'%1'失败:%2").arg(strStorePath, szError);
			gInfo() << Info.toLocal8Bit().data();
			return -1;
		}
	}
	if (nFull == 0)
		strFilePath = strStorePath + QString("Full_%1.bmp").arg((const char*)g_pDataCenter->GetIDCardInfo()->szIdentify);
	else
		strFilePath = strStorePath + QString("Face_%1.bmp").arg((const char*)g_pDataCenter->GetIDCardInfo()->szIdentify);
	return 0;
}

void uc_FaceCapture::OnLiveDetectStatusEvent(int eventID, int nFrameStatus)
{
	QString strInfo = QString("nEventID = %1\tnFrameStatus = %2.\n").arg(eventID, nFrameStatus);
	gInfo() << strInfo.toLocal8Bit().data();
	QString strEvent = "";

	if (eventID == 1)
	{
		try
		{
			strEvent = "人脸检测成功!";
			gInfo() << gQStr(strEvent);
			//if (m_bFaceDetectSucceed)
			//{
			//	QString strInfo = "人脸检测成功,并且切换信号已经发送!";
			//	gInfo() << gQStr(strInfo);
			//	return;
			//}
			QString strFullImageFile;
			if (QFailed(SaveImage(strFullImageFile, strEvent, 0)))
			{
				gError() << gQStr(strEvent);
				return;
			}
			QString strFaceImageFile;
			if (QFailed(SaveImage(strFaceImageFile, strEvent, 1)))
			{
				gError() << gQStr(strEvent);
				return;
			}

			QFileInfo fi(QString::fromLocal8Bit(g_pDataCenter->strIDImageFile.c_str()));
			if (!fi.isFile())
			{
				strEvent = QString("加载身份证照片'%1'失败!").arg(g_pDataCenter->strIDImageFile.c_str());
				gInfo() << gQStr(strEvent);
				return;
			}

			strEvent = QString("尝试对身份证照片与识别人脸匹配!");
			gInfo() << gQStr(strEvent);

			double dfSimilarity = m_pFaceDetectOcx->FaceCompareByImage(QString::fromLocal8Bit(g_pDataCenter->strIDImageFile.c_str()), strFaceImageFile);
			if (dfSimilarity >= g_pDataCenter->GetSysConfigure()->dfFaceSimilarity)
			{
				gInfo() << QString("人脸匹配成功!").arg(g_pDataCenter->strIDImageFile.c_str()).arg(strFaceImageFile).arg(dfSimilarity).toLocal8Bit().data();
				if (!m_bFaceDetectSucceed)
				{
					gInfo() << QString("切换到下一页面!").toStdString().c_str();
					emit FaceCaptureSucceed();
					m_bFaceDetectSucceed = true;        // 防止多次发送消息导致多次页面切换
				}
			}
			else
			{
				emit FaceCaptureFailed();
				strEvent = QString("身份证照片与识别人脸对比相似度:%1,未达到设置最低匹配要求").arg(dfSimilarity);/*.arg(g_pDataCenter->GetSysConfigure()->dfFaceSimilarity);*/
				gInfo() << gQStr(strEvent);
			}
		}
		catch (std::exception& e)
		{
			QString strException = e.what();
			gError() << gQStr(strException);
		}
	}
	else if (eventID == 0)
	{
		switch (nFrameStatus)
		{
		case 1:
			strEvent = "人脸检测正在进行:没有检测到人脸";
			break;
		case 2:
			strEvent = "人脸检测正在进行:人脸超出蒙层";
			break;
		case 3:
			strEvent = "人脸检测正在进行:人脸角度不对,请正视摄像头!";
			break;
		case 4:
			strEvent = "人脸检测正在进行:人脸太大";
			break;
		case 5:
			strEvent = "人脸检测正在进行:人脸太小";
			break;
		case 6:
			strEvent = "人脸检测正在进行:人脸靠近边界";
			break;
		case 7:
			strEvent = "人脸检测正在进行:额头被遮挡";
			break;
		case 8:
			strEvent = "人脸检测正在进行:眼睛被遮挡";
			break;
		case 9:
			strEvent = "人脸检测正在进行:嘴巴被遮挡";
			break;
		case 10:
			strEvent = "人脸检测正在进行:人脸质量不合格";
			break;
		case 11:
			strEvent = "人脸检测正在进行:非真实人物";
			break;
		default:
			break;
		}
	}
	else if (eventID == -1) //人脸检测失败，超时
	{
		strEvent = "检测超时";

	}
	else if (eventID == -2)//人脸检测取消
	{
		strEvent = "人脸检测取消";
	}
	ui->label_Text->setText(strEvent);
}


void uc_FaceCapture::on_PopMessage(QString strMessage, QMessageBox::Icon nIcon)
{
	QMessageBox(nIcon, "Information", strMessage);
}

void uc_FaceCapture::on_UpdateVideo()
{
	list<IplImage*> TempList;
	do
	{
		mutexListImage.lock();
		TempList.swap(ListImage);
		mutexListImage.unlock();
		if (this->isHidden())
		{
			break;
		}

		if (!TempList.size())
			return;
		IplImage* pImage = TempList.front();
		if (!g_pQImage)
			g_pQImage = new QImage(pImage->width, pImage->height, QImage::Format_RGB32);

		IplImageToQImage(pImage, g_pQImage);
		QPixmap pxmp = QPixmap::fromImage(*g_pQImage);
		ui->label_FaceDetect->setPixmap(pxmp.scaled(ui->label_FaceDetect->size(), Qt::KeepAspectRatio));
	} while (0);
	for (auto var = TempList.begin(); var != TempList.end();)
	{
		cvReleaseImage(&(*var));
		var = TempList.erase(var);
	}
}
