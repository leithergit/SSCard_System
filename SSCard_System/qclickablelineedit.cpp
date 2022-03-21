#include "qclickablelineedit.h"


QClickableLineEdit::QClickableLineEdit(QWidget* parent, Qt::WindowFlags f)
    : QLineEdit(parent) {
}

void QClickableLineEdit::mousePressEvent(QMouseEvent* event)
{
    emit clicked();
}
