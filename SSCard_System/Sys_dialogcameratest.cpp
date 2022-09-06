#pragma execution_character_set("utf-8")
#include "Sys_dialogcameratest.h"
#include "ui_Sys_dialogcameratest.h"
#include "Gloabal.h"
#include <QDir>
#include <QMessageBox>
#include <QTimer>
#include <QProcess>
#include "payment.h"
//#include <conio.h>
//#include <direct.h>

DialogCameraTest::DialogCameraTest(bool bShowGuardian, QWidget* parent) :
	QDialog(parent),
	ui(new Ui::DialogCameraTest)
{
	ui->setupUi(this);
	/*QIcon icon;
	icon.addFile("url(./Image/Camera.png)", QSize(), QIcon::Normal, QIcon::On);
	ui->pushButton_TakePhoto->setIcon(icon);
	ui->pushButton_TakePhoto->setIconSize(QSize(48, 48));*/
	QString strQSS = QString("border-image: url(./Image/FaceCaptureSample.jpg);");
	ui->label_FaceCapture->setStyleSheet(strQSS);
	pButtonGrp = new QButtonGroup(this);
	pButtonGrp->addButton(ui->radioButton_Oneself, 0);
	pButtonGrp->addButton(ui->radioButton_Guardian, 1);
	pButtonGrp->setExclusive(true);
	ui->radioButton_Oneself->setChecked(true);

	if (!bShowGuardian)
	{
		ui->radioButton_Oneself->hide();
		ui->radioButton_Guardian->hide();
	}

	QTimer::singleShot(50, this, [=]()
		{
			QString strError;
			g_pDataCenter->OpenCamera();
			OpenCamara(strError);
		});
}

DialogCameraTest::~DialogCameraTest()
{
	QString strError;
	CloseCamera(strError);
	delete ui;
}

int DialogCameraTest::OpenCamara(QString& strError)
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
		// 		if (!g_pDataCenter->StartDetect(this, 2000, m_nTimeout * 1000))
		// 		{
		// 			strError = "启动人脸检测失败!";
		// 			return -1;
		// 		}
				//connect(this, SIGNAL(LiveDetectStatusEvent(int, int)), this, SLOT(OnLiveDetectStatusEvent(int, int)));
				//m_bDetectionStart = true;
	} while (0);
	return nResult;
}

int DialogCameraTest::CloseCamera(QString& strError)
{
	int nResult = 0;
	do
	{
		g_pDataCenter->StopVideo();
		g_pDataCenter->StopFaceDetect();

	} while (0);
	return nResult;
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

#include <process.h>
#include <windows.h>
unsigned long CreateFaceProcess(const char* szPath, const char* szCurrentDir)
{
	STARTUPINFOA si;
	PROCESS_INFORMATION pi;

	ZeroMemory(&si, sizeof(si));
	si.cb = sizeof(si);
	ZeroMemory(&pi, sizeof(pi));
	// Start the child process. 
	if (!CreateProcessA(NULL,   // No module name (use command line)
		(LPSTR)szPath,        // Command line
		NULL,           // Process handle not inheritable
		NULL,           // Thread handle not inheritable
		FALSE,          // Set handle inheritance to FALSE
		0,              // No creation flags
		NULL,           // Use parent's environment block
		szCurrentDir,           // Use parent's starting directory 
		&si,            // Pointer to STARTUPINFO structure
		&pi)           // Pointer to PROCESS_INFORMATION structure
		)
	{
		return GetLastError();
	}

	// Wait until child process exits.
	WaitForSingleObject(pi.hProcess, INFINITE);
	// Close process and thread handles. 
	CloseHandle(pi.hProcess);
	CloseHandle(pi.hThread);
	return 0L;
}

void DialogCameraTest::on_pushButton_TakePhoto_clicked()
{
	QWaitCursor Wait;
	QString strMessage;
	QString strAppPath = QApplication::applicationDirPath();
	QString strPhotoPath1 = strAppPath + "/PhotoProcess/1.jpg";
	try
	{
		if (fs::exists(strPhotoPath1.toStdString()))
			fs::remove(strPhotoPath1.toStdString());

		if (!g_pDataCenter->SaveFaceImage(strPhotoPath1.toStdString(), true))
		{
			QMessageBox(QMessageBox::Critical, "提示", "无法成图,可能摄像机故障!", QMessageBox::Ok, this);
			return;
		}
		ui->label_FaceCapture->setStyleSheet("border-image: url();");
		if (QFailed(ProcessHeaderImage(strHeaderPhoto, strMessage)))
		{
			QMessageBox(QMessageBox::Critical, "提示", strMessage, QMessageBox::Ok, this);
			return;
		}
		QString strDestPhoto = strAppPath + "/PhotoProcess/Oneself.jpg";
		if (pButtonGrp->checkedId() == 1)
			strDestPhoto = strAppPath + "/PhotoProcess/Guardian.jpg";
		fs::rename(strHeaderPhoto.toStdString(), strDestPhoto.toStdString());

		QString strQSS = QString("border-image: url(%1);").arg(strDestPhoto);
		ui->label_FaceCapture->setStyleSheet(strQSS);
	}
	catch (std::exception& e)
	{
		gInfo() << "发生异常" << e.what();
	}
}

void DialogCameraTest::on_pushButton_OK_clicked()
{
	QWaitCursor Wait;
	QString strAppPath = QApplication::applicationDirPath();
	QString strPhotoPath1 = strAppPath + "/PhotoProcess/1.jpg";

	QString strPhotoOneSelf = strAppPath + "/PhotoProcess/Oneself.jpg";
	QString strPhotoGuardian = strAppPath + "/PhotoProcess/Guardian.jpg";

	try
	{
		if (fs::exists(strPhotoPath1.toStdString()))
			fs::remove(strPhotoPath1.toStdString());

		if (fs::exists(strPhotoOneSelf.toStdString()))
		{
			string strSSCardPhoto;
			if (QSucceed(GetImageStorePath(strSSCardPhoto, PhotoType::Photo_SSCard)))
			{
				if (fs::exists(strSSCardPhoto))
					fs::remove(strSSCardPhoto);
				fs::copy(strPhotoOneSelf.toStdString(), strSSCardPhoto);
				g_pDataCenter->strSSCardPhotoFile = strSSCardPhoto;
			}
			fs::remove(strPhotoOneSelf.toStdString());
		}
		if (pButtonGrp->checkedId() == 1 && fs::exists(strPhotoGuardian.toStdString()))
		{
			string strGuaidianPhoto;
			if (QSucceed(GetImageStorePath(strGuaidianPhoto, PhotoType::Photo_Guardian)))
			{
				if (fs::exists(strGuaidianPhoto))
					fs::remove(strGuaidianPhoto);
				fs::copy(strPhotoGuardian.toStdString(), strGuaidianPhoto);
				g_pDataCenter->strGuardianPhotoFile = strGuaidianPhoto;
			}
			fs::remove(strPhotoGuardian.toStdString());
		}
	}
	catch (std::exception& e)
	{
		gInfo() << "发生异常" << e.what();
	}
	QDialog::accept();
}
