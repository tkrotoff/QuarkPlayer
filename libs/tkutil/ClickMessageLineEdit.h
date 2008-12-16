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

#ifndef CLICKMESSAGELINEEDIT_H
#define CLICKMESSAGELINEEDIT_H

#include <tkutil/tkutil_export.h>

#include <QtGui/QLineEdit>
#include <QtGui/QPalette>

#include <QtCore/QString>

class QMouseEvent;
class QKeyEvent;
class QFocusEvent;

/**
 * QLineEdit with a tooltip that disappears when the user clicks on it.
 *
 * Inspired by KLineEdit, most of the code is from klineedit.cpp.
 *
 * @see QLineEdit
 * @see KLineEdit
 * @author Tanguy Krotoff
 */
class TKUTIL_API ClickMessageLineEdit : public QLineEdit {
	Q_OBJECT
public:

	ClickMessageLineEdit(QWidget * parent = 0);

public slots:

	/**
	 * This makes the line edit display a grayed-out hinting text as long as the user didn't enter any text.
	 * It is often used as indication about the purpose of the line edit.
	 */
	void setClickMessage(const QString & message);

	void setText(const QString & text);

private:

	void paintEvent(QPaintEvent * event);

	void focusInEvent(QFocusEvent * event);

	void focusOutEvent(QFocusEvent * event);


	bool _enableClickMessage;

	QString _clickMessage;

	bool _drawClickMessage;
};

#endif	//CLICKMESSAGELINEEDIT_H
