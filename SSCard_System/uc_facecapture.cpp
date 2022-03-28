#pragma execution_character_set("utf-8")

#include "uc_facecapture.h"
#include "ui_uc_facecapture.h"
#include <QCoreApplication>
#include <QObject>
#include "Gloabal.h"

//#include "mainwindow.h"

uc_FaceCapture::uc_FaceCapture(QLabel* pTitle, QString strStepImage, Page_Index nIndex, QWidget* parent) :
	QStackPage(pTitle, strStepImage, nIndex, parent),
	ui(new Ui::FaceCapture)
{
	ui->setupUi(this);
	connect(this, &uc_FaceCapture::FaceCaptureSucceed, this, &uc_FaceCapture::OnFaceCaptureSucceed);
	connect(this, &uc_FaceCapture::FaceCaptureFailed, this, &uc_FaceCapture::OnFaceCaptureFailed);
}

uc_FaceCapture::~uc_FaceCapture()
{
	ShutDownDevice();
	delete ui;
}

void  uc_FaceCapture::ShutDownDevice()
{
	QString strError;
	if (CloseCamera(strError))
	{
		gError() << strError.toLocal8Bit().data();
	}

}
void uc_FaceCapture::ShutDown()
{
	gInfo() << __FUNCTION__;

	g_pDataCenter->StopFaceDetect();

}

int uc_FaceCapture::ProcessBussiness()
{
	m_bFaceDetectSucceed = false;
	QString strError;

	if (QFailed(OpenCamara(strError)))
	{
		gError() << strError.toLocal8Bit().data();
		///QMessageBox::critical(this,tr("打开摄像机失败"),strError,QMessageBox::Ok);
		emit ShowMaskWidget("操作失败", strError, Fetal, Return_MainPage);
		return -1;
	}
	return 0;
}

void uc_FaceCapture::OnTimeout()
{
	ShutDown();
}

int uc_FaceCapture::OpenCamara(QString& strError)
{
	int nResult = 0;
	do
	{
		g_pDataCenter->StopFaceDetect();
		if (g_pDataCenter->IsVideoStart())
		{
			if (!g_pDataCenter->SwitchVideoWnd((HWND)ui->label_FaceDetect->winId()))
			{
				strError = "切换视频显示窗口失败!";
				nResult = -1;
				break;
			}
		}
		else
		{
			if (!g_pDataCenter->StartVideo((HWND)ui->label_FaceDetect->winId()))
			{
				strError = "获取视频数据失败!";
				return -1;
			}

		}
		if (!g_pDataCenter->StartFaceDetect(this, 2000, m_nTimeout * 1000))
		{
			strError = "启动人脸检测失败!";
			return -1;
		}
		connect(this, SIGNAL(LiveDetectStatusEvent(int, int)), this, SLOT(OnLiveDetectStatusEvent(int, int)));
		m_bDetectionStart = true;
	} while (0);
	return nResult;
}

int uc_FaceCapture::CloseCamera(QString& strError)
{
	int nResult = 0;
	do
	{
		g_pDataCenter->StopVideo();
		g_pDataCenter->StopFaceDetect();

	} while (0);
	return nResult;
}

void  uc_FaceCapture::OnFaceCaptureSucceed()
{
	QString strError;
	g_pDataCenter->StopFaceDetect();

	gInfo() << "OnFaceCaptureSucceed!";
	emit ShowMaskWidget("操作成功", "人脸识别成功,稍后请确认卡信息!", Success, Switch_NextPage);
}

void uc_FaceCapture::OnFaceCaptureFailed()
{
	QString strError;
	g_pDataCenter->StopFaceDetect();
	gInfo() << "OnFaceCaptureFailed!";
	emit ShowMaskWidget("操作失败", "身份证照片与当前人脸对比相似度太低,匹配失败!", Failed, Return_MainPage);
}

int  uc_FaceCapture::SaveImage(QString& strFaceImageFile, QString& strMessage, bool bFull)
{
	if (QFailed(GetFaceCaptureStorePath(strFaceImageFile, bFull)))
	{
		gError() << QString("无法访问人脸数据目录!").toLocal8Bit().data();
		return -1;
	}
	strMessage = QString("准备生成临时文件:%1").arg(strFaceImageFile);
	gInfo() << gQStr(strMessage);
	g_pDataCenter->SaveFaceImage(strFaceImageFile.toStdString(), bFull);

	return 0;
}

int  uc_FaceCapture::GetFaceCaptureStorePath(QString& strFilePath, bool bFull)
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
	if (bFull)
		strFilePath = strStorePath + QString("Full_%1.bmp").arg((const char*)g_pDataCenter->GetIDCardInfo()->szIdentity);
	else
		strFilePath = strStorePath + QString("Face_%1.bmp").arg((const char*)g_pDataCenter->GetIDCardInfo()->szIdentity);
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

			QString strFullImageFile;
			if (QFailed(SaveImage(strFullImageFile, strEvent, true)))
			{
				gError() << gQStr(strEvent);
				return;
			}
			QString strFaceImageFile;
			if (QFailed(SaveImage(strFaceImageFile, strEvent, false)))
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

			float dfSimilarity = 0.0f;
			if (g_pDataCenter->FaceCompareByImage(QString::fromLocal8Bit(g_pDataCenter->strIDImageFile.c_str()).toStdString(), strFaceImageFile.toStdString(), dfSimilarity))
			{
				if (dfSimilarity >= g_pDataCenter->GetSysConfigure()->dfFaceSimilarity)
				{
					strEvent = "人脸匹配成功!";
					gInfo() << gQStr(strEvent);
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
			else
			{
				strEvent = QString("人脸对比失败!");
				gInfo() << gQStr(strEvent);
				emit FaceCaptureFailed();
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
