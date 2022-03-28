#include "QCheckComboBox.h"

#include <QLineEdit>
#include <QMouseEvent>
#include <QDebug>

QCheckComboBox::QCheckComboBox(QWidget* parent)
	: QComboBox(parent)
{
	pLineEdit = new QLineEdit(this);
	pLineEdit->setReadOnly(true);
	this->setLineEdit(pLineEdit);
	this->lineEdit()->disconnect();

	KeyPressEater* keyPressEater = new KeyPressEater(this);

	connect(keyPressEater, SIGNAL(signActivated(int)), this, SLOT(SlotActivated(int)));
	pListView = new QListView(this);
	pListView->installEventFilter(keyPressEater);
	this->setView(pListView);
	this->setModel(&m_model);

	//installEventFilter(this);
	connect(this, SIGNAL(activated(int)), this, SLOT(SlotActivated(int)));

}

QCheckComboBox::~QCheckComboBox()
{
}

void QCheckComboBox::AddItem(const QString& str, bool bChecked /*= false*/, QVariant userData /*= QVariant()*/)
{
	QStandardItem* item = new QStandardItem(str);
	item->setCheckable(true);
	item->setCheckState(bChecked ? Qt::Checked : Qt::Unchecked);
	item->setData(userData, Qt::UserRole + 1);
	m_model.appendRow(item);

	UpdateText();
}

void QCheckComboBox::AddItems(const QList<ItemInfo>& lstItemInfo)
{
	for (auto a : lstItemInfo)
	{
		AddItem(a.str, a.bChecked, a.userData);
	}
}

void QCheckComboBox::AddItems(const QMap<QString, bool>& mapStrChk)
{
	for (auto it = mapStrChk.begin(); it != mapStrChk.end(); ++it)
	{
		AddItem(it.key(), it.value());
	}
}

void QCheckComboBox::AddItems(const QList<QString>& lstStr)
{
	for (auto a : lstStr)
	{
		AddItem(a, false);
	}
}

void QCheckComboBox::RemoveItem(int idx)
{
	m_model.removeRow(idx);
	UpdateText();
}

void QCheckComboBox::Clear()
{
	m_model.clear();
	UpdateText();
}

QStringList QCheckComboBox::GetSelItemsText()
{
	QStringList lst;
	QString str = pLineEdit->text();
	if (str.isEmpty())
	{
		return lst;
	}
	else
	{
		return pLineEdit->text().split(",");
	}
}

QList<ItemInfo> QCheckComboBox::GetSelItemsInfo()
{
	QList<ItemInfo> lstInfo;
	for (int i = 0; i < m_model.rowCount(); i++)
	{
		QStandardItem* item = m_model.item(i);
		if (item->checkState() == Qt::Unchecked) continue;

		ItemInfo info;
		info.idx = i;
		info.str = item->text();
		info.bChecked = true;
		info.userData = item->data(Qt::UserRole + 1);

		lstInfo << info;
	}

	return lstInfo;
}

QString QCheckComboBox::GetItemText(int idx)
{
	if (idx < 0 || idx >= m_model.rowCount())
	{
		return QString("");
	}

	return m_model.item(idx)->text();
}

ItemInfo QCheckComboBox::GetItemInfo(int idx)
{
	ItemInfo info;
	if (idx < 0 || idx >= m_model.rowCount())
	{
		return info;
	}

	QStandardItem* item = m_model.item(idx);
	info.idx = idx;
	info.str = item->text();
	info.bChecked = (item->checkState() == Qt::Checked);
	info.userData = item->data(Qt::UserRole + 1);

	return info;
}

void QCheckComboBox::showPopup()
{
	emit showingPopup();
	QComboBox::showPopup();
}

void QCheckComboBox::hidePopup()
{
	int width = this->view()->width();
	int height = this->view()->height();
	int x = QCursor::pos().x() - mapToGlobal(geometry().topLeft()).x() + geometry().x();
	int y = QCursor::pos().y() - mapToGlobal(geometry().topLeft()).y() + geometry().y();

	QRect rectView(0, this->height(), width, height);
	if (!rectView.contains(x, y))
	{
		emit hidingPopup();
		QComboBox::hidePopup();
	}
}

void QCheckComboBox::mousePressEvent(QMouseEvent* event)
{
	QComboBox::mousePressEvent(event);
	event->accept();
}

void QCheckComboBox::mouseReleaseEvent(QMouseEvent* event)
{
	QComboBox::mouseReleaseEvent(event);
	event->accept();
}

void QCheckComboBox::mouseMoveEvent(QMouseEvent* event)
{
	QComboBox::mouseMoveEvent(event);
	event->accept();
}

void QCheckComboBox::UpdateText()
{
	QStringList lstTxt;
	for (int i = 0; i < m_model.rowCount(); ++i)
	{
		QStandardItem* item = m_model.item(i);
		if (item->checkState() == Qt::Unchecked) continue;

		lstTxt << item->text();
	}

	pLineEdit->setText(lstTxt.join(","));
	pLineEdit->setToolTip(lstTxt.join("\n"));
}

void QCheckComboBox::SlotActivated(int idx)
{
	QStandardItem* item = m_model.item(idx);
	if (nullptr == item)
		return;

	Qt::CheckState state = (item->checkState() == Qt::Checked) ? Qt::Unchecked : Qt::Checked;
	item->setCheckState(state);

	UpdateText();
}

void QCheckComboBox::SetCheck(int idx, bool bChecked)
{
	QStandardItem* item = m_model.item(idx);
	if (nullptr == item)
		return;
	Qt::CheckState state = bChecked ? Qt::Unchecked : Qt::Checked;
	item->setCheckState(state);
	UpdateText();
}

void QCheckComboBox::CheckString(QString strItemList)
{
	QStringList strList = strItemList.split(",");
	if (strList.size() == 0)
		return;
	for (auto s : strList)
	{
		int nRows = m_model.rowCount();
		for (int nIndex = 0; nIndex < nRows; nIndex++)
		{
			if (m_model.item(nIndex)->text() == s)
			{
				m_model.item(nIndex)->setCheckState(Qt::Checked);
				break;
			}
		}
	}
	pLineEdit->setText(strItemList);
	pLineEdit->setToolTip(strItemList);
}
