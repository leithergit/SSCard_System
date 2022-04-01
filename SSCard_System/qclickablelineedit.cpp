#include "qclickablelineedit.h"


QClickableLineEdit::QClickableLineEdit(QWidget* parent, Qt::WindowFlags f)
	: QLineEdit(parent) {
	Q_UNUSED(f);
}

void QClickableLineEdit::mousePressEvent(QMouseEvent* event)
{
	emit clicked();
}
