#ifndef SYS_SSCARDSERVICET_H
#define SYS_SSCARDSERVICET_H

#include <QWidget>
#include <QButtonGroup>
#include "../SSCardService/SSCardService.h"

namespace Ui {
	class Sys_SSCardServiceT;
}

class SSCardServiceT : public QWidget
{
	Q_OBJECT

public:
	explicit SSCardServiceT(QWidget* parent = nullptr);
	~SSCardServiceT();

	QButtonGroup* pBtnGroup = nullptr;

private slots:
	void on_pushButton_LoadCardID_clicked();

	void on_pushButton_QueryCardStatus_clicked();

	void on_pushButton_CommitInfo_clicked();

	void on_pushButton_CreateService_clicked();

	void on_pushButton_PremakeCard_clicked();

	void on_pushButton_WriteCard_clicked();

	void on_pushButton_ReturnCard_clicked();

	void on_pushButton_ActiveCard_clicked();

	void on_pushButton_LoadPreMakeCard_clicked();

	void on_pushButton_GetPhoto_clicked();

	void on_pushButton_QueryCardInfo_clicked();

    void on_pushButton_UnRegisterLost_clicked();

    void on_pushButton_ReadBankNum_clicked();

private:
	Ui::Sys_SSCardServiceT* ui;
};

#endif // SYS_SSCARDSERVICET_H
