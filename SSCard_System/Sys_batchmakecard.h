#ifndef SYS_BATCHMAKECARD_H
#define SYS_BATCHMAKECARD_H

#include <QWidget>
#include <QButtonGroup>
#include "Gloabal.h"
using namespace std;

namespace Ui {
	class Sys_BatchMakeCard;
}

struct MakeCardInfo
{
	ServiceType		nServiceType;
	IDCardInfoPtr	pIDCard;
	SSCardBaseInfoPtr pSSCardInfo;
	bool			bFinished;
	MakeCardInfo(IDCardInfoPtr pIDCard)
	{
		pSSCardInfo = make_shared<SSCardBaseInfo>();
		this->pIDCard = pIDCard;
		nServiceType = ServiceType::Service_Unknown;
		bFinished = false;
	}
};
using MakeCardInfoPtr = shared_ptr<MakeCardInfo>;
class Sys_BatchMakeCard : public QWidget
{
	Q_OBJECT

public:
	explicit Sys_BatchMakeCard(QWidget* parent = nullptr);
	~Sys_BatchMakeCard();

	//map<string, IDCardInfoPtr> m_MapIDCardInfo;
	vector<MakeCardInfoPtr> vecMakeCardInfo;
	QButtonGroup* pButttonGrp = nullptr;
	void    ThreadBatchMakeCard();
	int		BuildNewCardInfo(QString& strMessage);
	int		BuildUpdateCardInfo(QString& strMessage);
	int		ImportNewIDCard(vector<QString>& vecInfo);

	void    ThreadReadIDCard();
	volatile bool    m_bWorkThreadRunning = false;
	std::thread* m_pWorkThread = nullptr;

signals:
	void AddNewIDCard(IDCardInfo* IDCard);
	void ShowMessage(QString strMessage);
	void UpdateTableWidget(int nRow, int nCol, QString strMessage);
private slots:
	void on_pushButton_StartReadIDCard_clicked();

	void on_pushButton_StartMakeCard_clicked();

	void on_pushButton_StopMakeCard_clicked();

	void on_AddNewIDCard(IDCardInfo* pIDCard);

	void on_ShowMessage(QString strMessage);

	void on_UpdateTableWidget(int nRow, int nCol, QString strMessage);

	void on_pushButton_ImportList_clicked();

	void on_pushButton_ExportList_clicked();

	//void on_ComboxoDeledateModelIndexChanged(int iRow, int iColumn, int nIndex);

private:
	Ui::Sys_BatchMakeCard* ui;
};

#endif // SYS_BATCHMAKECARD_H
