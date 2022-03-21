#ifndef QCLICKABLELINEEDIT_H
#define QCLICKABLELINEEDIT_H

#include <QObject>
#include <QLineEdit>

class QClickableLineEdit : public QLineEdit
{
    Q_OBJECT
public:

    explicit QClickableLineEdit(QWidget* parent = Q_NULLPTR, Qt::WindowFlags f = Qt::WindowFlags());
signals:
    void clicked();

protected:
    void mousePressEvent(QMouseEvent* event);
};

#endif // QCLICKABLELINEEDIT_H
