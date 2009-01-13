/*
 * QuarkPlayer, a Phonon media player
 * Copyright (C) 2008  Trolltech ASA. All rights reserved.
 * Copyright (C) 2008  Benjamin C. Meyer <ben@meyerhome.net>
 * Copyright (C) 2008-2009  Tanguy Krotoff <tkrotoff@gmail.com>
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

#ifndef SQUEEZELABEL_H
#define SQUEEZELABEL_H

#include <tkutil/tkutil_export.h>

#include <QtGui/QLabel>

/**
 * Paint the current text with an elided in the middle
 * if the width of the text is less then the width of the widget.
 *
 * Taken from Trolltech Browser demo and Arora source code
 * @see http://github.com/Arora/arora/tree/master
 */
class TKUTIL_API SqueezeLabel : public QLabel {
public:

	SqueezeLabel(QWidget * parent = 0);

	SqueezeLabel(const QString & text, QWidget * parent = 0);

protected:

	void paintEvent(QPaintEvent * event);
};

#endif	//SQUEEZELABEL_H
