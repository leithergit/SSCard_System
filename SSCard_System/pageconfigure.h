#ifndef PAGECONFIGURE_H
#define PAGECONFIGURE_H

#include <QWidget>
#include <QButtonGroup>
#include "ConfigurePage.h"
#include <map>
using namespace std;

namespace Ui {
	class PageConfigure;
}

class PageConfigure : public QWidget
{
	Q_OBJECT

public:
	explicit PageConfigure(QWidget* parent = nullptr);
	~PageConfigure();
	bool Save(QString& strMessage);
	QButtonGroup* m_pCheckBoxGroup = nullptr;
public slots:
	void on_TableCheckBoxStateChanged(int nIndex);
private:
	Ui::PageConfigure* ui;
    map<int,bool> m_mapPageStatus;
};

#endif // PAGECONFIGURE_H
