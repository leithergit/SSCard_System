#pragma execution_character_set("utf-8")
#include "register.h"
#include "ui_register.h"

#include <string>
#include <openssl/aes.h>
#include <openssl/rsa.h>
#include <openssl/pem.h>
#include <openssl/err.h>
#include <openssl/bio.h>
#include <QFileDialog>
#include <QSettings>
#include <QMessageBox>
#include <QDebug>
#include "../utility/Utility.h"

extern "C"
{
#include <openssl/applink.c>
}

using namespace std;

#pragma comment(lib,"libcrypto.lib")
#pragma comment(lib,"libssl.lib")
#pragma comment(lib,"user32.lib")
#pragma comment(lib,"advapi32.lib")
#pragma comment(lib,"OleAut32.lib")
#pragma comment(lib,"Version.lib")

const char* g_szPrivateKey = "-----BEGIN RSA PRIVATE KEY-----\n"
"MIICXAIBAAKBgQClc+3UN7YBHTUN0M2xK1inyXg5Yaxg3fJLL5DD5c8CibkuDmec\n"
"Pwb+WYQOQec3Pc502RoI4nFISxeWF04Yn0ELrPIfsW6bW/Aw6DedAD2uFoyWvvH4\n"
"PWVM6OoQUq1L7LIWFrXMewdZQJrlPAGnJoVVJEtSOgk7PIMvaSSU9swwbwIDAQAB\n"
"AoGAUpiuOQxa2vGGw8/ib7jw+EpWr2a4VF9Yjvo+7dzy3BR/EqP0S4cXabCsgSWb\n"
"VPOQ9rpPCfJplSqPqskVnqsaH/kMP83+bkKdOhYxIwZq0XSSE+wy6iWLDJ+fTlR7\n"
"7qGEwbf/+toTikezyCEZJ2RNm9NTw/A6e4AS7AYBXgGIbHkCQQDcgDK2X6vb0L70\n"
"SlNAsGBXOh4pFwYKUrkqrcumTUWNTHMOwVk9RabCnE0My1OYZMVRCm2NnblZ8UWI\n"
"Spb+Y0idAkEAwBb3CJPZ83JrnJAIKE9bUfPT465HtjpxSyqPtrGcpl8j99CfQy8T\n"
"XBr3048+AKAuapIAdYDI58Eif45x2MdxewJAL5Pl3V4lWgffvBxxfErSbt/WdC1I\n"
"m1+gHqpjf926fTOau/tSQxtNUT6ijXkt73sG7GagnucCgHrD8xWNIRTRLQJBAK4x\n"
"ZS4DHge1wD6HIs6jQDXuPj1fYyqujuTSIOdU47C/4Fcvee2r+AtzbUANyvU4TT9Z\n"
"TU9BTW9U5lr0Kuuh2ZcCQFhg02emcOOH05RD6HG5p2tP2qZ47dumjOSLIfZEnVcY\n"
"JjHm1rAH5+vLXzbaJ4jOiB9RxKyenIMBH3HGR32goLs=\n"
"-----END RSA PRIVATE KEY-----\n";

const char* g_szPublicKey = "-----BEGIN PUBLIC KEY-----\n"
"MIGfMA0GCSqGSIb3DQEBAQUAA4GNADCBiQKBgQClc+3UN7YBHTUN0M2xK1inyXg5\n"
"Yaxg3fJLL5DD5c8CibkuDmecPwb+WYQOQec3Pc502RoI4nFISxeWF04Yn0ELrPIf\n"
"sW6bW/Aw6DedAD2uFoyWvvH4PWVM6OoQUq1L7LIWFrXMewdZQJrlPAGnJoVVJEtS\n"
"Ogk7PIMvaSSU9swwbwIDAQAB\n"
"-----END PUBLIC KEY-----\n";

std::string EncryptRSAKey(const char* szKey, const std::string& strData)
{
	if (strData.empty())
	{
		assert(false);
		return "";
	}
	std::string strRet;
	BIO* bio = NULL;
	RSA* pRSAPrivateKey = NULL;
	char* szPrivateKey = const_cast<char*>(szKey);
	if ((bio = BIO_new_mem_buf(szPrivateKey, -1)) == NULL)       //从字符串读取RSA私钥
	{
		return "";
	}

	//PEM_read_bio_RSA_PUBKEY
	pRSAPrivateKey = PEM_read_bio_RSAPrivateKey(bio, NULL, NULL, NULL);		//从bio结构中得到rsa结构
	if (!pRSAPrivateKey)
	{
		ERR_load_crypto_strings();
		char errBuf[512];
		ERR_error_string_n(ERR_get_error(), errBuf, sizeof(errBuf));
		BIO_free_all(bio);
		return "";
	}

	int nLen = RSA_size(pRSAPrivateKey);
	char* pEncode = new char[nLen + 1];
	int ret = RSA_private_encrypt(strData.length(), (const unsigned char*)strData.c_str(), (unsigned char*)pEncode, pRSAPrivateKey, RSA_PKCS1_PADDING);
	if (ret >= 0)
	{
		strRet = std::string(pEncode, ret);
	}
	delete[] pEncode;
	RSA_free(pRSAPrivateKey);
	CRYPTO_cleanup_all_ex_data();
	return strRet;
}

std::string DecryptRSAKey(const char* szKey, const std::string& strData)
{
	if (strData.empty())
	{
		assert(false);
		return "";
	}
	std::string strRet;
	BIO* bio = NULL;
	RSA* pRSAPubKey = NULL;
	char* chPublicKey = const_cast<char*>(szKey);
	if ((bio = BIO_new_mem_buf(chPublicKey, -1)) == NULL)       //从字符串读取RSA公钥
	{
		return "";
	}
	pRSAPubKey = PEM_read_bio_RSA_PUBKEY(bio, NULL, NULL, NULL);		//从bio结构中得到rsa结构
	if (!pRSAPubKey)
	{
		ERR_load_crypto_strings();
		char errBuf[512];
		ERR_error_string_n(ERR_get_error(), errBuf, sizeof(errBuf));
		BIO_free_all(bio);
		return "";
	}

	int nLen = RSA_size(pRSAPubKey);
	char* pDecode = new char[nLen + 1];

	int ret = RSA_public_decrypt(strData.length(), (const unsigned char*)strData.c_str(), (unsigned char*)pDecode, pRSAPubKey, RSA_PKCS1_PADDING);
	if (ret >= 0)
	{
		strRet = std::string((char*)pDecode, ret);
	}
	else
	{
		ERR_load_crypto_strings();
		char errBuf[512];
		ERR_error_string_n(ERR_get_error(), errBuf, sizeof(errBuf));
		BIO_free_all(bio);
	}
	delete[] pDecode;
	RSA_free(pRSAPubKey);
	CRYPTO_cleanup_all_ex_data();
	return strRet;
}

Register::Register(QWidget* parent)
	: QDialog(parent)
	, ui(new Ui::Register)
{
	ui->setupUi(this);
}

Register::~Register()
{
	delete ui;
}


void Register::on_ButtonRegister_clicked()
{
	char szHardwareCodeText[128] = { 0 };
	QString strHardwareCode = ui->lineEdit_HardwareCode->text();
	strcpy_s(szHardwareCodeText, 128, strHardwareCode.toStdString().c_str());

	unsigned char szHardCodeBinary[128] = { 0 };
	AscString2HexA(strHardwareCode.toStdString().c_str(), strHardwareCode.size(), szHardCodeBinary, 128, '\0');

	string strHardCodeBinary((const char*)szHardCodeBinary, 32);
	std::string strRegisterCode = EncryptRSAKey(g_szPrivateKey, strHardCodeBinary);
	char szRegisterCodeText[512] = { 0 };

	Hex2AscStringA((unsigned char*)strRegisterCode.c_str(), strRegisterCode.size(), szRegisterCodeText, 512, '\0');
	ui->textEdit_Registercode->setText(szRegisterCodeText);
}


void Register::on_ButtonExport_clicked()
{
	QString selectedFilter;
	strLicenseFile = QFileDialog::getSaveFileName(this,
		"导出授权文件",
		strLicenseFile,
		tr("授权文件(*.lic);;All Files (*)"),
		&selectedFilter);

	if (strLicenseFile.isEmpty())
	{
		return;
	}
	QString strMachineCode = ui->lineEdit_HardwareCode->text();
	if (strMachineCode.size() != 64)
	{
		QMessageBox::information(nullptr, "提示", "机器码必须为64字节的16进制字符串", QMessageBox::Ok);
		return;
	}
	QString strRegister = ui->textEdit_Registercode->toPlainText();

	if (strRegister.size() != 256)
	{
		QMessageBox::information(nullptr, "提示", "机器码必须为256字节的16进制字符串", QMessageBox::Ok);
		return;
	}

	QSettings License(strLicenseFile, QSettings::IniFormat);
	License.beginGroup("License");
	License.setValue("MachineCode", strMachineCode);
	License.setValue("RegisterCode", strRegister);
	License.endGroup();
}

bool Register::ImportLicene(QString& strMachineCode, QString& strLicense, QString& strMessage)
{
	QString selectedFilter;
	strLicenseFile = QFileDialog::getOpenFileName(this,
		"导入机器码",
		QCoreApplication::applicationDirPath(),
		tr("授权文件(*.lic);;All Files (*)"),
		&selectedFilter);

	if (strLicenseFile.isEmpty())
	{
		return false;
	}

	QSettings License(strLicenseFile, QSettings::IniFormat);
	License.beginGroup("License");
	strMachineCode = License.value("MachineCode", "").toString();
	if (strMachineCode.size() != 64)
	{
		strMessage = "所选文件中没有机器码或机器码无效!";
		return false;
	}
	strLicense = License.value("RegisterCode", "").toString();
	qDebug() << "MachineCode = " << strMachineCode;
	qDebug() << "License = " << strLicense;
	License.endGroup();
	return true;
}

void Register::on_ButtonImportMachineCode_clicked()
{
	QString strMachineCode;
	QString strLicense;
	QString strMessage;
	if (!ImportLicene(strMachineCode, strLicense, strMessage))
	{
		QMessageBox::information(nullptr, "提示", strMessage, QMessageBox::Ok);
		return;
	}
	ui->lineEdit_HardwareCode->setText(strMachineCode);
}

void Register::on_ButtonCheckLicense_clicked()
{
	QString strHardwareCode;
	QString strLicenseCode;
	QString strMessage;
	if (!ImportLicene(strHardwareCode, strLicenseCode, strMessage))
	{
		QMessageBox::information(nullptr, "提示", strMessage, QMessageBox::Ok);
		return;
	}
	if (strLicenseCode.size() != 256)
	{
		QMessageBox::information(nullptr, "提示", "选定的文件中没有授权信息或授权信息无效！", QMessageBox::Ok);
		return;
	}
	ui->lineEdit_HardwareCode->setText(strHardwareCode);
	ui->textEdit_Registercode->setText(strLicenseCode);
	char szHardwareCodeText[128] = { 0 };
	strcpy_s(szHardwareCodeText, 128, strHardwareCode.toStdString().c_str());

	unsigned char szHardCodeBinary[128] = { 0 };
	AscString2HexA(strHardwareCode.toStdString().c_str(), strHardwareCode.size(), szHardCodeBinary, 128, '\0');

	char szLicenseCodeA[512];
	strcpy(szLicenseCodeA, strLicenseCode.toStdString().c_str());

	char szLicenseBinary[256] = { 0 };
	int nSize = AscString2HexA(szLicenseCodeA, strlen(szLicenseCodeA), (unsigned char*)szLicenseBinary, 256, '\0');//注册码转换
	std::string strLicenseCodeBinary = std::string(szLicenseBinary, nSize);

	// 使用公钥解密码，得到硬件码,进行比较
	std::string strHardwareCode2 = DecryptRSAKey(g_szPublicKey, strLicenseCodeBinary);
	char szAscHardwareCode2[256] = { 0 };
	Hex2AscStringA((unsigned char*)strHardwareCode2.c_str(), strHardwareCode2.size(), szAscHardwareCode2, 256, '\0');
	if ((strHardwareCode.compare(szAscHardwareCode2) == 0))
		QMessageBox::information(nullptr, "提示", "授权校验通过", QMessageBox::Ok);
	else
		QMessageBox::critical(nullptr, "提示", "授权校验失败", QMessageBox::Ok);
}
