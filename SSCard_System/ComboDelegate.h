#pragma once

#include <QtWidgets/QItemDelegate>
#include <QtWidgets/QStyledItemDelegate>

class ReadOnlyDelegate : public QItemDelegate
{

public:
	ReadOnlyDelegate(QWidget* parent = NULL) :QItemDelegate(parent)
	{}

	QWidget* createEditor(QWidget* parent, const QStyleOptionViewItem& option,
		const QModelIndex& index) const override //final
	{
		Q_UNUSED(parent)
			Q_UNUSED(option)
			Q_UNUSED(index)
			return NULL;
	}
};


class RegExpEditorDelegate : public QItemDelegate
{

public:
	RegExpEditorDelegate(QRegExp* pRegExp = nullptr, QObject* parent = 0);
	~RegExpEditorDelegate();
	QWidget* createEditor(QWidget* parent, const QStyleOptionViewItem& option, const QModelIndex& index) const;
	void setEditorData(QWidget* editor, const QModelIndex& index) const;
	void setModelData(QWidget* editor, QAbstractItemModel* model, const QModelIndex& index) const;
private:
	QRegExp* pRegExp = nullptr;
};


class ComboBoxDelegate : public QItemDelegate
{
	Q_OBJECT

public:
	ComboBoxDelegate(const QStringList& items, bool bEditable = false, QObject* parent = 0);
	~ComboBoxDelegate();

	QWidget* createEditor(QWidget* parent, const QStyleOptionViewItem& option, const QModelIndex& index) const;
	void setEditorData(QWidget* editor, const QModelIndex& index) const;
	void setModelData(QWidget* editor, QAbstractItemModel* model, const QModelIndex& index) const;
	void updateEditorGeometry(QWidget* editor, const QStyleOptionViewItem& option, const QModelIndex& index) const;
	int nOpIndex = 0;
	void UpdateItems(const QStringList& items);
	void UpdateItemsStatus(QList<bool>& listStatus);
	void EnableItem(int nIndex, bool bEnable);
	void EnableItem(QString strItem, bool bEnable);
signals:
	void ComboxCurrentIndexChanged(int iRow, int iColumn) const;			// 定义消息必须有const
	void ComboxIndexChanged(int iRow, int iColumn, int nIndex) const;		// ComboBox选项被改变,但尚未改变数据，用户可以继续作其它选项
	void ModelIndexChanged(int iRow, int iColumn, int nIndex) const;		// 数据被改变
private slots:
	void OnCurrentIndexChanged(int nIndex)
	{
		if (nRow != -1 && nCol != -1 && nIndex != -1)
			emit ComboxIndexChanged(nRow, nCol, nIndex);
	}
private:
	QList<bool> ItemStatus;
	QStringList myItems;
	int nRow = -1;
	int nCol = -1;
	bool bEditable = false;
	void SetIndex(const QModelIndex& Index)
	{
		nRow = Index.row();
		nCol = Index.column();
	}
};

struct DeviceTypeDesc
{
	int nType;
	QString strTypeName;
};
using DeviceTypeDescList = QList<DeviceTypeDesc>;
class ComboBoxDevTypeDelegate : public QItemDelegate
{
	Q_OBJECT

public:
	ComboBoxDevTypeDelegate(const DeviceTypeDescList& items, QObject* parent = 0);
	~ComboBoxDevTypeDelegate();

	QWidget* createEditor(QWidget* parent, const QStyleOptionViewItem& option, const QModelIndex& index) const;
	void setEditorData(QWidget* editor, const QModelIndex& index) const;
	void setModelData(QWidget* editor, QAbstractItemModel* model, const QModelIndex& index) const;
	void updateEditorGeometry(QWidget* editor, const QStyleOptionViewItem& option, const QModelIndex& index) const;
	int nOpIndex = 0;
signals:
	void ComboxCurrentIndexChanged(int iRow, int iColumn) const;			// 定义消息必须有const
	void ComboxIndexChanged(int iRow, int iColumn, int nIndex) const;		// ComboBox选项被改变,但尚未改变数据，用户可以继续作其它选项
	void ModelIndexChanged(int iRow, int iColumn, int nIndex) const;		// 数据被改变
private slots:
	void OnCurrentIndexChanged(int nIndex)
	{
		if (nRow != -1 && nCol != -1 && nIndex != -1)
			emit ComboxIndexChanged(nRow, nCol, nIndex);
	}
private:
	DeviceTypeDescList myItems;
	int nRow = -1;
	int nCol = -1;
	void SetIndex(const QModelIndex& Index)
	{
		nRow = Index.row();
		nCol = Index.column();
	}
};



class CheckBoxComboDelegate : public QItemDelegate
{
	Q_OBJECT

public:
	CheckBoxComboDelegate(const QStringList& items, QObject* parent = 0);
	~CheckBoxComboDelegate();
	QWidget* createEditor(QWidget* parent, const QStyleOptionViewItem& option, const QModelIndex& index) const;
	void setEditorData(QWidget* editor, const QModelIndex& index) const;
	void setModelData(QWidget* editor, QAbstractItemModel* model, const QModelIndex& index) const;
	void updateEditorGeometry(QWidget* editor, const QStyleOptionViewItem& option, const QModelIndex& index) const;
private:
	QStringList myItems;
};


class PasswordDelegate : public QItemDelegate
{
	Q_OBJECT

public:
	PasswordDelegate(QObject* parent = 0);
	~PasswordDelegate();
	QWidget* createEditor(QWidget* parent, const QStyleOptionViewItem& option, const QModelIndex& index) const;
	void setEditorData(QWidget* editor, const QModelIndex& index) const;
	void setModelData(QWidget* editor, QAbstractItemModel* model, const QModelIndex& index) const;
};
