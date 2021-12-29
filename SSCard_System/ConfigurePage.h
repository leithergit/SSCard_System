#ifndef CONFIGUREPAGE_H
#define CONFIGUREPAGE_H
#include <QObject>
#include <QWidget>
#include "Gloabal.h"
class ConfigurePage :public QWidget
{
	Q_OBJECT
public:
	ConfigurePage(QWidget* parent = Q_NULLPTR) :
		QWidget(parent)
	{
	}
	~ConfigurePage()
	{
	}
	virtual bool Save(QString& strMessage) = 0;
public slots:

signals:

};

#endif // CONFIGUREPAGE_H
