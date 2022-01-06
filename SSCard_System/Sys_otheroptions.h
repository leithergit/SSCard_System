#ifndef SYS_OTHEROPTIONS_H
#define SYS_OTHEROPTIONS_H

#include <QWidget>
#include "Gloabal.h"

namespace Ui {
	class OtherOptions;
}

class OtherOptions : public QWidget
{
	Q_OBJECT

public:
	explicit OtherOptions(QWidget* parent = nullptr);

	~OtherOptions();
	QButtonGroup* m_pCheckBoxGroup = nullptr;

private slots:
	void on_pushButton_TestUploadLog_clicked();
	void on_TableCheckBoxStateChanged(int nIndex);

private:
	Ui::OtherOptions* ui;
};

#endif // SYS_OTHEROPTIONS_H
