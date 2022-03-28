#include "ComboDelegate.h"
#include <QtWidgets/QComboBox>
#include <QDebug>
#include "QCheckComboBox.h"
#include <QLineEdit>
#include <QRegExpValidator>
#include <algorithm>
using namespace std;

ComboBoxDelegate::ComboBoxDelegate(const QStringList &items, bool bEditable , QObject *parent) :
QItemDelegate(parent)
{
	myItems = items;
	this->bEditable = bEditable;
	for (int i = 0; i < myItems.size();i ++)
	{
		ItemStatus.push_back(true);
	}
}

void ComboBoxDelegate::UpdateItems(const QStringList &items)
{
	myItems = items;
	ItemStatus.clear();
	for (int i = 0; i < myItems.size(); i++)
	{
		ItemStatus.push_back(true);
	}
}

void ComboBoxDelegate::UpdateItemsStatus( QList<bool> &listStatus)
{
	if (listStatus.size() == myItems.size())
	{
		ItemStatus.swap(listStatus);
	}
}
void ComboBoxDelegate::EnableItem(int nIndex,bool bEnable)
{
	if (nIndex >=0 && nIndex < ItemStatus.size())
		ItemStatus[nIndex] = bEnable;
}

void ComboBoxDelegate::EnableItem(QString strItem, bool bEnable)
{
	auto itFind = find_if(myItems.begin(), myItems.end(), [&](QString &var){return strItem == var; });
	if (itFind != myItems.end())
	{
		int nIndex = itFind - myItems.begin();
		EnableItem(nIndex, bEnable);
	}

}
ComboBoxDelegate::~ComboBoxDelegate()
{
	int a = 3;
}

QWidget *ComboBoxDelegate::createEditor(QWidget *parent, const QStyleOptionViewItem &, const QModelIndex &) const
{
	QComboBox *pComboBox = new QComboBox(parent);
	pComboBox->setStyleSheet("QComboBox QAbstractItemView::item::!enabled{color:rgb(125, 125, 125);");
	//pComboBox->setView(new QListView);		// 不用QListView也可以实现禁用风格
	pComboBox->addItems(myItems);
	//pComboBox->setMaxVisibleItems(myItems.size() + 1);

	for (int nIndex = 0; nIndex < myItems.size(); nIndex++)
	{
		QModelIndex ModelIndex = pComboBox->model()->index(nIndex, 0);
		if (ItemStatus[nIndex])
			pComboBox->model()->setData(ModelIndex, QVariant(-1), Qt::UserRole - 1);
		else
			pComboBox->model()->setData(ModelIndex, QVariant(0), Qt::UserRole - 1);
	}
	
	pComboBox->setEditable(bEditable);
	
	qDebug() << __FUNCTION__  << endl;
	return pComboBox;
}

void ComboBoxDelegate::setEditorData(QWidget *editor, const QModelIndex &index) const
{
	QComboBox *pComboBox = static_cast<QComboBox*>(editor);
	connect(editor, SIGNAL(currentIndexChanged(int)), this, SLOT(OnComboBoxChanged(int)));
}

void ComboBoxDelegate::setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const
{
	QComboBox *pComboBox = static_cast<QComboBox*>(editor);
	QString value = pComboBox->currentText();
	model->setData(index, value, Qt::EditRole);
	int nCurIndex = pComboBox->currentIndex();
	if (nCurIndex >= 0)
		emit ModelIndexChanged(index.row(), index.column(), nCurIndex);   //发送信号，实现其它联动更新
}

void ComboBoxDelegate::updateEditorGeometry(QWidget *editor,const QStyleOptionViewItem &option, const QModelIndex & index) const
{
	editor->setGeometry(option.rect);
	QComboBox *pComboBox = static_cast<QComboBox*>(editor);
	QString value = index.model()->data(index, Qt::EditRole).toString();
	int icurIndex = pComboBox->findText(value);
	//if (icurIndex >= 0)
		pComboBox->setCurrentIndex(icurIndex);
	(const_cast<ComboBoxDelegate *>(this))->SetIndex(index);
	pComboBox->showPopup();
}

// 设备类型代理
ComboBoxDevTypeDelegate::ComboBoxDevTypeDelegate(const DeviceTypeDescList &items, QObject *parent) :
QItemDelegate(parent)
{
	myItems = items;
}

ComboBoxDevTypeDelegate::~ComboBoxDevTypeDelegate()
{
}
QWidget *ComboBoxDevTypeDelegate::createEditor(QWidget *parent, const QStyleOptionViewItem &, const QModelIndex &) const
{
	QComboBox *pComboBox = new QComboBox(parent);
	for (auto var : myItems)
	{
		pComboBox->addItem(var.strTypeName);
		pComboBox->setEditable(false);
	}

	return pComboBox;
}

void ComboBoxDevTypeDelegate::setEditorData(QWidget *editor, const QModelIndex &index) const
{
	QComboBox *pComboBox = static_cast<QComboBox*>(editor);
	connect(editor, SIGNAL(currentIndexChanged(int)), this, SLOT(OnComboBoxChanged(int)));
}

void ComboBoxDevTypeDelegate::setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const
{
	QComboBox *pComboBox = static_cast<QComboBox*>(editor);
	QString value = pComboBox->currentText();
	model->setData(index, value, Qt::EditRole);
	int nCurIndex = pComboBox->currentIndex();
	if (nCurIndex >= 0)
		emit ModelIndexChanged(index.row(), index.column(), nCurIndex);   //发送信号，实现其它联动更新
}

void ComboBoxDevTypeDelegate::updateEditorGeometry(QWidget *editor, const QStyleOptionViewItem &option, const QModelIndex & index) const
{
	editor->setGeometry(option.rect);
	QComboBox *pComboBox = static_cast<QComboBox*>(editor);
	
	QString value = index.model()->data(index, Qt::EditRole).toString();
	int icurIndex = pComboBox->findText(value);
	if (icurIndex >= 0)
		pComboBox->setCurrentIndex(icurIndex);
	(const_cast<ComboBoxDevTypeDelegate *>(this))->SetIndex(index);
	pComboBox->showPopup();
}

// CheckComboDelagate 定义
CheckBoxComboDelegate::CheckBoxComboDelegate(const QStringList &items, QObject *parent) :
	QItemDelegate(parent)
{
	myItems = items;
}

QWidget *CheckBoxComboDelegate::createEditor(QWidget *parent, const QStyleOptionViewItem &, const QModelIndex &) const
{
	QCheckComboBox *pComboBox = new QCheckComboBox(parent);
	pComboBox->AddItems(myItems);
	// 下面这句会导致combobox无法弹出
	//pComboBox->setEditable(false);

	return pComboBox;
}
CheckBoxComboDelegate::~CheckBoxComboDelegate()
{
}

void CheckBoxComboDelegate::setEditorData(QWidget *editor, const QModelIndex &index) const
{
	QString value = index.model()->data(index, Qt::EditRole).toString();
	QCheckComboBox *pCheckComboBox = static_cast<QCheckComboBox*>(editor);
	pCheckComboBox->CheckString(value);
}

void CheckBoxComboDelegate::setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const
{
	QCheckComboBox *pComboBox = static_cast<QCheckComboBox*>(editor);
	QString value = pComboBox->currentText();
	model->setData(index, value, Qt::EditRole);
}

void CheckBoxComboDelegate::updateEditorGeometry(QWidget *editor, const QStyleOptionViewItem &option, const QModelIndex & index) const
{
	editor->setGeometry(option.rect);	
	QString value = index.model()->data(index, Qt::EditRole).toString();
	QCheckComboBox *pCheckComboBox = static_cast<QCheckComboBox*>(editor);
	pCheckComboBox->showPopup();
}


// RegExpEditorDelegate 定义
RegExpEditorDelegate::RegExpEditorDelegate(QRegExp *pRegExp, QObject *parent) :
QItemDelegate(parent)
{
	this->pRegExp = pRegExp;
}

QWidget *RegExpEditorDelegate::createEditor(QWidget *parent, const QStyleOptionViewItem &, const QModelIndex &) const
{
	QLineEdit *pEditor = new QLineEdit(parent);	
	QRegExpValidator *pRegExpValidator = new QRegExpValidator(*pRegExp, pEditor);
	pEditor->setValidator(pRegExpValidator);
	return pEditor;
}
RegExpEditorDelegate::~RegExpEditorDelegate()
{
}

void RegExpEditorDelegate::setEditorData(QWidget *editor, const QModelIndex &index) const
{
	QString value = index.model()->data(index, Qt::EditRole).toString();
	QLineEdit *pEditor = static_cast<QLineEdit*>(editor);
	pEditor->setText(value);
}

void RegExpEditorDelegate::setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const
{
	QLineEdit *pEditor = static_cast<QLineEdit*>(editor);
	model->setData(index, pEditor->text(), Qt::EditRole);
}

// PasswordDelagate 定义
PasswordDelegate::PasswordDelegate(QObject *parent) :
QItemDelegate(parent)
{
}

QWidget *PasswordDelegate::createEditor(QWidget *parent, const QStyleOptionViewItem &, const QModelIndex &) const
{
	QLineEdit *pEditor = new QLineEdit(parent);
	pEditor->setEchoMode(QLineEdit::PasswordEchoOnEdit);
	return pEditor;
}
PasswordDelegate::~PasswordDelegate()
{
}

void PasswordDelegate::setEditorData(QWidget *editor, const QModelIndex &index) const
{
	QString value = index.model()->data(index, Qt::UserRole).toString();
	QLineEdit *pEditor = static_cast<QLineEdit*>(editor);
	pEditor->setText(value);
}

void PasswordDelegate::setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const
{
	QLineEdit *pEditor = static_cast<QLineEdit*>(editor);
	model->setData(index, pEditor->text(), Qt::UserRole);
}
