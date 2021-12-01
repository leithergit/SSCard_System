#include "FaceDetect.h"
#include <QMessageBox>
#include <QDebug>
#include <QFile>
#include <QImage>
#include <QPixmap>
#include <QFileDialog>
#pragma execution_character_set("utf-8")
FaceDetect::FaceDetect(QWidget *parent)
    : QDialog(parent)
{
    ui.setupUi(this);  
}

int  FaceDetect::GetErrorMessage(int nErrorCode, QString strMessage)
{
    switch (nErrorCode)
    {
    default:
        break;
    }
    return 0;
}

void FaceDetect::OnLiveDetectStatusEvent(int eventID, int nFrameStatus)
{
    qDebug("nEventID = %d\tnFrameStatus = %d.\n", eventID, nFrameStatus);
	QString strEvent = "";
	if (eventID == 1)
	{
		strEvent = "活体检测成功";
		ui.label_Info->setStyleSheet(QString::fromUtf8("color: rgb(0, 0, 0);"));
		//LONG iDataClass：0 -- 全景数据 1 -- 人脸数据
		QString strPhotoBase64 = m_pFaceDetectOcx->GetImageData(1);
		if (strPhotoBase64.size())
		{
			ImageDetected = QImage::fromData(QByteArray::fromBase64(strPhotoBase64.toLatin1()));
			QString strCurrentPath = QDir::currentPath();
			strCurrentPath += "/PhotoDetect.bmp";
			ImageDetected.save(strCurrentPath);
			ui.label_FaceCapture->setPixmap(QPixmap::fromImage(ImageDetected));
		}
		ui.pushButton_DetectStop->setEnabled(false);
		ui.pushButton_DetectStart->setEnabled(true);
	}
	else if (eventID == 0)
	{
		ui.label_Info->setStyleSheet(QString::fromUtf8("color: rgb(255, 0, 0);"));
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
		ui.pushButton_DetectStop->setEnabled(false);
		ui.pushButton_DetectStart->setEnabled(true);
	}
	else if (eventID == -2)//活体检测取消
	{
		strEvent = "活体检测取消";
		ui.pushButton_DetectStop->setEnabled(false);
		ui.pushButton_DetectStart->setEnabled(true);
	}
	ui.label_Info->setText(strEvent);
}
void FaceDetect::on_pushButton_CameraOpen_clicked()
{
    if (!m_pFaceDetectOcx)
    {
		m_pFaceDetectOcx = new DVTLDCamOCXLib::DVTLDCamOCX(ui.label_FaceDetect);
		m_pFaceDetectOcx->setObjectName(QString::fromUtf8("axWidget_FaceDetect"));
		m_pFaceDetectOcx->setMinimumSize(ui.label_FaceDetect->size());
		m_pFaceDetectOcx->setMaximumSize(ui.label_FaceDetect->size());
        m_pFaceDetectOcx->show();
        QObject::connect(m_pFaceDetectOcx, SIGNAL(LiveDetectStatusEvent(int, int)), this, SLOT(OnLiveDetectStatusEvent(int, int)));

//         QString strDoc = m_pFaceDetectOcx->generateDocumentation();//导出所有ocx控件的所有信号、函数、属性等，可供开发参考
// 		   QString strDetectPhoto = QDir::currentPath() +  "/axHelp.html";
//         QFile file("strDetectPhoto);       
//         file.open(QIODevice::WriteOnly);
//         file.write(strDoc.toLatin1(),strDoc.size());
//         file.close();
        int nResult = m_pFaceDetectOcx->OpenCamera();
        if (nResult)
        {
            QMessageBox::information(nullptr, "提示", "打开摄像机失败:" + QString("%1").arg(nResult));
            return;
        }
        ui.pushButton_CameraOpen->setEnabled(false);
        ui.pushButton_DetectStart->setEnabled(true);
        ui.pushButton_CameraClose->setEnabled(true);
        ui.pushButton_PhotoCapture->setEnabled(true);
    }
}

void FaceDetect::on_pushButton_DetectStart_clicked()
{
    long nTimeout = 15;
    m_pFaceDetectOcx->dynamicCall("StartLiveDetection(LONG nTimeout)", nTimeout);
    if (m_pFaceDetectOcx)
    {
        m_pFaceDetectOcx->StartLiveDetection(15);
		ui.pushButton_DetectStart->setEnabled(false);
		ui.pushButton_DetectStop->setEnabled(true);
    }
}

void FaceDetect::on_pushButton_DetectStop_clicked()
{
	if (m_pFaceDetectOcx)
	{
		m_pFaceDetectOcx->EndLiveDectection();
		ui.pushButton_DetectStart->setEnabled(true);
		ui.pushButton_DetectStop->setEnabled(false);
	}
}

void FaceDetect::on_pushButton_CameraClose_clicked()
{
	if (m_pFaceDetectOcx)
	{
		m_pFaceDetectOcx->CloseCamera();
		ui.pushButton_CameraOpen->setEnabled(true);
		ui.pushButton_DetectStart->setEnabled(false);
		ui.pushButton_DetectStop->setEnabled(false);
		ui.pushButton_CameraClose->setEnabled(false);
		ui.pushButton_PhotoCapture->setEnabled(false);
        delete m_pFaceDetectOcx;
        m_pFaceDetectOcx = nullptr;
 	}
}


void FaceDetect::on_pushButton_PhotoCapture_clicked()
{
	if (m_pFaceDetectOcx)
	{
        m_pFaceDetectOcx->GetImageData(1);
	}
}

void FaceDetect::on_pushButton_PhotoBrowse_clicked()
{
	QString strFilePhoto = QFileDialog::getOpenFileName(this);
	if (!strFilePhoto.isEmpty())
	{
		ui.lineEdit_IndentifyPhoto->setText(strFilePhoto);
		QImage ImageIdentify;
		ImageIdentify.load(strFilePhoto);
		ui.label_IdentifyPhoto->setPixmap(QPixmap::fromImage(ImageIdentify));
	}
}


void FaceDetect::on_pushButton_PhotoCompare_clicked()
{
	if (m_pFaceDetectOcx)
	{
		QString strDetectPhoto = QDir::currentPath() +  "/PhotoDetect.bmp";
		QString strIdentifyPhoto = ui.lineEdit_IndentifyPhoto->text();
		double dfSimilarity = m_pFaceDetectOcx->FaceCompareByImage(strDetectPhoto, strIdentifyPhoto);
		QString strInfo = QString("相似度:%1%%").arg(dfSimilarity);
		ui.label_Info->setText(strInfo);
	}
}

