#pragma execution_character_set("utf-8")
#include "SystemConfigure.h"
#include <QCheckBox>
#include "Gloabal.h"


SystemConfigure::SystemConfigure(QWidget* parent)
	: QWidget(parent)
{
	ui.setupUi(this);
	/*m_pVarManager = new QtVariantPropertyManager(ui.PropertyWidget);
	m_pVarFactory = new QtVariantEditorFactory(ui.PropertyWidget);
	QtProperty* pGroupItem = m_pVarManager->addProperty(QtVariantPropertyManager::groupTypeId(), QString("地区"));
	QtVariantProperty* pSubitem = m_pVarManager->addProperty(QVariant::Int, QString("整形数据:"));
	pSubitem->setValue(101);
	pGroupItem->addSubProperty(pSubitem);

	pSubitem = m_pVarManager->addProperty(QVariant::Bool, QString("布尔型数据:"));
	pSubitem->setValue(true);
	pGroupItem->addSubProperty(pSubitem);

	pSubitem = m_pVarManager->addProperty(QVariant::Double, QString("浮点数据:"));
	pSubitem->setValue(3.1415926);
	pGroupItem->addSubProperty(pSubitem);

	pSubitem = m_pVarManager->addProperty(QVariant::String, QString("字符串数据:"));
	pSubitem->setValue(QString("姓名"));
	pGroupItem->addSubProperty(pSubitem);

	ui.PropertyWidget->addProperty(pGroupItem);
	ui.PropertyWidget->setFactoryForManager(m_pVarManager, m_pVarFactory);*/
	//SysConfigPtr &pSysCofnig = g_pDataCenter->GetSysConfigure();
	//int nRows = ui.tableWidget->rowCount();
	//for (int nIndex = 0;nIndex < nRows; nIndex ++)
	//{
	//    ui.tableWidget->item(nIndex,1)->setText(QString("%1").arg(pSysCofnig->nPageTimeout[nIndex]));
	//    QCheckBox * checkB = new QCheckBox(ui.tableWidget); // 创建checkbox
	//    checkB->setEnabled(false);                          // 该功能暂不开放
	//    checkB->setCheckState(Qt::Checked);                 // 设置状态
	//    QWidget *w = new QWidget(ui.tableWidget);           // 创建一个widget
	//    QHBoxLayout *hLayout = new QHBoxLayout();           // 创建布局
	//    hLayout->addWidget(checkB);                         // 添加checkbox
	//    hLayout->setMargin(0);                              // 设置边缘距离 否则会很难看
	//    hLayout->setAlignment(checkB, Qt::AlignCenter);     // 居中
	//    w->setLayout(hLayout);                              // 设置widget的布局
	//    ui.tableWidget->setCellWidget(nIndex, 2, w);        // 将widget放到table中
	//}
}

SystemConfigure::~SystemConfigure()
{

}
