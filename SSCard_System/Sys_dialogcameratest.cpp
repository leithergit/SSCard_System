#pragma execution_character_set("utf-8")
#include "Sys_dialogcameratest.h"
#include "ui_Sys_dialogcameratest.h"
#include "Gloabal.h"
#include <QDir>
#include <QMessageBox>
#include <QTimer>

DialogCameraTest::DialogCameraTest(QWidget* parent) :
	QDialog(parent),
	ui(new Ui::DialogCameraTest)
{
	ui->setupUi(this);
	QString strQSS = QString("border-image: url(./Image/FaceCaptureSample.jpg);");
	ui->label_FaceCapture->setStyleSheet(strQSS);
	QTimer::singleShot(50, this, [=]()
		{
			OpenCamera();
		});
}

DialogCameraTest::~DialogCameraTest()
{
	CloseCamera();
	delete ui;
}

void DialogCameraTest::on_pushButton_clicked()
{
	QDialog::accept();
}

void DialogCameraTest::OpenCamera()
{

}

void DialogCameraTest::CloseCamera()
{

}

void DialogCameraTest::OnLiveDetectStatusEvent(int eventID, int nFrameStatus)
{
	qDebug("nEventID = %d\tnFrameStatus = %d.\n", eventID, nFrameStatus);
	QString strEvent = "";
	if (eventID == 1)
	{
		strEvent = "人脸检测成功";
		ui->label_Info->setStyleSheet(QString::fromUtf8("color: rgb(0, 0, 0);"));
		//LONG iDataClass：0 -- 全景数据 1 -- 人脸数据
		QString strPhotoBase64;// m_pFaceDetectOcx->GetImageData(1);
		if (strPhotoBase64.size())
		{
			ImageDetected = QImage::fromData(QByteArray::fromBase64(strPhotoBase64.toLatin1()));
			QString strCurrentPath = QDir::currentPath();
			strCurrentPath += "/PhotoDetect.bmp";
			ImageDetected.save(strCurrentPath);

			QString strQSS = QString("border-image: url(%1);").arg(strCurrentPath);
			ui->label_FaceCapture->setStyleSheet(strQSS);
		}
	}
	else if (eventID == 0)
	{
		ui->label_Info->setStyleSheet(QString::fromUtf8("color: rgb(255, 0, 0);"));
		switch (nFrameStatus)
		{
		case 1:
			strEvent = "人脸检测正在进行:没有检测到人脸";
			break;
		case 2:
			strEvent = "人脸检测正在进行:人脸超出蒙层";
			break;
		case 3:
			strEvent = "人脸检测正在进行:人脸角度不对,请正视摄像";
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
	ui->label_Info->setText(strEvent);
}
