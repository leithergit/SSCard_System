#pragma execution_character_set("utf-8")
#include "up_ensurepwd.h"
#include "ui_up_ensurepwd.h"

#include <QString>
#include <chrono>
#include <QMessageBox>

#pragma comment(lib,"user32.lib")
#pragma comment(lib,"advapi32.lib")
#pragma comment(lib,"OleAut32.lib")


up_EnsurePWD::up_EnsurePWD(QLabel* pTitle, int nTimeout, QWidget* parent) :
	QStackPage(pTitle, nTimeout, parent),
	ui(new Ui::EnsurePWD)
{
	ui->setupUi(this);
}

up_EnsurePWD::~up_EnsurePWD()
{
	delete ui;
}

int up_EnsurePWD::ProcessBussiness()
{
	return 0;
}

void up_EnsurePWD::OnTimeout()
{

}

void up_EnsurePWD::on_pushButton_EnsurePWD_clicked()
{
	emit SwitchNextPage();
}
