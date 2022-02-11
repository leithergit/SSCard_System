#ifndef REGISTER_H
#define REGISTER_H

#pragma execution_character_set("utf-8")
#include <QDialog>

QT_BEGIN_NAMESPACE
namespace Ui { class Register; }
QT_END_NAMESPACE

class Register : public QDialog
{
	Q_OBJECT

public:
	Register(QWidget* parent = nullptr);
	~Register();
	QString strLicenseFile;

private slots:
	void on_ButtonRegister_clicked();

	void on_ButtonExport_clicked();

	void on_ButtonImportMachineCode_clicked();

	void on_ButtonCheckLicense_clicked();

private:
	Ui::Register* ui;
};
#endif // REGISTER_H
