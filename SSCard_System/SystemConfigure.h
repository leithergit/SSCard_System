#pragma once

#include <QWidget>
#include "ui_SystemConfigure.h"
#include <QTranslator>
#include "./qtpropertybrowser/qtpropertybrowser.h"
#include "./qtpropertybrowser/qtpropertymanager.h"
#include "./qtpropertybrowser/qtvariantproperty.h"

class SystemConfigure : public QWidget
{
	Q_OBJECT

public:
	SystemConfigure(QWidget* parent = Q_NULLPTR);
	~SystemConfigure();
	QtVariantPropertyManager* m_pVarManager = nullptr;
	QtVariantEditorFactory* m_pVarFactory = nullptr;

private:
	Ui::SystemConfigure ui;
};
