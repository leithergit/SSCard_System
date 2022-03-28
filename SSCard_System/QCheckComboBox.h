#pragma once

#include <QComboBox>
#include <QStandardItemModel>
#include <QListView>
#include <QKeyEvent>
#include <QtWidgets/QListWidget>

class QLineEdit;
class QListView;

struct ItemInfo
{
	int idx;
	QString str;
	QVariant userData;
	bool bChecked;

	ItemInfo()
	{
		idx = -1;
		str = QString("");
		userData = QVariant();
		bChecked = false;
	}
};

// 事件过滤器
class KeyPressEater : public QObject
{
	Q_OBJECT
public:
	KeyPressEater(QObject* parent = nullptr) :QObject(parent) {}
	~KeyPressEater() {}
signals:
	void signActivated(int idx);

protected:
	bool eventFilter(QObject* obj, QEvent* event)
	{
		if (event->type() == QEvent::KeyPress)
		{
			QKeyEvent* keyEvent = static_cast<QKeyEvent*>(event);
			if (keyEvent->key() == Qt::Key_Space)
			{
				QListView* lstV = qobject_cast<QListView*>(obj);
				if (nullptr != lstV)
				{
					int idx = lstV->currentIndex().row();
					if (-1 != idx)
					{
						emit signActivated(idx);
					}
				}
			}
			else if (keyEvent->key() == Qt::Key_Up || keyEvent->key() == Qt::Key_Down)
			{
				return QObject::eventFilter(obj, event);
			}

			return true;
		}
		else {
			// standard event processing
			return QObject::eventFilter(obj, event);
		}
	}
};

class QCheckComboBox : public QComboBox
{
	Q_OBJECT

public:
	QCheckComboBox(QWidget* parent = Q_NULLPTR);
	~QCheckComboBox();

	// 添加item
	void AddItem(const QString& str, bool bChecked = false, QVariant userData = QVariant());
	void AddItems(const QList<ItemInfo>& lstItemInfo);
	void AddItems(const QMap<QString, bool>& mapStrChk);
	void AddItems(const QList<QString>& lstStr);
	void SetCheck(int idx, bool bChecked = true);
	void CheckString(QString strItemList);
	// 删除item
	void RemoveItem(int idx);
	// 清空item
	void Clear();
	// 获取选中的数据字符串列表
	QStringList GetSelItemsText();
	// 获取选中item的信息
	QList<ItemInfo> GetSelItemsInfo();
	// 获取item文本
	QString GetItemText(int idx);
	// 获取item信息
	ItemInfo GetItemInfo(int idx);
	void showPopup();
	// 重写QComboBox的hidePopup函数
	// 目的选择过程中，不隐藏listview
	void hidePopup();

signals:
	// popup显示信号
	void showingPopup();
	// popup隐藏信号
	void hidingPopup();
	void signActivated(int idx);
	void signDeatctive(QWidget* pWidget);

protected:

	virtual void mousePressEvent(QMouseEvent* event);
	virtual void mouseReleaseEvent(QMouseEvent* event);
	virtual void mouseMoveEvent(QMouseEvent* event);
	bool eventFilter(QObject* pWatched, QEvent* event)
	{
		if (pWatched != this)
			return false;

		if (QEvent::WindowDeactivate == event->type())
		{
			emit signDeatctive(this);
			return true;
		}
		else
			return QObject::eventFilter(pWatched, event);
	}


private:
	void UpdateText();

private slots:
	void SlotActivated(int idx);

private:
	QLineEdit* pLineEdit;
	QListView* pListView;
	QListWidget* pListWidget;
	QStandardItemModel m_model;
};
