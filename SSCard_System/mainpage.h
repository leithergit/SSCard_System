#ifndef MAINPAGE_H
#define MAINPAGE_H
#pragma execution_character_set("utf-8")
#include <QWidget>
#include "imageviewlabel.h"
#include "Gloabal.h"

namespace Ui {
	class MainPage;
}

struct ServiceDesc
{
	QString		strQR_Photo;
	QString		strService[3];
};

class MainPage : public QWidget
{
	Q_OBJECT

public:
	explicit MainPage(QWidget* parent = nullptr);
	~MainPage();
	QWidget* m_pMainWindow = nullptr;
	ServiceDesc	m_ServiceDescription[2];
	bool LoadServiceDescription();

private slots:
	void on_pushButton_Updatecard_clicked();

	void on_pushButton_ChangePWD_clicked();

	void on_pushButton_RegisterLost_clicked();

    void on_pushButton_NewCard_clicked();

private:
	Ui::MainPage* ui;
};

#endif // MAINPAGE_H
