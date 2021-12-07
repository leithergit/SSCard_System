#pragma execution_character_set("utf-8")

#include "uc_facecapture.h"
#include "ui_uc_facecapture.h"
#include <QCoreApplication>
#include <QObject>
#include "Gloabal.h"

//#include "mainwindow.h"

uc_FaceCapture::uc_FaceCapture(QLabel* pTitle, QString strStepImage, int nTimeout, QWidget* parent) :
	QStackPage(pTitle, strStepImage, nTimeout, parent),
	ui(new Ui::FaceCapture)
{
	ui->setupUi(this);
	connect(this, &uc_FaceCapture::FaceCaptureSucceed, this, &uc_FaceCapture::OnFaceCaptureSucceed);
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
	ShutDownDevice();
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
	ShutDownDevice();
}

int uc_FaceCapture::OpenCamara(QString& strError)
{
	int nResult = 0;
	do
	{
		if (!m_pFaceDetectOcx)
		{
			m_pFaceDetectOcx = new DVTLDCamOCXLib::DVTLDCamOCX(ui->label_FaceDetect);
			if (!m_pFaceDetectOcx)
			{
				strError = "人脸检测组件初始化失败!";
				nResult = -1;
				break;
			}
		}
		if (!m_bOuputProductInfo)
		{
			QString strProduct = "人脸检测组件产品信息:." + m_pFaceDetectOcx->GetProduct();
			QString strVersion = "人脸检测组件版本:" + m_pFaceDetectOcx->GetVersion();
			gInfo() << strProduct.toLocal8Bit().data();
			gInfo() << strVersion.toLocal8Bit().data();
			m_bOuputProductInfo = true;
		}

		m_pFaceDetectOcx->setObjectName(QString::fromUtf8("axWidget_FaceDetect"));
		m_pFaceDetectOcx->setMinimumSize(ui->label_FaceDetect->size());
		m_pFaceDetectOcx->setMaximumSize(ui->label_FaceDetect->size());
		m_pFaceDetectOcx->show();
		connect(m_pFaceDetectOcx, SIGNAL(LiveDetectStatusEvent(int, int)), this, SLOT(OnLiveDetectStatusEvent(int, int)));

		//        QString strDoc = m_pFaceDetectOcx->generateDocumentation();//导出所有ocx控件的所有信号、函数、属性等，可供开发参考
		//        QString strFaceDetectDoc = QDir::currentPath() +  "/axHelp.html";
		//        QFile file(strFaceDetectDoc);
		//        file.open(QIODevice::WriteOnly);
		//        file.write(strDoc.toLatin1(),strDoc.size());
		//        file.close();
		int nResult = m_pFaceDetectOcx->OpenCamera();
		if (nResult)
		{
			strError = QString("打开摄像机失败,错误代码:%1,请检查设备连接!").arg(nResult);
			nResult = -1;
			break;
		}

		nResult = m_pFaceDetectOcx->StartLiveDetection(m_nTimeout);
		if (nResult)
		{
			strError = QString("启动人脸检测失败,错误代码:%1,请检查设备连接!").arg(nResult);
			nResult = -1;
			break;
		}
		m_pImageFaceDetected = new QImage;
	} while (0);
	return nResult;
}

int uc_FaceCapture::CloseCamera(QString& strError)
{
	int nResult = 0;
	do
	{
		if (!m_pFaceDetectOcx)
			break;
		nResult = m_pFaceDetectOcx->EndLiveDectection();
		if (nResult)
		{
			strError = QString("停止人脸检测失败,错误代码:%1!").arg(nResult);
			break;
		}
		nResult = m_pFaceDetectOcx->CloseCamera();
		if (nResult)
		{
			strError = QString("关闭摄像机失败,错误代码:%1!").arg(nResult);
			break;
		}
		if (m_pImageFaceDetected)
		{
			delete m_pImageFaceDetected;
			m_pImageFaceDetected = nullptr;
		}
		delete m_pFaceDetectOcx;
		m_pFaceDetectOcx = nullptr;

	} while (0);
	return nResult;
}

void  uc_FaceCapture::OnFaceCaptureSucceed()
{
	QString strError;
	if (QFailed(CloseCamera(strError)))
	{
		gInfo() << strError.toLocal8Bit().data();
	}
	gInfo() << "OnFaceCaptureSucceed!";
	emit ShowMaskWidget("操作成功", "人脸识别成功,稍后请确认卡信息!", Success, Switch_NextPage);
	//emit SwitchNextPage();
}

int  uc_FaceCapture::GetFaceCaptureStorePath(QString& strFilePath)
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
		strEvent = "活体检测成功!";
		gInfo() << strEvent.toLocal8Bit().data();

		QString strFaceImageFile;
		if (QFailed(GetFaceCaptureStorePath(strFaceImageFile)))
		{
			gError() << QString("无法访问人脸数据目录!").toLocal8Bit().data();
			return;
		}

		gInfo() << QString("准备生成临时文件:%1").arg(strFaceImageFile).toLocal8Bit().data();
		//LONG iDataClass：0 -- 全景数据 1 -- 人脸数据
		QString strPhotoBase64 = m_pFaceDetectOcx->GetImageData(1);
		if (!strPhotoBase64.size())
		{
			gError() << QString("获取人脸数据失败!").toLocal8Bit().data();
			return;
		}

		*m_pImageFaceDetected = QImage::fromData(QByteArray::fromBase64(strPhotoBase64.toLatin1()));
		m_pImageFaceDetected->save(strFaceImageFile);
		QFileInfo fi(QString::fromLocal8Bit(g_pDataCenter->strIDImageFile.c_str()));
		if (!fi.isFile())
		{
			QString strTemp = QString("身份证照片'%1'丢失!").arg(g_pDataCenter->strIDImageFile.c_str());
			gError() << strTemp.toLocal8Bit().data();
			return;
		}

		double dfSimilarity = m_pFaceDetectOcx->FaceCompareByImage(QString::fromLocal8Bit(g_pDataCenter->strIDImageFile.c_str()), strFaceImageFile);
		if (dfSimilarity >= g_pDataCenter->GetSysConfigure()->dfFaceSimilarity)
		{
			gInfo() << QString("人脸匹配成功!").arg(g_pDataCenter->strIDImageFile.c_str()).arg(strFaceImageFile).arg(dfSimilarity).toLocal8Bit().data();
			if (!m_bFaceDetectSucceed)
			{
				emit FaceCaptureSucceed();
				m_bFaceDetectSucceed = true;        // 防止多次发送消息导致多次页面切换
			}
		}
	}
	else if (eventID == 0)
	{
		switch (nFrameStatus)
		{
		case 1:
			strEvent = "活体检测正在进行:没有检测到人脸";
			break;
		case 2:
			strEvent = "活体检测正在进行:人脸超出蒙层";
			break;
		case 3:
			strEvent = "活体检测正在进行:人脸角度不对、属于低头， 抬头， 左右转情况";
			break;
		case 4:
			strEvent = "活体检测正在进行:人脸太大";
			break;
		case 5:
			strEvent = "活体检测正在进行:人脸太小";
			break;
		case 6:
			strEvent = "活体检测正在进行:人脸靠近边界";
			break;
		case 7:
			strEvent = "活体检测正在进行:额头被遮挡";
			break;
		case 8:
			strEvent = "活体检测正在进行:眼睛被遮挡";
			break;
		case 9:
			strEvent = "活体检测正在进行:嘴巴被遮挡";
			break;
		case 10:
			strEvent = "活体检测正在进行:人脸质量不合格";
			break;
		case 11:
			strEvent = "活体检测正在进行:非活体";
			break;
		default:
			break;
		}
	}
	else if (eventID == -1) //活体检测失败，超时
	{
		strEvent = "检测超时";

	}
	else if (eventID == -2)//活体检测取消
	{
		strEvent = "活体检测取消";
	}
	ui->label_Text->setText(strEvent);
}
