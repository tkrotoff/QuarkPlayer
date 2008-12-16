/*
 * QuarkPlayer, a Phonon media player
 * Copyright (C) 2008  Tanguy Krotoff <tkrotoff@gmail.com>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "ClickMessageLineEdit.h"

#include <QtGui/QtGui>

#include <QtCore/QDebug>

ClickMessageLineEdit::ClickMessageLineEdit(QWidget * parent)
	: QLineEdit(parent) {

	_enableClickMessage = false;
	_drawClickMessage = false;
}

void ClickMessageLineEdit::paintEvent(QPaintEvent * event) {
	QLineEdit::paintEvent(event);

	if (_enableClickMessage && _drawClickMessage && !hasFocus() && text().isEmpty()) {
		QPainter p(this);
		//No italic, difficult to read
		//QFont f = font();
		//f.setItalic(true);
		//p.setFont(f);

		p.setPen(palette().color(QPalette::Disabled, QPalette::Text));

		//FIXME: fugly alert!
		// qlineedit uses an internal qstyleoption set to figure this out
		// and then adds a hardcoded 2 pixel interior to that.
		// probably requires fixes to Qt itself to do this cleanly
		// see define horizontalMargin 2 in qlineedit.cpp
		QStyleOptionFrame opt;
		initStyleOption(&opt);
		QRect cr = style()->subElementRect(QStyle::SE_LineEditContents, &opt, this);
		cr.setLeft(cr.left() + 2);
		cr.setRight(cr.right() - 2);

		p.drawText(cr, Qt::AlignLeft | Qt::AlignVCenter, _clickMessage);
	}
}

void ClickMessageLineEdit::focusInEvent(QFocusEvent * event) {
	if (_enableClickMessage && _drawClickMessage) {
		_drawClickMessage = false;
		update();
	}
	QLineEdit::focusInEvent(event);
}

void ClickMessageLineEdit::focusOutEvent(QFocusEvent * event) {
	if (_enableClickMessage && text().isEmpty()) {
		_drawClickMessage = true;
		update();
	}
	QLineEdit::focusOutEvent(event);
}

void ClickMessageLineEdit::setClickMessage(const QString & message) {
	_enableClickMessage = true;
	_clickMessage = message;
	_drawClickMessage = text().isEmpty();
	update();
}

void ClickMessageLineEdit::setText(const QString & text) {
	if (_enableClickMessage) {
		_drawClickMessage = text.isEmpty();
		update();
	}
	QLineEdit::setText(text);
}
