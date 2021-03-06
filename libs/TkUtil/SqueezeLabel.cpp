/*
 * QuarkPlayer, a Phonon media player
 * Copyright (C) 2008  Trolltech ASA. All rights reserved.
 * Copyright (C) 2008  Benjamin C. Meyer <ben@meyerhome.net>
 * Copyright (C) 2008-2009  Tanguy Krotoff <tkrotoff@gmail.com>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "SqueezeLabel.h"

#include "TkUtilLogger.h"

SqueezeLabel::SqueezeLabel(QWidget * parent)
	: QLabel(parent) {
}

SqueezeLabel::SqueezeLabel(const QString & text, QWidget * parent)
	: QLabel(text, parent) {
}

QString SqueezeLabel::plainText() const {
	QString tmp = text();
	tmp.replace(QRegExp("<[^>]*>"), "");
	return tmp;
}

void SqueezeLabel::paintEvent(QPaintEvent * event) {
	//FIXME: here we have a graphical bug
	//in case the font is bold or italic since we assume here
	//the font is a regular one without these attributes
	QFontMetrics fm = fontMetrics();
	if (fm.width(plainText()) > contentsRect().width()) {
		QString elided = fm.elidedText(plainText(), Qt::ElideMiddle, width());
		QString oldText = text();
		setText(elided);
		QLabel::paintEvent(event);
		setText(oldText);
	} else {
		QLabel::paintEvent(event);
	}
}

QSize SqueezeLabel::minimumSizeHint() const {
	QSize size = QLabel::minimumSizeHint();
	size.setWidth(-1);
	return size;
}
