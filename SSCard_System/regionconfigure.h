#ifndef REGIONCONFIGURE_H
#define REGIONCONFIGURE_H

#include <QWidget>
#include "ConfigurePage.h"

namespace Ui {
	class RegionConfigure;
}

class RegionConfigure : public QWidget
{
	Q_OBJECT

public:
	explicit RegionConfigure(QWidget* parent = nullptr);
	~RegionConfigure();
	bool Save(QString& strMessage);

private:
	Ui::RegionConfigure* ui;
};

#endif // REGIONCONFIGURE_H
