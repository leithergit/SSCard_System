#ifndef SSCARDREPORT_H
#define SSCARDREPORT_H

#include <QWidget>
#include <vector>
#include <QString>
#include "../utility/json/CJsonObject.hpp"
using namespace std;
using namespace neb;

namespace Ui {
	class SSCardReport;
}

class SSCardReport : public QWidget
{
	Q_OBJECT

public:
	explicit SSCardReport(QWidget* parent = nullptr);
	~SSCardReport();
	int  SearchCardProgres();
	int  LoadCardProgress();
	bool GetSSCardInfo(CJsonObject&& Json, string& strSSCard, string& strBankCard);
	vector<QString> vecCardProgress;

private slots:


	void on_pushButton_Send_clicked();

private:
	Ui::SSCardReport* ui;
};

#endif // SSCARDREPORT_H
