#ifndef SYS_BATCHMAKECARD_H
#define SYS_BATCHMAKECARD_H

#include <QWidget>
#include <QButtonGroup>
#include "Gloabal.h"
using namespace std;

namespace Ui {
	class Sys_BatchMakeCard;
}

class Sys_BatchMakeCard : public QWidget
{
	Q_OBJECT

public:
	explicit Sys_BatchMakeCard(QWidget* parent = nullptr);
	~Sys_BatchMakeCard();
	int     ReaderIDCard(IDCardInfo* pIDCard);
	void    ThreadWork();
	volatile bool    m_bWorkThreadRunning = false;
	std::thread* m_pWorkThread = nullptr;
	string  m_strDevPort;
	ushort  m_nBaudreate = 9600;
	map<string, IDCardInfoPtr> m_MapIDCardInfo;
	QButtonGroup* pButttonGrp = nullptr;

signals:
	void AddNewIDCard(IDCardInfo* IDCard);
private slots:
	void on_pushButton_StartReadIDCard_clicked();

	void on_pushButton_StopReadIDCard_clicked();

	void on_pushButton_ImportList_clicked();

	void on_pushButton_StartMakeCard_clicked();

	void on_pushButton_StopMakeCard_clicked();

	void on_pushButton__ExportList_clicked();

	void on_AddNewIDCard(IDCardInfo* pIDCard);

private:
	Ui::Sys_BatchMakeCard* ui;
};

#endif // SYS_BATCHMAKECARD_H
