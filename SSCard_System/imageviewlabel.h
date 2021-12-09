#ifndef IMAGEVIEWLABEL_H
#define IMAGEVIEWLABEL_H

#include <QBoxLayout>
#include <QButtonGroup>
#include <QLabel>
#include <QObject>
#include <QParallelAnimationGroup>
#include <QPropertyAnimation>
#include <QWidget>

class ImageViewLabel : public QLabel
{
	Q_OBJECT
public:
	ImageViewLabel(QWidget* parent = nullptr);
	~ImageViewLabel();

	void addImage(const QList<std::pair<QString, QString> >& imagepathpairlst);
protected:
	void paintEvent(QPaintEvent* event);
	void resizeEvent(QResizeEvent* event);
private:
	int m_offset, m_curIndex, m_preIndex;
	bool m_blefttoright;

	//QButtonGroup* m_btnGroup;
	QParallelAnimationGroup* m_btnParalGroup;
	QPropertyAnimation* m_imageAnimation, * m_btnExpAnimation, * m_btnShrikAnimation;
	//QWidget* m_horizontalLayoutWidget;
	//QHBoxLayout* m_switchBtnLayout;
	QTimer* m_imageTimer;
	QList<std::pair<QString, QString>> m_imagepathpairlst;
private:

	void initControl();
private slots:
	void onImagevalueChanged(const QVariant& variant);
	void onBtnExpvalueChanged(const QVariant& variant);
	void onBtnShrikvalueChanged(const QVariant& variant);
	void onbuttonClicked(int index);
	void onImageShowTimeOut();
};

#endif // IMAGEVIEWLABEL_H
