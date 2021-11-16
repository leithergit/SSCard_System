#ifndef MAINPAGE_H
#define MAINPAGE_H
#pragma execution_character_set("utf-8")
#include <QWidget>

namespace Ui {
	class MainPage;
}

class MainPage : public QWidget
{
	Q_OBJECT

public:
	explicit MainPage(QWidget* parent = nullptr);
	~MainPage();
	QWidget* m_pMainWindow = nullptr;

private slots:
	void on_pushButton_Updatecard_clicked();

	void on_pushButton_ChangePWD_clicked();

	void on_pushButton_RegisterLost_clicked();

private:
	Ui::MainPage* ui;
};

#endif // MAINPAGE_H
