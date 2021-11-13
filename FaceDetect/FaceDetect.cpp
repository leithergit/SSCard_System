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
		strEvent = "������ɹ�";
		ui.label_Info->setStyleSheet(QString::fromUtf8("color: rgb(0, 0, 0);"));
		//LONG iDataClass��0 -- ȫ������ 1 -- ��������
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
			strEvent = "���������ڽ���:û�м�⵽����";
			break;					   
		case 2:						   
			strEvent = "���������ڽ���:���������ɲ�";
			break;					   
		case 3:						   
			strEvent = "���������ڽ���:�����ǶȲ��ԡ����ڵ�ͷ�� ̧ͷ�� ����ת���";
			break;					   
		case 4:						   
			strEvent = "���������ڽ���:����̫��";
			break;					   
		case 5:						   
			strEvent = "���������ڽ���:����̫С";
			break;					   
		case 6:						   
			strEvent = "���������ڽ���:���������߽�";
			break;					   
		case 7:						   
			strEvent = "���������ڽ���:��ͷ���ڵ�";
			break;					   
		case 8:						   
			strEvent = "���������ڽ���:�۾����ڵ�";
			break;					   
		case 9:						   
			strEvent = "���������ڽ���:��ͱ��ڵ�";
			break;					   
		case 10:					   
			strEvent = "���������ڽ���:�����������ϸ�";
			break;					   
		case 11:					   
			strEvent = "���������ڽ���:�ǻ���";
			break;
		default:
			break;
		}
	}
	else if (eventID == -1) //������ʧ�ܣ���ʱ
	{
		strEvent = "��ⳬʱ";
		ui.pushButton_DetectStop->setEnabled(false);
		ui.pushButton_DetectStart->setEnabled(true);
	}
	else if (eventID == -2)//������ȡ��
	{
		strEvent = "������ȡ��";
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

//         QString strDoc = m_pFaceDetectOcx->generateDocumentation();//��������ocx�ؼ��������źš����������Եȣ��ɹ������ο�
// 		   QString strDetectPhoto = QDir::currentPath() +  "/axHelp.html";
//         QFile file("strDetectPhoto);       
//         file.open(QIODevice::WriteOnly);
//         file.write(strDoc.toLatin1(),strDoc.size());
//         file.close();
        int nResult = m_pFaceDetectOcx->OpenCamera();
        if (nResult)
        {
            QMessageBox::information(nullptr, "��ʾ", "�������ʧ��:" + QString("%1").arg(nResult));
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
		QString strInfo = QString("���ƶ�:%1%%").arg(dfSimilarity);
		ui.label_Info->setText(strInfo);
	}
}

